
/*
			  Main module.
*/
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <locale.h>
//#include <conio.h>
#include "DECW.h"        /* delimiters definition             */
#include "DECKEY.h"      /* definition of key words codes       */
#include "TREE.h"        /* identifiers usage cases */
#include "TYPES.h"       /* types codes declaration		  */
#include "TYPET.h"       /* external variables               */
#include "FUNCS.h"       /* functions types definition              */
#include "IODEFS.h"      /* i/o block defs      */
#include "RUMERCOD.h"    /* errors codes                           */
#include "Nextch.c"
#include "HASH.C"
#include "ERROR.C"
#include "LITFUNCS.C"
#include "NEXTSYM4.C"
#include "Rumtabl3.c"

#include "LAST.C"

#define NEW_PAGE 12
#define TOUT
#define labelquality 2
#define SLENGTH  100
extern int lineindex;	/* from NEXTCH			*/

FILE *mesfile,      /* error messages 		 */
     *srcfile,      /* source file 			 */
     *dstfile;      /* listing file */

  FILE *t;            /* temp file    */

char  line[ MAXLEN ],           /* current string 			 */
      *messages[ MAXMES ],  /* error mes   */
      sfile[ FNAME ],      /* file names */
      dfile[ FNAME ],      /* without dir 	*/
      fulls[ MAXLEN ] = "",	/* adding */
      fulld[ MAXLEN ] = "";	/* - dir  */

short LastInLine,              /*   significant characters in string 		 */
      ErrInx,                  /*   amount of errors by string      */
      ErrorOverflow,           /*   1-overmuch, 0-normal  */
      ErrorCount;              /*   total errors per program   */

struct textposition positionnow,
       token,
       next;
unsigned symbol,         /* code of token given by syntax analyzer	 */
	 nextsymbol,     /* code of following token	 */
	 errorcode;

	/*    tokens from lexer */

char               name[79];     /* identifier                */
int		   nmbi;         /* integer                   */
double	      	   nmbf;         /* float    */
int   	           onesymbol;    /* character                         */
unsigned  short    cnt;          /* for debug                   */
unsigned  short    lname;
unsigned           sw;
int	     	   ch;           /* current character                */
char strings[MAXLEN];

	    

unsigned long dstr[SLENGTH];


/* Definition of sets presented as bit sets, 	*/
/* which are the start chars for processed constructions			*/

unsigned

*idstarters,		/* single start char - ident */
*begpart,		/* start char for block()		*/
*rpar,			/* right parenthesis  				*/
*st_typepart,		/* start chars for typepart()		*/
*st_varpart,		/* start chars for varpart()		*/
*st_procfuncpart,	/* start chars for procfuncpart()	*/
*st_statpart,		/* start chars for statpart()		*/
*st_constant,	 	/* start chars for constant()		*/
*st_conaftersign,	/* start chars for const, 	*/
	/* after plus/minus sign					*/
*st_typ,		/* start chars for typ()		*/
*st_simpletype,	/* start chars for simpletype()	*/
*st_statement,          /* start chars for construction <operator>     */
*st_startstatement,     /* start chars for operator*/
*st_express,		/* start chars for expression 				*/
*st_termfact;		/* start chars for term, factor 			*/


/* Definition of sets presented as bit sets, 	*/
/* which are follow processed constructions			*/

unsigned

*blockfol,		/* ...after processing of main program block  */
*af_1constant,		/* ...after processing of const while calling	*/
	/* constant() from constdeclaration(), also after  */
	/* analysis of constrution of declaration variables while calling */
	/* vardeclaration() from varpart()			*/
*af_3const1,		/* ...after processing of first const	*/
	/* into simpletype()			*/
*af_4const2,		/* ...after processing of second const	*/
	/* into simpletype()			*/
*af_1typ,		/* ...after processing of type construction	*/
	/* while calling typ() from typedeclaration()		*/
