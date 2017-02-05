
		/* ФАЙЛ "NEXTSYM.C" - ЛЕКСИЧЕСКИЙ АНАЛИЗАТОР */
      #include <stdio.h>	 /* Файл определений стандартного в/в   */
      #include <string.h>	 /* Файл станд. ф-ий работы со строками */

      #define MAX_IDENT 78	 /* Максимальная длина идентификатора	*/

      #define PLUS	1
      #define MINUS    -1

      extern FILE *d;	 	 /* для отладочных печатей		*/
      extern FILE *t;		 /* для отладочных печатей		*/

/*-------------------------- N E X T S Y M -----------------------------*/
void		 nextsym	() 	/* обработка следующего сим-	*/
			/* вола ( лексемы ) в тексте программы пользо-	*/
			/* вателя					*/
				/* аргумент функции - значение внешней	*/
			/* переменной ch - текущая литера текста про-	*/
			/* граммы пользователя				*/
				/* результат - код собранной или опо-	*/
			/* знанной анализатором лексемы - значение 	*/
			/* внешней переменной symbol			*/
	{
      char firstch;	 /* вспомогательная переменная для хранения     */
	/* первой литеры числа,идентификатора ии ключевого слова        */
#ifdef REE
printf("вошли в nextsym()\n");
#endif
	start:
      if (ch!=endoffile)
	{
		/* если не  конец файла, то продолжение разбора лексем 	*/
	if ( nextsymbol==0)
	{	/* если не  особый случай, когда разбирается лексема	*/
	/* twopoints ,то разбор  происходит обычным образом 		*/
		/* пропустим  пробелы, концы строки и знаки табуляции: 	*/
	while  ((ch == ' ') || (ch=='\t')||(ch=='\n')) ch=nextch();
		/* запомним начальные позиции  разбора лексемы:     	*/
       token.linenumber = positionnow.linenumber;
       token.charnumber = positionnow.charnumber;
		/* определим,буква  или  цифра? 			*/
	firstch=ch;	/* запоминаем литеру: если это буква или цифра,	*/
	/* то любую букву или любую цифру обрабатываем однообразно	*/
	if  (ch>= '0' &&  ch<='9') ch='0';
	if ((ch>= 'a' &&  ch<='z')||(ch>='A' && ch <= 'Z'))  ch='a';
	if((ch>='А' && ch<='Я') || (ch>='а' && ch<='я')) ch='а';
	lname=0; 	/* номер элемента массива - имени, ключевого 	*/
	/* слова или строковой константы				*/
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
		     symbol=assign;     /* Символ  присваивания */
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
		    { /* пропускаем комментарий */
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
		     } /* закончили пропуск комментария */
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
			/* пропускаем комментарий */
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
			/* закончили пропуск комментария */
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
				/* замена заглавных букв строчными: 	*/
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
	       case 'а':
		     ch=firstch;
		     while ((ch=='_' || (ch>='А' && ch<='Я')
			|| (ch>='а' && ch<='я')
			|| (ch>='0' && ch<='9'))&& lname<MAX_IDENT)
				{
				/* замена заглавных букв строчными: 	*/
				if(ch<'а' && ch!='_')
					if(ch<'Я')
						ch+=RUSDIF1;
					else
						ch+=RUSDIF2;
				addsym( );
				ch=nextch( );
				}
		     if(lname<10)
			testkey1();
		     else
                        {
			symbol=ident;
			if(lname==MAX_IDENT)
				while ((ch>='А' &&  ch<='п')||
					(ch>='р' &&  ch<='я')||
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
			printf("eof");
		      symbol=endoffile;
		      break;
		default:
		     /* Символ неопознанный          */
#ifdef RDB
		      switch(ch)
			{case ' ':
				fprintf(d,"пробел\n");
				break;
			case '\n':
				fprintf(d,"новая строка\n");
				break;
			case '\0':
				fprintf(d,"конец строки\n");
				break;
			default:
				fprintf(d,"неизвестный символ\n");
			}
#endif
		      Error(E_BSYM);
		      ch=nextch();
		      goto start;
		  }	/* switch */
		}	/* if(nextsymbol==0) */
	       else
	       {
	       /* случай , когда происходит  разбор  символа  twopoints */

	       token.linenumber=next.linenumber;
	       token.charnumber=next.charnumber;
	       symbol=nextsymbol;
	       nextsymbol=0;
	       }	/* else */
	      }		/* if(ch!=endoffile) */
	else
	    {
#ifdef RDB
	    puts("NEXTSYM2 - пойман конец файла");
#endif
	 symbol=endoffile;
	    }
#ifdef RDB
	if(symbol==ident)
		{fprintf(d,"Идентификатор. ");
		fprintf(d,"%s\n",name);
		puts(name);}
	else if(symbol==charc)
		{fprintf(d,"Символьная константа. ");
		fprintf(d,"%c\n",onesymbol);}
	else if(symbol==stringc)
		{fprintf(d,"Строковая константа. ");
		fprintf(d,"%s\n",strings);}
	else if(symbol==intc)
		fprintf(d,"Константа типа integer=%d.\n",nmbi);
	else if(symbol==floatc)
		fprintf(d,"Константа типа real=%f.\n",nmbf);
	else fprintf(d,"либо разделитель,либо ключевое слово.\n");
#endif
#ifdef RT
fprintf(t,"код символа %d\n",symbol);
#endif
#ifdef REE
printf("	вышли из nextsym()\n");
#endif
}	/* nextsym() */

/*------------------------------- A D D S Y M --------------------------*/
void		 addsym		()      /* накапливает символы в масси-	*/
			/* ве name - для сборки идентификатора или клю-	*/
			/* чевого слова					*/
				/* результаты - значения внешних пере-	*/
			/* менных lname - длина собранной символьной 	*/
			/* строки - и name - сама строка		*/
		    {
#ifdef REE
printf("вошли в addsym()\n");
#endif
		    name[lname]=ch;
		    name[++lname]='\0';
#ifdef REE
printf("	вышли из addsym()\n");
#endif
		    }

/*------------------------------ T E S T K E Y -------------------------*/
void 		 testkey	()  	/* обработка латинских ключевых */
			/* слов и идентификаторов, формирование кода 	*/
			/* лексемы					*/
				/* аргумент - значение внешней перемен-	*/
			/* name - имя ( идентификатор или ключевое сло-	*/
			/* во ) 					*/
				/* результат - значение внешней пере-	*/
			/* менной symbol - код лексемы			*/
		    {
		    struct key
			      {
			      unsigned codekey;
			      char     namekey[15];
			      };
		    static struct key  keywords[64]=
		    /* таблица ключевых слов и их кодов 		*/
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

			   /* массив номеров строк с кодом ident: 	*/
	unsigned short last[15]=
			   {
			   -1,0,7,17,28,35,45,50,53,57,58,59,60,61,63
			   };

	unsigned short i;  /* вспомогат.переменная для поиска собранной */
	/* лексемы в таблице ключевых слов 				*/
#ifdef REE
printf("вошли в testkey()\n");
#endif
			strcpy(keywords[last[lname]].namekey,name);
			i = last[lname-1]+1;
			while (strcmp(keywords[i].namekey,name)!=0) i++;
			symbol=keywords[i].codekey;
#ifdef REE
printf("	вышли из testkey()\n");
#endif
	    }


/*-------------------------- T E S T K E Y 1 ---------------------------*/
void 		 testkey1	()  	/* обработка ключевых слов и	*/
			/* идентификаторов, записанных на кириллице,	*/
			/* формирование кода лексемы			*/
				/* аргумент - значение внешней перемен-	*/
			/* name - имя ( идентификатор или ключевое сло-	*/
			/* во ) 					*/
				/* результат - значение внешней пере-	*/
			/* менной symbol - код лексемы			*/
	    {
	      struct key
		      {
		       unsigned codekey;
		       char namekey[10];
		       };
	      static struct key  keywords[49]=
	      /*  таблица ключевых слов и их кодов  			*/
		 {
		   { andsy,"и"},
		   { ident," "},
		   { untilsy,"до"},
		   { ofsy,   "из"},
		   { gotosy, "на"},
		   { notsy,  "не"},
		   { tosy,   "по"},
		   { thensy, "то"},
		   { ident,  "  "},
		   { divsy, "дел"},
		   { forsy, "для"},
		   { orsy,  "или"},
		   { dosy,  "исп"},
		   { modsy, "мод"},
		   { withsy,"над"},
		   { nilsy, "нич"},
		   { typesy,"тип"},
		   { ident, "   "},
		   { ifsy,    "если"},
		   { setsy,   "множ"},
		   { whilesy, "пока"},
		   { filesy,  "файл"},
		   { packedsy,"упак"},
		   { insy,    "элем"},
		   { ident,  "    "},
		   { casesy, "выбор"},
		   { elsesy, "иначе"},
		   { endsy,  "конец"},
		   { constsy,"конст"},
		   { labelsy,"метка"},
		   { varsy,  "перем"},
		   { ident,  "     "},
		   { downtosy,"вниздо"},
		   { importsy,"импорт"},
		   { recordsy,"запись"},
		   { beginsy, "начало"},
		   { arraysy, "массив"},
		   { modulesy,"модуль"},
		   { repeatsy,"повтор"},
		   { onlysy,  "только"},
		   { ident,   "      "},
		   { functionsy, "функция"},
		   { exportsy,   "экспорт"},
		   { ident,      "       "},
		   { qualifiedsy,"квалифик"},
		   { ident,      "        "},
		   { programsy,  "программа"},
		   { proceduresy,"процедура"},
		   { ident,      "         "}
		    };

		    /* массив номеров строк с кодом ident:	 	*/
	   unsigned short last[10]=
		      {
		      -1,1,8,17,24,31,40,43,45,48
		      };

	   unsigned short i;/*вспомогательная переменная для поиска со-	*/
	   /* бранной лексемы в таблице ключевых слов 			*/
#ifdef REE
printf("вошли в testkey1()\n");
#endif
			  strcpy(keywords[last[lname]].namekey,name);
			  i=last[lname-1]+1;
			  while (strcmp(keywords[i].namekey,name)!=0 )
			  i++;
			  symbol=keywords[i].codekey;
#ifdef REE
printf("	вышли из testkey1()\n");
#endif
		}
/*--------------------------- N U M B E R ------------------------------*/
void             number		()   	/* обработка целой или вещест-	*/
			/* венной константы, формирование ее значения	*/
				/* аргумент - значение внешней перемен-	*/
			/* ной name - последовательность символов, за-	*/
			/* дающих константу в программе пользователя	*/
				/* результат - значение одной из внеш-	*/
			/* них переменных: nmbi - если константа целая,	*/
			/* или nmbf - если константа вещественная	*/

{
char 	lit1,lit2;	/* вспомогат.переменные для обработки конструк-	*/
	/* ции, в которой встретилась точка     			*/
int 	ord_correct=0,  /* для констант с положительным порядком - по-	*/
	/* рядок, на который вещественное число, заданное пользователем,*/
	/* отличается от нормализованного ( в сторону уменьшения ман-	*/
	/* тиссы );							*/
	/* для констант с отрицательным порядком - то же число +1, но	*/
	/* только в сторону увеличения мантиссы				*/
	digit,		/* целочисленное представление литеры         	*/
	order=0,	/* порядок вещественного числа                	*/
	differ=0,	/* вспомогательная переменная для обработки     */
	/* дробной части вещественного числа          			*/
	intflag=FALSE,	/* принимает значение FALSE, если целая часть 	*/
	/* не превышает максимально допустимого значения целой констан-	*/
	/* ты, TRUE - в противном случае;				*/
	ordflag=PLUS;	/* принимает значение MINUS, если порядок в ве-	*/
	/* щественной константе отрицательный,PLUS - в противном случае	*/
#ifdef REE
printf("вошли в number()\n");
#endif
nmbi=nmbf=0;
while( ch >= '0' && ch <= '9')	/* обрабатывается целая часть константы	*/
	{
	digit=ch-'0';
	if (nmbi<maxint/10 || (nmbi==maxint/10 && digit<=maxint%10))
		{
		nmbi=10*nmbi+digit;	/* формируем значение целой  	*/
	/* константы							*/
		nmbf=10*nmbf+digit;	/* формируем значение вещест-	*/
	/* венной константы 						*/
#ifdef RDB
	printf("nmbf==%g\n",nmbf);
#endif
		ch=nextch();
		}
	else
		{
		intflag=TRUE;	/* запомнили, что формируемая целая 	*/
	/* константа превышает предел					*/
		nmbf=10*nmbf+digit;	/* продолжаем формировать зна-	*/
	/* чение вещественной константы					*/
#ifdef RDB
		printf("nmbf==%g\n", nmbf);
#endif
		ch=nextch();
		}
	if (nmbf!=0) ord_correct--; 	/* считаем количество значащих 	*/
	/* цифр целой части						*/

	}	/* while... */
if (ch== '.' || ch=='E' || ch == 'e')
	{
	if (ch=='.')
		{
		/* возможен случай ".." 				*/
		next.linenumber=positionnow.linenumber;
		next.charnumber=positionnow.charnumber;
		lit1=ch; 	/* запоминаем первую точку 		*/
		ch=nextch();
		if (ch=='.')
			{
			nextsymbol=twopoints;
			ch=nextch();	/* имеем случай 3..5 , т.е.	*/
	/* выдадим код целого числа синтаксическому анализатору, а во  	*/
	/* вспомогательной переменной nextsymbol запомним код следующей */
	/* лексемы, которую уже разобрали, - twopoints			*/
			goto intnumber;	/* переход к анализу целой кон-	*/
	/* станты 							*/
			}
		else
			{
			lit2=ch;	/* запоминаем последнюю литеру 	*/
			ch=lit1; 	/* восстанавливаем предыдущую 	*/
	/* литеру 							*/
			}
		}  	/* if(ch=='.') */
	/* Итак, константа вещественная:				*/
	symbol = floatc;
	switch(ch)
		{
		case '.':
			order=0;
			ch=lit2;
			/* чтение дробной части     			*/
			while (ch>='0' && ch<='9')
				{
				digit=ch-'0';
				nmbf=10*nmbf+digit;	/* продолжаем 	*/
	/* формировать значение вещественной константы    		*/
#ifdef RDB
	printf("nmbf==%g, order==%d ",nmbf,order);
#endif
				differ--;	/* считаем, на сколько 	*/
	/* необходимо уменьшить порядок числа, чтобы получить результат,*/
	/*  равный заданному пользователем числу 			*/
				if (nmbf==0) ord_correct++;	/* счи-	*/
	/* таем количество нулей после точки 				*/
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
		ordscan:	/*  чтение порядка:  			*/
			ch=nextch( );
			if (ch == '-')
				{
				ordflag=MINUS;
				ord_correct++;  /* максимально допусти-	*/
	/* мая степень отличается по абсолютному значению на единицу от	*/
	/* минимально допустимой					*/
				ch=nextch();
				}
			else
				if( ch == '+')
					ch=nextch();
			if(ch < '0' || ch > '9')
				Error(E_BCONST);	/* если в обра-	*/
	/* батываемой константе встретилась литера, отличная от цифры,	*/
	/* то выдаем соответствующее сообщение об ошибке		*/
			order=0;    /* формируем абсолютное значение 	*/
	/* порядка, например, для константы 56.39е-4  order=4		*/
			while (ch>='0' && ch<='9')
				{
				digit=ch-'0';
				order=order*10+digit;
#ifdef RDB
	printf("\nПроверка: order=%d, MAXORDER=%d, ord_correct=%d\n\n",
		order,MAXORDER,ord_correct);
#endif
				if(order>MAXORDER+ord_correct*ordflag)
	/* слишком большая или слишком маленькая вещественная константа */
					{
					if(ordflag+1)
						Error(E_BREALC);
					else
						Error(E_LREALC);
					nmbf=0;
					while (ch>='0' && ch<='9')
	/* пропускаем литеры до конца константы:			*/
						ch=nextch();
#ifdef REE
printf("	вышли из number()\n");
#endif
					return;	/* Good-bye, my love,	*/
	/* good-bye!							*/
					}	/* if(order... */
				ch=nextch();
				};  	/* while... */
			break;
		}; 	/* switch */
	order+=differ*ordflag;	/* компенсация за слишком большое зна-	*/
	/* чение фикcированной части сформированной нами константы пу-	*/
	/* тем соответствующего уменьшения порядка          		*/
#ifdef RDB
	printf("=== Cформирован order == %d\n",order);
#endif
	/* Далее - окончательное формирование значения константы в nmbf,*/
	/* умножение на 10 в степени order:				*/
	if(order<0)	/* Здесь идут переприсваивания в чисто техни-	*/
	/* ческих целях:						*/
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
printf("	вышли из number()\n");
#endif
	return;	/* покидаем number(); значение вещественной константы 	*/
	/* сформировано во внешней переменной nmbf			*/
	}	/* if (ch=='.' || ch=='E' || ch=='e') */
intnumber:
if (intflag==TRUE)	/* целая константа превышает предел		*/
	{
	Error(E_BINTC);	/* ошибочка вышла!				*/
	nmbi=0;
	}

	/* Возвращаем код целого  числа: 				*/
symbol=intc;
#ifdef REE
printf("	вышли из number()\n");
#endif
}	/* number() */

/*===================== T H E   E N D   O F   F I L E ==================*/﻿
