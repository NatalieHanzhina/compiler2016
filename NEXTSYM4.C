
		/* FILE "NEXTSYM.C" - LEXER */
      #include <stdio.h>	
      #include <string.h>	

      #define MAX_IDENT 78	 /* max ident length	*/

      #define PLUS	1
      #define MINUS    -1

      extern FILE *d;	 	 /* for debug printing		*/
      extern FILE *t;		 /* for debug printing		*/

/*-------------------------- N E X T S Y M -----------------------------*/
void		 nextsym	() 	/* Processing of the next character (token) 
							in the text of the user program the function 
							argument - the value of the external variable ch 
							- the current text of the user program text, the 
							result - the code of the token collected or recognized
							by the parser - the value of the external variable symbol
							*/
	{
      char firstch;	 /* Auxiliary variable for storing the first letter of the number, the identifier and the keyword       */
#ifdef REE
printf("вошли в nextsym()\n");
#endif
	start:
      if (ch!=endoffile)
	{
		/* If not the end of the file, then continue parsing the tokens 	*/
	if ( nextsymbol==0)
	{	/* If not a special case, when the lexeme twopoints is parsed, then parsing occurs in the usual way		*/
		/* Skip spaces, end lines and tabs: 	*/
	while  ((ch == ' ') || (ch=='\t')||(ch=='\n')) ch=nextch();
		/* Remember the initial positions of the token analysis:     	*/
       token.linenumber = positionnow.linenumber;
       token.charnumber = positionnow.charnumber;
		/* Define a letter or number? 			*/
	firstch=ch;	/* Remember the letter: if it's a letter or a number, */
/* Then we process any letter or any digit the same way	*/
	if  (ch>= '0' &&  ch<='9') ch='0';
	if ((ch>= 'a' &&  ch<='z')||(ch>='A' && ch <= 'Z'))  ch='a';
	lname=0; 	/* Number of the element of the array - name, keyword or string constant				*/
	switch(ch)
	   {
	   case '*':
		ch=nextch( );
		symbol=star;
		break;
	   case '/':
		symbol=slash;
		ch=nextch( );
		break;
	   case '=':
		symbol=equal;
		ch=nextch( );
		break;
	   case ',':
		symbol=comma;
		ch=nextch( );
		break;
	   case ';':
		symbol=semicolon;
		ch=nextch( );
		break;
	   case '<':
		ch=nextch( );
		if (ch=='=')
		 {
		   symbol=laterequal ;
		   ch=nextch( );
		 }
		 else if (ch=='>')
		 {
		  symbol=latergreater;
		  ch=nextch( );
		 }
		 else
		   symbol=later;
		 break;
	   case '>':
		 ch=nextch( );
		 if (ch=='=')
		  {
		    symbol=greaterequal;
		    ch=nextch( );
		  }
		  else
		    symbol=greater;
		  break;
	    case ':':
		  ch=nextch( );
		  if (ch=='=')
		  {
		     symbol=assign;     
		     ch=nextch( );
		   }
		   else
		     symbol=colon;
		   break;
	     case '.':

		   ch=nextch( );

		   if (ch=='.')
		   {
		     symbol=twopoints;
		     ch=nextch( );
		    }
		    else
		     {
		     symbol=point;
			
		     }
		    break;
	      case '^':
		    symbol=arrow;
		    ch=nextch( );
		    break;
	      case '(':
		    ch=nextch( );
		    if (ch=='*')
		    { /* Skip comment */
		       do
			  {do ch=nextch( );
			  while (ch !='*' && ch !=endoffile);
			  ch=nextch();
			  }
		       while (ch !=')' && ch !=endoffile);
		       if(ch==')')
			  {ch=nextch();
			  goto start;
			  }
		       else
			  {symbol=endoffile;
			  Error(E_NRCOMM);
			  }
		     } /*Finished skipping a comment */
		     else
		       symbol=leftpar;
		     break;
	       case ')':
		     symbol=rightpar;
		     ch=nextch( );
		     break;
	       case '[':
		     symbol=lbracket;
		     ch=nextch( );
		     break;
	       case ']':
		     symbol=rbracket;
		     ch=nextch( );
		     break;
	       case '{':
			/* Skip comment */
		     do ch=nextch();
		     while(ch!='}' && ch !=endoffile);
		     if(ch=='}')
			{ch=nextch();
			goto start;
			}
		     else
			{
			Error(E_NRCOMM);
			symbol=endoffile;
			}
		     break;
			/* Finished skipping a comment */
	       case '0':
		     ch=firstch;
		     number( );
		     break;
	       case 'a':
		     ch=firstch;
		     while (((ch>='a' &&  ch<='z')||
			(ch>='A' &&  ch<='Z')||
			(ch>='0'  &&  ch<='9')||
			ch == '_') && lname<MAX_IDENT)
				{
				/* Capitalization of lowercase letters: 	*/
				if(ch<'a'&& ch!='_' && ch>'9')
					ch+=LATDIF;
				addsym( );
				ch=nextch( );
				}
		     if(lname<15)
			testkey();
		     else
			{
			symbol=ident;
			if(lname==MAX_IDENT)
				while ((ch>='a' &&  ch<='z')||
					(ch>='A' &&  ch<='Z')||
					(ch>='0'  &&  ch<='9')||
					ch == '_')
						ch=nextch();
			}
		     if(symbol==ident)
			SearchInTable(name);
		     break;
	       case '"':
		       ch=nextch( );
		       while (ch != '"' &&  ch!=endoffile && lname<MAXLEN)
			  {
			   strings[lname]=ch;
			   lname++;
			   strings[lname]='\0';
			   ch=nextch( );
			  }
			symbol = stringc;
			ch=nextch( );
			if(lname>MAXLEN-1)
				{
				Error(E_STRC);
				while(ch!='"' && ch!=endoffile)
					ch=nextch();
				}
			break;
	       case '\'':
		if((ch=nextch( ))=='\'')
			if((ch=nextch())!='\'')
				{
				Error(E_CHARC);
				onesymbol=0;
				goto ass;
				}
		onesymbol=ch;
		if((ch=nextch())!='\'')
			Error(E_CHARC);
		else
			ch=nextch();
		ass:
		symbol=charc;
		break;
	       case '+':
		      symbol=plus;
		      ch=nextch( );
		      break;
		case '-':
		      symbol=minus;
		      ch=nextch( );
		      break;
		case endoffile:
		      symbol=endoffile;
		      break;
		default:
		     /*Unidentified symbol          */
#ifdef RDB

		      switch(ch)
			{case ' ':
				fprintf(d,"space\n");
				break;
			case '\n':
				fprintf(d,"new string\n");
				break;
			case '\0':
				fprintf(d,"the end of string\n");
				break;
			default:
				fprintf(d,"unidentified symbol\n");
			}
#endif 
		      Error(E_BSYM);
		      ch=nextch();
		      goto start;
		  }	/* switch */
		}	/* if(nextsymbol==0) */
	       else
	       {
	       /* The case where the character twopoints is parsed   */

	       token.linenumber=next.linenumber;
	       token.charnumber=next.charnumber;
	       symbol=nextsymbol;
	       nextsymbol=0;
	       }	/* else */
	      }		/* if(ch!=endoffile) */
	else
	    { 
#ifdef RDB
	    puts("NEXTSYM2 - The end of the file");
#endif
	 symbol=endoffile;
	    }
#ifdef RDB
	if(symbol==ident)
		{fprintf(d,"Identifier. ");
		fprintf(d,"%s\n",name);
		puts(name);}
	else if(symbol==charc)
		{fprintf(d,"char. ");
		fprintf(d,"%c\n",onesymbol);}
	else if(symbol==stringc)
		{fprintf(d,"String. ");
		fprintf(d,"%s\n",strings);}
	else if(symbol==intc)
		fprintf(d,"integer const=%d.\n",nmbi);
	else if(symbol==floatc)
		fprintf(d,"real const=%f.\n",nmbf);
	else fprintf(d,"Either a delimiter or a keyword.\n");
#endif
#ifdef RT
fprintf(t,"symbol code %d\n",symbol);
#endif
#ifdef REE
printf("	nextsym() is off\n");
#endif
}	/* nextsym() */