*af_2typ,		/* ... after analyzing the construction of the type declaration */
/* When calling the typ () function from fixpart () */
*af_proclistparam,      /* ... after analyzing the list of procedure parameters */
*af_funclistparam,      /* ... after analyzing the list of function parameters   */
*af_blockprocfunc,      /* ... after analyzing the procedure or function block */
*af_sameparam,          /* ... after analyzing the same parameters       */
*af_factparam,          /* ... after analyzing the actual parameters      */
	/* of procedure or function                                         */
*af_oneparam,           /* ... after analyzing the standard parameter * /
/* of Procedures and functions that have one parameter                   */
*af_writeparam,         /* ...After analyzing the parameter of standard       */
	/* procedures write and writeln                                     */

*af_assignment,         /* ...After analyzing the variable in the assign statement      */
*af_compstatement,      /* ...After the analysis of the operator in the composite operator */
*af_iftrue,             /* ...After analyzing the conditional expression in the if statement*/

*af_iffalse,            /* ...After analyzing the operator of the branch "true" in if statement */

*af_whilefor,		/* ...After analyzing the conditional expression in*/
	/*  while statement					*/
*af_repeat,             /* ...After the analysis of the operator in  repeat loop body*/
*af_forassign,	        /* ...After analyzing the variable in the for statement		*/
*af_for1, 		/* ...After analyzing the expression-first boundary of the loop parameter in the for statement			*/
*af_ident;		/* ...After the identifier in the "variable"	*/

/* Definition of sets presented as bit sets, 	*/
/* for expressions				*/

unsigned

*op_rel,		/* Operations of relation over simple expressions	*/
*op_add,		/* Additive operations on terms		*/
*op_mult;		/* Multiplicative operations on factors	*/

/* Definition of sets presented as bit sets for cases of identifiers usage	*/

unsigned

*set_VARCONFUNCS,	/* Acceptable use of VARS, CONSTS, FUNCS */
*set_VARS,	/* Acceptable use of  VARS		*/
*set_TYPES, 	/*Acceptable use of  TYPES 		*/
*set_CONSTS,	/* Acceptable use of  CONSTS 		*/
*set_TYCON,     /* Acceptable use of  TYPES,CONSTS	*/
*set_FUNCS,     /* Acceptable use of  FUNCS              */
*set_PROCS,     /* Acceptable use of  PROCS              */
*set_FUNPR,     /* Acceptable use of  FUNCS,PROCS       */
*set_VARFUNPR;  /* Acceptable use of  VARS,FUNCS,PROCS  */

/* Definition of sets presented as bit sets for types codes,	*/
/* Unacceptable for use in a particular context:		*/

unsigned *illegalcodes;

/*----------------------------- M A I N --------------------------------*/
int main( argc, argv )
int argc;
char *argv[];
{
setlocale(LC_ALL,"rus");
/* Sets of CODES characters starting for different processed */
/* Constructions: */

unsigned

codes_idstart[]=	{ident,eolint},	/* Sets of CODES characters starting for different processed */
/* Constructions: */
codes_block[]=		{labelsy,constsy,typesy,varsy,functionsy,
	proceduresy,beginsy,eolint},	/* Start symbols for sections of descriptions and section of operators			*/
codes_rightpar[]=	{rightpar,eolint},
		/* right parenthesis 					*/
codes_constant[]=	{plus,minus,charc,stringc,ident,intc,
	eolint},	/* Starting symbols for the construction constant       */
codes_typ[]=		{packedsy,arrow,arraysy,filesy,setsy,recordsy,
	plus,minus,ident,leftpar,intc,charc,stringc,eolint},
			/* Start symbols for the type declaration;	*/
		/* {plus,minus,ident,leftpar,intc,charc,stringc,eolint}-*/
		/* Start symbols for the description of a simple type */

codes_statement[]=      {intc,endsy,elsesy,untilsy,ident,beginsy,ifsy,
	whilesy,repeatsy,forsy,casesy,withsy,semicolon,gotosy,eolint},
		/* Starting symbols of the construct <operator>             */
		/* (When neutralizing errors, symbols:      */
		/* {beginsy,ifsy,whilesy,repeatsy,forsy,casesy,withsy,  */
		/* semicolon,gotosy,eolint} )                           */
codes_express[]=	{ plus, minus, leftpar, lbracket, notsy, ident,
	intc, charc, stringc, nilsy, eolint },
		/* Starting symbols of an expression and a simple expression 	*/
codes_termfact[]=	{ ident, leftpar, lbracket, notsy, intc, 
	charc, stringc, nilsy, eolint };
		/*Starting symbols for the term and multiplier 		*/


/* The sets of character codes expected after processing of various constructions:								*/

unsigned

acodes_block[]=			{point,endoffile,eolint},	/* Symbols following the block construction in the main program */
acodes_simpletype[]=		{comma,rbracket,eolint}, 	/* \F1\E8\EC\E2\EE\EB\FB, \EE\E6\E8\E4\E0\E5\EC\FB\E5 \F1\F0\E0\E7\F3 \EF\EE\F1\EB\E5 \E2\FB\E7\EE\E2\E0 simpletype() \E2\EE \E2\F0\E5\EC\FF \E0\ED\E0\EB\E8\E7\E0 \F2\E8\EF\E0 "\EC\E0\F1\F1\E8\E2" 				*/
acodes_typ[]=		{endsy,rightpar,semicolon,eolint},	/* Characters expected immediately after analysis of the typ construct when calling the typ () function from fixpart ()		*/
acodes_3const[]=	{twopoints,comma,rbracket,eolint},	/* Character codes expected immediately after the analysis of the constant 	*/
acodes_listparam[]=     {colon,semicolon,forwardsy,constsy,varsy,eolint},
		/* Characters expected immediately after the parameter list     */
		/* ( functionsy,proceduresy,beginsy \F3\E6\E5 \E5\F1\F2\FC \E2  */
		/*   followers)                                         */
acodes_factparam[]=      {comma,rightpar,eolint},
		/* Characters expected immediately after parsing the actual parameters of procedures and functions                       */
acodes_assign[]=         {assign,eolint},
		/* The character expected immediately after the variable in the assignment statement and in the for statement                       */
acodes_compcase[]=      {semicolon,endsy,eolint},
		/* Characters expected immediately after the statement in the compound statement     */
acodes_iftrue[]=        { thensy,eolint},
		/* The character expected immediately after the conditional expression in the if statement						*/
acodes_iffalse[]=       { elsesy,eolint},
		/*Character expected immediately after the operator of the branch "true" in the if statement					*/
acodes_wiwifor[]=       {comma,dosy,eolint},
		/*   { dosy,eolint} - The character expected immediately after the conditional expression in the while statement and immediately after the expression-the second boundary of the loop parameter change in the for statement				*/
acodes_repeat[]=         { untilsy, semicolon, eolint },
		/* Characters expected immediately after the operator in the body of the repeat statement					*/
acodes_for1[]=           { tosy,downtosy,eolint},
		/* Characters expected immediately after the expression-the first boundary of the loop perimeter change in the for statement       */
acodes_ident[]=		{ lbracket, arrow, point, eolint };
		/* ... After the identifier in the variable 		*/

/* Sets of operation codes in the section for compiling expressions: 		*/

unsigned

codes_rel[]=		{ later, laterequal, greater, greaterequal,
	equal, latergreater, insy, eolint },
		/* Relational operations					*/
codes_add[]=		{ plus, minus, orsy, eolint },
		/* Additive operations				*/
codes_mult[]=		{ star, slash, divsy, modsy, andsy, eolint };
		/* \EC\F3\EB\FC\F2\E8\EF\EB\E8\EA\E0\F2\E8\E2\ED\FB\E5 \EE\EF\E5\F0\E0\F6\E8\E8				*/

/* Sets of codes for the acceptable use of identifiers:	*/

unsigned

codes_VARCONFUNCS[]=	{VARS,FUNCS,CONSTS,eolint},
codes_VARS[]=		{VARS,eolint},
codes_CONSTS[]=		{CONSTS,eolint},
codes_TYPES[]=		{TYPES,eolint},
codes_TYCON[]=		{TYPES,CONSTS,eolint},
codes_FUNCS[]=          {FUNCS,eolint},
codes_VARFUNPR[]=       {VARS,FUNCS,PROCS,eolint};



/* Sets of type codes that are not valid for use in a particular context*/

unsigned codes_illegal[]={eolint};

int f=fileswork( argc, argv );

illegalcodes=convert_to_bits(codes_illegal);
idstarters=convert_to_bits(codes_idstart);
begpart=convert_to_bits(codes_block);
st_typepart=convert_to_bits(codes_block+2);
st_varpart=convert_to_bits(codes_block+3);
st_procfuncpart=convert_to_bits(codes_block+4);
st_statpart=convert_to_bits(codes_block+6);
st_constant=convert_to_bits(codes_constant);
st_conaftersign=convert_to_bits(codes_constant+4);
st_typ=convert_to_bits(codes_typ);
st_simpletype=convert_to_bits(codes_typ+6);
st_startstatement=convert_to_bits(codes_statement+5);
st_statement=convert_to_bits(codes_statement);

st_express=convert_to_bits(codes_express);
st_termfact=convert_to_bits(codes_termfact);

blockfol=convert_to_bits(acodes_block);
rpar=convert_to_bits(codes_rightpar);
af_1constant=convert_to_bits(acodes_typ+2);
af_3const1=convert_to_bits(acodes_3const);
af_4const2=convert_to_bits(acodes_3const+1);
af_1typ=convert_to_bits(acodes_typ+1);
af_2typ=convert_to_bits(acodes_typ);
af_funclistparam=convert_to_bits(acodes_listparam);
af_proclistparam=convert_to_bits(acodes_listparam+1);
af_blockprocfunc=convert_to_bits(acodes_typ+2);
af_sameparam=convert_to_bits(acodes_typ+1);
af_factparam=convert_to_bits(acodes_factparam);
af_oneparam=convert_to_bits(acodes_factparam+1);

af_assignment=convert_to_bits(acodes_assign);
af_compstatement=convert_to_bits(acodes_compcase);
af_iftrue=convert_to_bits(acodes_iftrue);
af_iffalse=convert_to_bits(acodes_iffalse);
af_whilefor=convert_to_bits(acodes_wiwifor+1);
af_repeat=convert_to_bits(acodes_repeat);
af_for1=convert_to_bits(acodes_for1);
af_forassign=convert_to_bits(acodes_assign);

af_ident=convert_to_bits(acodes_ident);

op_rel=convert_to_bits(codes_rel);
op_add=convert_to_bits(codes_add);
op_mult=convert_to_bits(codes_mult);

set_VARCONFUNCS=convert_to_bits(codes_VARCONFUNCS);
set_VARS=convert_to_bits(codes_VARS);
set_TYPES=convert_to_bits(codes_TYPES);
set_CONSTS=convert_to_bits(codes_CONSTS);
set_TYCON=convert_to_bits(codes_TYCON);
set_FUNCS=convert_to_bits(codes_FUNCS);
set_FUNPR=convert_to_bits(codes_VARFUNPR+1);
set_PROCS=convert_to_bits(codes_VARFUNPR+2);
set_VARFUNPR=convert_to_bits(codes_VARFUNPR);

   nextsymbol=0;
   next.linenumber=0;
   next.charnumber=0;
   positionnow.linenumber = 0;       /* Position setting */
   positionnow.charnumber = 1;       /* The first character of the text */
   lineindex = 0;                    /* Index letters */
   ErrorCount = ErrInx = 0;
   ErrorOverflow = FALSE;
   ReadNextLine();
   cnt=0;
   char ch=nextch( );
   nextsym();

   programme();
   //dstfile=fopen(dfile,"a+");
	
   if ( ErrorCount == 0 )
      fprintf( dstfile, "\nThe compilation is over: no errors\n");
      else fprintf( dstfile, "\nThe compilation is over:  %3d errors!\n",ErrorCount );
#ifdef TOUT
   fclose(t);
#endif
   fclose( srcfile );
   fclose( dstfile );
return 0;
}