/*------------------------------- A D D S Y M --------------------------*/
void		 addsym		()      /* Accumulates symbols in the array name - 
								the results for the assembly of the identifier or the keyword 
								 are the values of the external variables lname - 
								 the length of the collected character string -
								 and name - the string itself		*/
		    {
#ifdef REE
printf("Entered into addsym()\n");
#endif
		    name[lname]=ch;
		    name[++lname]='\0';
#ifdef REE
printf("	Leave addsym()\n");
#endif
		    }

/*------------------------------ T E S T K E Y -------------------------*/
void 		 testkey	()  	/* Processing of Latin keywords and identifiers, 
								the formation of the code of the lexeme argument
								- the value of the external variable - name 
								(identifier or keyword) result - the value of the 
								external variable symbol - the code of the token		
								*/
		    {
		    struct key
			      {
			      unsigned codekey;
			      char     namekey[15];
			      };
		    static struct key  keywords[64]=
		    /* Table of keywords and their codes 		*/
		      {
			  { ident," "},
			  { dosy, "do"},
			  { ifsy, "if"},
			  { insy, "in"},
			  { ofsy, "of"},
			  { orsy, "or"},
			  { tosy, "to"},
			  { ident,"  "},
			  { andsy,"and"},
			  { divsy,"div"},
			  { endsy,"end"},
			  { forsy,"for"},
			  { modsy,"mod"},
			  { nilsy,"nil"},
			  { notsy,"not"},
			  { setsy,"set"},
			  { varsy,"var"},
			  { ident,"   "},
			  { casesy,"case"},
			  { elsesy,"else"},
			  { filesy,"file"},
			  { gotosy,"goto"},
		//	  { readsy,"read"},// 
			  { onlysy,"only"},
			  { thensy,"then"},
			  { typesy,"type"},
			  { unitsy,"unit"},
			  { usessy,"uses"},
			  { withsy,"with"},
			  { ident, "    "},
			  { arraysy,"array"},
			  { beginsy,"begin"},
			  { constsy,"const"},
			  { labelsy,"label"},
			  { untilsy,"until"},
			  { whilesy,"while"},
		//	  { writesy,"write"},//
			  { ident,  "     "},
			  { exportsy,"export"},
			  { importsy,"import"},
			  { downtosy,"downto"},
			  { modulesy,"module"},
			  { packedsy,"packed"},
		//	  { readlnsy,"readln"},
			  { recordsy,"record"},
			  { repeatsy,"repeat"},
			  { vectorsy,"vector"},
			  { stringsy,"string"},
			  { ident,   "      "},
			  { forwardsy,"forward"},
			  { processsy,"process"},
			  { programsy,"program"},
			  { segmentsy,"segment"},
		//	  { writelnsy,"writeln"},
			  { ident,    "       "},
			  { functionsy,"function"},
			  { separatesy,"separate"},
			  { ident,     "        "},
			  { interfacesy,"interface"},
			  { proceduresy,"procedure"},
			  { qualifiedsy,"qualified"},
			  { ident,      "         "},
			  { ident,      "          "},
			  { ident,      "           "},
			  { ident,      "            "},
			  { ident,      "             "},
			  { implementationsy,"implementation"},
			  { ident,        "              "}
			   };

			   /* Array of line numbers with code ident: 	*/
	unsigned short last[15]=
			   {
			   -1,0,7,17,28,35,45,50,53,57,58,59,60,61,63
			   };

	unsigned short i;  /* Auxiliary variable to search for collected */
/* Tokens in the keyword table 				*/
#ifdef REE
printf("Enter testkey()\n");
#endif
			strcpy(keywords[last[lname]].namekey,name);
			i = last[lname-1]+1;
			while (strcmp(keywords[i].namekey,name)!=0) i++;
			symbol=keywords[i].codekey;
#ifdef REE
printf("	Leave testkey()\n");
#endif
	    }