/*****				FILESWORK				*****/
int fileswork( argc, argv )	/*Performs all initial manipulation of files */
int argc;
char *argv[];
{  int index=0;			/* Number of current error */
   char hotline[80]; 		/* current string */

      /* ----- load error messages ----- */
   if((mesfile=fopen("Err.msg", "r"))==NULL)
      { puts("Error opening ERR.MSG\n"); exit(3); }
   rewind( mesfile );
   if( fgets(hotline,MAXLEN,mesfile)==NULL || fgets(hotline,MAXLEN,mesfile)==NULL)
      puts("Error reading header of message file");
   while( !feof(mesfile) ) {
      fgets( hotline, MAXLEN, mesfile );	/* Reading a string in a message format */
      index=atoi(strncpy(hotline,hotline,MAXLEN));	/* Getting an index        */
      messages[index]=(char*)malloc( sizeof(char) * MAXMES );
messages[index] = strcpy(messages[index],hotline);//strdup( strchr(hotline,':')+2 );
	//printf("%d %s\n",index,messages[index]);
 //     if( strlen(strchr( hotline, *(strchr(hotline,':')+2))) > 0) 
//	  {	 if((messages[index] = strdup( strchr(hotline,':')+2 ))==NULL) 
//		{printf("Index =%d, not enough memory\n",index);
//	     exit(0);	 
//		}
  ///    }
    //  else messages[index] = NULL;
   }
   fclose( mesfile );
      /* ----- Error messages loaded or error is detected----- */
      /* ----- File opening ----- */
   if( argc==3 ) {
      srcfile=fopen( argv[1], "r" );
      dstfile=fopen( argv[2], "w" );
   }
   else {
   printf( "\n\n Enter file name of source program  : " );
   scanf( "%s", sfile );
   printf( " Enter file name of listing : " );
   scanf( "%s", dfile );
   if(( srcfile = fopen( strcat(fulls,sfile), "r")) == NULL ) {
      printf( "Unable to open TESTS file \\%s with source text !\n", sfile );
	  exit(1);
   }
   if(( dstfile = fopen( strcat(fulld,dfile), "w")) == NULL ) {
      printf( "Can not open TESTS file \\%s with program listing !\n", dfile );

      exit(2);
   }
   }
#ifdef TOUT
   t=fopen( "TESTS\\t.out", "w" );
   rewind( t );
   fprintf( t, "                      File of debug T.OUT\n\n");
#endif
   rewind( srcfile );
   rewind( dstfile );
   fprintf( dstfile, "               Pascal - compiler \n" );
   fprintf( dstfile, "                   Listing of the program\n" );
return 0;
}