/*--------------------------- N U M B E R ------------------------------*/
void             number		()   	/* Processing of an integer or real constant,
									the formation of its value argument - the value
									of an external variable name - the sequence of
									characters that give a constant in the user program 
									result - the value of one of the external variables:
									nmbi - if the constant is integer, or nmbf - if the 
									constant is real	*/

{
char 	lit1,lit2;	/* Auxiliary variables for processing the structure in which the point occurred     			*/
int 	ord_correct=0,  /* For constants with positive order - the order on which 
						the real number, given by the user, differs from the 
						normalized (towards the decrease of the mantissa); For 
						constants with negative order - the same number +1, but 
						only in the direction of increasing the mantissa				
						*/
	digit,		/* Integer representation        	*/
	order=0,	/* The order of the real number                	*/
	differ=0,	/* Auxiliary variable for processing     */
	/* Fractional part of the real number          			*/
	intflag=FALSE,	/* Takes the value FALSE if the integer part */
/* Does not exceed the maximum permissible value of an integer constant, TRUE - otherwise;				*/
	ordflag=PLUS;	/* Takes the value MINUS if the order in the real constant is negative, PLUS otherwise	*/
#ifdef REE
printf("Enter number()\n");
#endif
nmbi=nmbf=0;
while( ch >= '0' && ch <= '9')	/* The whole part of the constant is processed	*/
	{
	digit=ch-'0';
	if (nmbi<maxint/10 || (nmbi==maxint/10 && digit<=maxint%10))
		{
		nmbi=10*nmbi+digit;	/* Form the value of an integer constant							*/
		nmbf=10*nmbf+digit;	/* We form the value of the real constant				*/
#ifdef RDB
	printf("nmbf==%g\n",nmbf);
#endif
		ch=nextch();
		}
	else
		{
		intflag=TRUE;	/* Remember that the whole formed */
/* constant exceeds the limit					*/
		nmbf=10*nmbf+digit;	/* Continue to form the value of the real constant					*/
#ifdef RDB
		printf("nmbf==%g\n", nmbf);
#endif
		ch=nextch();
		}
	if (nmbf!=0) ord_correct--; 	/* Consider the number of significant digits of the integer part						*/

	}	/* while... */
if (ch== '.' || ch=='E' || ch == 'e')
	{
	if (ch=='.')
		{
		/* Case of ".." is possible  				*/
		next.linenumber=positionnow.linenumber;
		next.charnumber=positionnow.charnumber;
		lit1=ch; 	/* Remember the first point 		*/
		ch=nextch();
		if (ch=='.')
			{
			nextsymbol=twopoints;
			ch=nextch();	/* We have the case 3..5, i.e. Issue 
							the integer code to the parser, and in
							the auxiliary variable nextsymbol, 
							remember the code of the next token 
							that was already parsed, - twopoints		
							*/
			goto intnumber;	/* Transition to the analysis of an integer constant					*/
			}
		else
			{
			lit2=ch;	/* Remember the last letter 	*/
			ch=lit1; 	/* Restore the previous letter							*/
			}
		}  	/* if(ch=='.') */
	/* So, the constant is real:				*/
	//symbol = floatc;
	switch(ch)
		{
		case '.':
			order=0;
			ch=lit2;
			/* чтение дробной части     			*/
			while (ch>='0' && ch<='9')
				{
				digit=ch-'0';
				nmbf=10*nmbf+digit;	/* Continue to form the value of the real constant   		*/
#ifdef RDB
	printf("nmbf==%g, order==%d ",nmbf,order);
#endif
				differ--;	/* Consider how much it is necessary to 
							reduce the order of the number in order 
							to obtain a result equal to the user-specified number 	
							*/
				if (nmbf==0) ord_correct++;	/* считаем количество нулей после точки 				*/
#ifdef RDB
	printf("ord_correct==%d, differ==%d\n",ord_correct,differ);
#endif
				ch=nextch();
				} 	/* while... */
			if (ch== 'e' || ch=='E')
				goto ordscan;
			break;
		case 'e':
		case 'E':
		ordscan:	/*  Reading order:  			*/
			ch=nextch( );
			if (ch == '-')
				{
				ordflag=MINUS;
				ord_correct++;  /* The maximum permissible degree 
								differs in absolute value by one from 
								the minimum permissible					*/
				ch=nextch();
				}
			else
				if( ch == '+')
					ch=nextch();
			if(ch < '0' || ch > '9')
				Error(E_BCONST);	/* If a non-digit character is encountered in the 
									processed constant, then we issue the appropriate
									error message, order = 0; */
									/* Form the absolute value of the order, for example,
									for the constant 56.39e-4 order = 4		*/
			while (ch>='0' && ch<='9')
				{
				digit=ch-'0';
				order=order*10+digit;
#ifdef RDB
	printf("\nCheck: order=%d, MAXORDER=%d, ord_correct=%d\n\n",
		order,MAXORDER,ord_correct);
#endif
				if(order>MAXORDER+ord_correct*ordflag)
	/* Too large or too small real constant */
					{
					if(ordflag+1)
						Error(E_BREALC);
					else
						Error(E_LREALC);
					nmbf=0;
					while (ch>='0' && ch<='9')
	/* Skip letters to the end of the constant:			*/
						ch=nextch();
#ifdef REE
printf("	LEave number()\n");
#endif
					return;	/* Good-bye, my love,	*/
	/* good-bye!							*/
					}	/* if(order... */
				ch=nextch();
				};  	/* while... */
			break;
		}; 	/* switch */
	order+=differ*ordflag;	/* Compensation for the too large value of
							the fixed part of the constant formed by the 
							corresponding reduction of the order         		
							*/
#ifdef RDB
	printf("=== Cформирован order == %d\n",order);
#endif
	/* Next - the final formation of the value of the constant in nmbf, multiplying by 10 in the order:			*/
	if(order<0)	/* There are re-appropriations for purely technical purposes:						*/
		{order=-order;
		ordflag=-ordflag;
		}
	if (ordflag+1)
		for(differ=0;differ<order;differ++)
			{
			nmbf*=10;
#ifdef RDB
	printf("nmbf==%g, order==%d\n",nmbf,order);
#endif
			}
	else
		for(differ=0;differ<order;differ++)
			{
			nmbf/=10;
#ifdef RDB
	printf("nmbf==%e, order==%d\n",nmbf,order);
#endif
			}
#ifdef RDB
	printf("**** nmbf==%g, order==%d\n",nmbf,order);
#endif
#ifdef REE
printf("	Leave number()\n");
#endif
	return;	/*Leave number (); The value of the real constant is formed in the external variable nmbf			*/
	}	/* if (ch=='.' || ch=='E' || ch=='e') */
intnumber:
if (intflag==TRUE)	/* The whole constant exceeds the limit		*/
	{
	Error(E_BINTC);	/* Error!				*/
	nmbi=0;
	}

	/* Return the integer code: 				*/
symbol=intc;
#ifdef REE
printf("	Leave number()\n");
#endif
}	/* number() */

/*===================== T H E   E N D   O F   F I L E ==================*/