/*-------------------- C O N V E R T _ T O _ B I T S -------------------*/
unsigned 	*convert_to_bits	(unsigned *intstr)
				/* Translates a one-dimensional array with base type 0..127 
				into a 128-bit string, the state of each bit in which reflects 
				the absence or presence in the source array of an element numerically 
				equal to the number of this bit used to represent in the form of bit 
				strings of start symbols of various Pascal language constructs, symbols ,
				Expected immediately after the processing of certain constructions, as 
				well as the set of acceptable ways of using the identifier and other 
				unseen things, the argument - the address of the array to be converted 
				returns The address of an array of integers with a total length of 
				elements of 128 bits, which we call a 128-bit string*/
{
unsigned     *set,
	*str,		/* Auxiliary pointer to the original array */
	wordnum,	/* Word number - of the constituent bit string	*/
	bitnum;		/* Bit number in the word contained in the string	*/
str=intstr;
#ifdef DEBPRINT
fprintf(t," In convert_to_bits\n");
#endif
set=(unsigned*)calloc(8,sizeof(unsigned));
while(*str!=eolint)	
	{wordnum= *str / WORDLENGTH;	/* Calculating the word number in which the bit with the number is located *str			*/
	bitnum= *str % WORDLENGTH;	/* Calculating the bit number in the word in which it is located			*/
	set[wordnum] |= (1<<(WORDLENGTH-1-bitnum));
	str++;
	}
#ifdef DEBPRINT
fprintf(t,"Out of convert_to_bits\n");
#endif
return((unsigned*)set);
}
