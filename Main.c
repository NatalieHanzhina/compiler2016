
/*
			  Главная программа.
*/
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <locale.h>
//#include <conio.h>
#include "DECW.h"        /* определение  разделителей             */
#include "DECKEY.h"      /* определение кодов ключевых слов       */
#include "TREE.h"        /* cпособы использования идентификаторов */
#include "TYPES.h"       /* объявление кодов типов		  */
#include "TYPET.h"       /* файл внешних переменных               */
#include "FUNCS.h"       /* описание нецелых функций              */
#include "IODEFS.h"      /* oписания для модуля ввода/вывода      */
#include "RUMERCOD.h"    /* коды ошибок                           */
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
extern int lineindex;	/* из модyля NEXTCH			*/

FILE *mesfile,      /* тексты сообщений об ошибках 		 */
     *srcfile,      /* файл с исходным текстом 			 */
     *dstfile;      /* файл с листингом и с сообщениями об ошибках */

  FILE *t;            /* временный файл с отладочными сообщениями    */

char  line[ MAXLEN ],           /* текущая строка 			 */
      *messages[ MAXMES ],  /* сообщения об ошибках   */
      sfile[ FNAME ],      /* имена файлов */
      dfile[ FNAME ],      /* без пути 	*/
      fulls[ MAXLEN ] = "",	/* добавка */
      fulld[ MAXLEN ] = "";	/* - путь  */

short LastInLine,              /*   значащие символы в строке 		 */
      ErrInx,                  /*   число сделанных в строке ошибок      */
      ErrorOverflow,           /*   1-слишком много ошибок, 0-нормально  */
      ErrorCount;              /*   число сделанных в программе ошибок   */

struct textposition positionnow,
       token,
       next;
unsigned symbol,         /* код лексемы,выдаваемый синт.анал.	 */
	 nextsymbol,     /* код следуюshcheй лексемы(случай [3..5]	 */
	 errorcode;

	/*    лексемы, полученные из  лексического анализатора */

char               name[79];     /* идентификатор                  */
int		   nmbi;         /* целое  число                   */
double	      	   nmbf;         /* число  с  плавающей  точкой    */
int   	           onesymbol;    /* символ                         */
unsigned  short    cnt;          /* для  отладки                   */
unsigned  short    lname;
unsigned           sw;
int	     	   ch;           /* текущий символ                 */
char strings[MAXLEN];

	       /* Переменные для генерации кода */

unsigned long dstr[SLENGTH];


/* Описание реализуемых в виде битовых строк множеств символов, 	*/
/* стартовых для различных обрабатываемых конструкций:			*/

unsigned

*idstarters,		/* множество из одного стартового символа ident */
*begpart,		/* стартовые символы функции block()		*/
*rpar,			/* правая скобка 				*/
*st_typepart,		/* стартовые символы функции typepart()		*/
*st_varpart,		/* стартовые символы функции varpart()		*/
*st_procfuncpart,	/* стартовые символы функции procfuncpart()	*/
*st_statpart,		/* стартовые символы функции statpart()		*/
*st_constant,	 	/* стартовые символы функции constant()		*/
*st_conaftersign,	/* стартовые символы конструкции константы, 	*/
	/* идущей после знака + или -					*/
*st_typ,		/* стартовые символы функции typ()		*/
*st_simpletype,	/* стартовые символы функции simpletype()	*/
*st_statement,          /* стартовые символы конструкции <оператор>     */
*st_startstatement,     /* стартовые символы оператора при нейтрализации*/
*st_express,		/* ... выражения 				*/
*st_termfact;		/* ... слагаемого и множителя 			*/


/* Описание реализуемых в виде битовых строк множеств символов, 	*/
/* ожидаемых сразу после обработки различных конструкций:		*/

unsigned

*blockfol,		/* ...после обработки блока основной программы  */
*af_1constant,		/* ...после анализа константы при вызове функ-	*/
	/* ции constant() из функции constdeclaration(), а также после  */
	/* анализа конструкции объявления переменных при вызове функции */
	/* vardeclaration() из функции varpart()			*/
*af_3const1,		/* ...после анализа первой константы отрезка	*/
	/* при обработке оного в функции simpletype()			*/
*af_4const2,		/* ...после анализа второй константы отрезка	*/
	/* при обработке оного в функции simpletype()			*/
*af_1typ,		/* ...после анализа конструкции описания типа	*/
	/* при вызове функции typ() из typedeclaration()		*/
*af_2typ,		/* ...после анализа конструкции описания типа	*/
	/* при вызове функции typ() из fixpart()			*/
*af_proclistparam,      /* ...после анализа списка параметров процедуры */
*af_funclistparam,      /* ...после анализа списка параметров функции   */
*af_blockprocfunc,      /* ...после анализа блока процедуры или функции */
*af_sameparam,          /* ...после анализа однотипных параметров       */
*af_factparam,          /* ...после анализа фактических параметров      */
	/* процедур и функций                                           */
*af_oneparam,           /* ...после анализа параметра стандартных       */
	/* процедур и функций, имеющих один параметр                    */
*af_writeparam,         /* ...после анализа параметра стандартных       */
	/* процедур write и writeln                                     */

*af_assignment,         /* ...после анализа переменной в операторе      */
	/* присваивания							*/
*af_compstatement,      /* ...после анализа оператора в составном оп-ре */
*af_iftrue,             /* ...после анализа условного выражения в опера-*/
	/*торе if							*/
*af_iffalse,            /* ...после анализа оператора ветви "истина" в  */
	/* операторе if							*/
*af_whilefor,		/* ...после анализа условного выражения в опера-*/
	/* торе while и выражения-второй границы изменения параметра    */
	/* цикла в операторе for					*/
*af_repeat,             /* ...после анализа оператора в теле цикла repeat*/
*af_with,               /* ...после анализа  переменной  в   заголовке  */
	/* оператора with						*/
*af_case1,              /* ...после анализа выбирающего выражения в case*/
*af_case2,              /* ...после анализа варианта в операторе case   */
*af_forassign,	        /* ...после анализа переменной в операторе for	*/
*af_for1, 		/* ...после анализа выражения-первой границы из-*/
	/* менения параметра цикла в операторе for			*/
*af_ident;		/* ...после идентификатора в "переменной"	*/

/* Описание реализуемых в виде битовых строк множеств допустимых симво- */
/* лов операций в разделе компиляции выражений 				*/

unsigned

*op_rel,		/* операции отношения над простыми выражениями	*/
*op_add,		/* аддитивные операции над слагаемыми		*/
*op_mult;		/* мультипликативные операции над множителями	*/

/* Описание реализуемых в виде битовых строк множеств способов исполь-	*/
/* зования идентификаторов:						*/

unsigned

*set_VARCONFUNCS,	/* доп. способы использования - VARS, CONSTS, FUNCS */
*set_VARS,	/* допустимый способ использования - VARS		*/
*set_TYPES, 	/* допустимый способ использования - TYPES 		*/
*set_CONSTS,	/* допустимый способ использования - CONSTS 		*/
*set_TYCON,     /* допустимые способы использования - TYPES,CONSTS	*/
*set_FUNCS,     /* допустимый способ использования - FUNCS              */
*set_PROCS,     /* допустимый способ использования - PROCS              */
*set_FUNPR,     /* допустимые способы использования - FUNCS,PROCS       */
*set_VARFUNPR;  /* допустимые способы использования - VARS,FUNCS,PROCS  */

/* Описание реализуемого в виде битовой строки множества кодов типов,	*/
/* недопустимых для использования в том или ином контексте:		*/

unsigned *illegalcodes;

/*----------------------------- M A I N --------------------------------*/
void main( argc, argv )
int argc;
char *argv[];
{
setlocale(LC_ALL,"rus");
/* Множества КОДОВ символов, стартовых для различных обрабатываемых 	*/
/* конструкций:								*/

unsigned

codes_idstart[]=	{ident,eolint},	/* стартовые символы некоторых 	*/
		/* обрабатываемых конструкций 				*/
codes_block[]=		{labelsy,constsy,typesy,varsy,functionsy,
	proceduresy,beginsy,eolint},	/* стартовые символы разделов	*/
		/* описаний и раздела операторов			*/
codes_rightpar[]=	{rightpar,eolint},
		/* правая скобка 					*/
codes_constant[]=	{plus,minus,charc,stringc,ident,intc,floatc,
	eolint},	/* стартовые символы конструкции constant       */
codes_typ[]=		{packedsy,arrow,arraysy,filesy,setsy,recordsy,
	plus,minus,ident,leftpar,intc,charc,stringc,eolint},
			/* стартовые символы конструкции описания типа;	*/
		/* {plus,minus,ident,leftpar,intc,charc,stringc,eolint}-*/
		/* стартовые символы конструкции описания простого типа */

codes_statement[]=      {intc,endsy,elsesy,untilsy,ident,beginsy,ifsy,
	whilesy,repeatsy,forsy,casesy,withsy,semicolon,gotosy,eolint},
		/* стартовые символы конструкции <оператор>             */
		/* (при нейтрализации ошибок используются символы:      */
		/* {beginsy,ifsy,whilesy,repeatsy,forsy,casesy,withsy,  */
		/* semicolon,gotosy,eolint} )                           */
codes_express[]=	{ plus, minus, leftpar, lbracket, notsy, ident,
	intc, floatc, charc, stringc, nilsy, eolint },
		/* стартовые символы выражения и простого выражения 	*/
codes_termfact[]=	{ ident, leftpar, lbracket, notsy, intc, floatc,
	charc, stringc, nilsy, eolint };
		/* стартовые символы слагаемого и множителя 		*/


/* Множества кодов символов, ожидаемых после обработки различных кон-	*/
/* струкций:								*/

unsigned

acodes_block[]=			{point,endoffile,eolint},	/* сим-	*/
		/* волы, следующие за конструкцией блока в основной 	*/
		/* программе			 			*/
acodes_simpletype[]=		{comma,rbracket,eolint}, 	/* сим- */
		/* волы, ожидаемые сразу после вызова simpletype() во 	*/
		/* время анализа типа "массив" 				*/
acodes_typ[]=		{endsy,rightpar,semicolon,eolint},	/* сим-	*/
		/* волы, ожидаемые сразу после анализа конструкции typ 	*/
		/* при вызове функции typ() из fixpart()		*/
acodes_3const[]=	{twopoints,comma,rbracket,eolint},	/* коды */
		/* символов, ожидаемых сразу после анализа константы 	*/
acodes_listparam[]=     {colon,semicolon,forwardsy,constsy,varsy,eolint},
		/* символы, ожидаемые сразу после списка параметров     */
		/* ( символы functionsy,proceduresy,beginsy уже есть в  */
		/*   followers)                                         */
acodes_factparam[]=      {comma,rightpar,eolint},
		/* символы, ожидаемые сразу после разбора фактических   */
		/* параметров процедур и функций                        */
acodes_assign[]=         {assign,eolint},
		/* символ, ожидаемый сразу после переменной в операторе */
		/* присваивания и в операторе for                       */
acodes_compcase[]=      {semicolon,endsy,eolint},
		/* символы, ожидаемые сразу после оператора в составном */
		/* операторе     */
acodes_iftrue[]=        { thensy,eolint},
		/* символ, ожидаемый сразу после условного выражения в  */
		/* операторе if						*/
acodes_iffalse[]=       { elsesy,eolint},
		/*символ, ожидаемый сразу после оператора ветви "истина"*/
		/* в операторе if					*/
acodes_wiwifor[]=       {comma,dosy,eolint},
		/* символы, ожидаемые сразу после переменной в заголовке*/
		/* оператора with;  { dosy,eolint} - символ, ожидаемый  */
		/* сразу после условного выражения в операторе while и  */
		/* сразу после выражения-второй границы изменения пара- */
		/* метра цикла в операторе for				*/
acodes_repeat[]=         { untilsy, semicolon, eolint },
		/* cимволs, ожидаемые сразу после оператора в теле      */
		/* оператора repeat					*/
acodes_case1[]=          { ofsy,eolint },
		/* символ, ожидаемый сразу после выбирающего выражения  */
		/* в операторе case					*/
acodes_for1[]=           { tosy,downtosy,eolint},
		/* символы, ожидаемые сразу после выражения-первой гра- */
		/* ницы изменения пераметра цикла в операторе for       */
acodes_ident[]=		{ lbracket, arrow, point, eolint };
		/* ... после идентификатора в переменной 		*/

/* Множества кодов операций в разделе компиляции выражений: 		*/

unsigned

codes_rel[]=		{ later, laterequal, greater, greaterequal,
	equal, latergreater, insy, eolint },
		/* операции отношения 					*/
codes_add[]=		{ plus, minus, orsy, eolint },
		/* аддитивные операции 					*/
codes_mult[]=		{ star, slash, divsy, modsy, andsy, eolint };
		/* мультипликативные операции				*/

/* Множества кодов допустимых способов использования идентификаторов:	*/

unsigned

codes_VARCONFUNCS[]=	{VARS,FUNCS,CONSTS,eolint},
codes_VARS[]=		{VARS,eolint},
codes_CONSTS[]=		{CONSTS,eolint},
codes_TYPES[]=		{TYPES,eolint},
codes_TYCON[]=		{TYPES,CONSTS,eolint},
codes_FUNCS[]=          {FUNCS,eolint},
codes_VARFUNPR[]=       {VARS,FUNCS,PROCS,eolint};



/* Множества кодов типов, недопустимых для использования в том или ином */
/* контексте:								*/

unsigned codes_illegal[]={REFERENCES,RECORDS,SETS,FILES,ARRAYS,eolint};

fileswork( argc, argv );

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
af_case1=convert_to_bits(acodes_case1);
af_case2=convert_to_bits(acodes_compcase);
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
   positionnow.linenumber = 0;       /* установка позиции */
   positionnow.charnumber = 1;       /* первого символа текста */
   lineindex = 0;                    /* индекс литеры */
   ErrorCount = ErrInx = 0;
   ErrorOverflow = FALSE;
   ReadNextLine();
   cnt=0;
   char ch=nextch( );
   nextsym();

   programme();
   dstfile=fopen(dfile,"a+");
	
   fprintf( dstfile, "\nКoмпиляция окончена: ошибок");
   if ( ErrorCount == 0 )
      fprintf( dstfile, " нет !\n");
      else fprintf( dstfile, " - %3d !\n",ErrorCount );
#ifdef TOUT
   fclose(t);
#endif
   fclose( srcfile );
   fclose( dstfile );
}


/*****				FILESWORK				*****/
fileswork( argc, argv )	/* осуществляет все начальные манипуляции с файлами */
int argc;
char *argv[];
{  int index=0;			/* номер свежепрочитанной ошибки */
   char hotline[80]; 		/* свежепрочитанная строка */

      /* ----- загрузка сообщений об ошибках ----- */
   if((mesfile=fopen("Err.msg", "r"))==NULL)
      { puts("Ошибка открытия ERR.MSG\n"); exit(3); }
   rewind( mesfile );
   if( fgets(hotline,MAXLEN,mesfile)==NULL || fgets(hotline,MAXLEN,mesfile)==NULL)
      puts("Ошибка чтения заголовка файла сообщений");
   while( !feof(mesfile) ) {
      fgets( hotline, MAXLEN, mesfile );	/* чтение строки в формате сообщения */
      index=atoi(strncpy(hotline,hotline,MAXLEN));	/* получение индекса         */
      messages[index]=(char*)malloc( sizeof(char) * MAXMES );
messages[index] = strcpy(messages[index],hotline);//strdup( strchr(hotline,':')+2 );
	//printf("%d %s\n",index,messages[index]);
 //     if( strlen(strchr( hotline, *(strchr(hotline,':')+2))) > 0) 
//	  {	 if((messages[index] = strdup( strchr(hotline,':')+2 ))==NULL) 
//		{printf("Индекс=%d, недостаточно памяти\n",index);
//	     exit(0);	 
//		}
  ///    }
    //  else messages[index] = NULL;
   }
   fclose( mesfile );
      /* ----- Сообщения об ошибках загружены или зафиксирована ошибка ----- */
      /* ----- Открытие вайлов ----- */
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
      printf( "Невозможно открыть файл TESTS\\%s с исходным текстом !\n", sfile );
	  exit(1);
   }
   if(( dstfile = fopen( strcat(fulld,dfile), "w")) == NULL ) {
      printf( "Невозможно открыть файл TESTS\\%s с листингом программы !\n", dfile );

      exit(2);
   }
   }
#ifdef TOUT
   t=fopen( "TESTS\\t.out", "w" );
   rewind( t );
   fprintf( t, "                      Файл отладочных печатей T.OUT\n\n");
#endif
   rewind( srcfile );
   rewind( dstfile );
   fprintf( dstfile, "               Работает Pascal - компилятор \n" );
   fprintf( dstfile, "                   Листинг программы\n" );
}

/*-------------------- C O N V E R T _ T O _ B I T S -------------------*/
unsigned 	*convert_to_bits	(unsigned *intstr)
				/* переводит одномерный массив с базо-	*/
			/* вым типом 0..127 в 128-битовую строку, со-	*/
			/* стояние каждого бита в которой отражает от-	*/
			/* сутствие или наличие в исходном массиве эле-	*/
			/* мента, численно равного номеру этого бита	*/
				/* используется для представления в	*/
			/* виде битовых строк стартовых символов раз-	*/
			/* личных языковых конструкций паскаля, симво-	*/
			/* лов, ожидаемых сразу после обработки тех или */
			/* иных конструкций, а также множеств допусти-	*/
			/* мых способов использования идентификатора и 	*/
			/* прочих неблаговидных вещей			*/
				/* аргумент - адрес массива, под-	*/
			/* лежащего конвертированию			*/
				/* возвращает адрес другого массива, -	*/
			/* массива целых чисел с суммарной длиной эле-	*/
			/* ментов в 128 бит, который и именуем 128-би-	*/
			/* товой строкой				*/
{
unsigned     *set,
	*str,		/* вспомогательный указатель на исходный массив */
	wordnum,	/* номер слова - составляющего битовой строки	*/
	bitnum;		/* номер бита в слове, содержащемся в строке	*/
str=intstr;
#ifdef DEBPRINT
fprintf(t," Вошли в convert_to_bits\n");
#endif
set=(unsigned*)calloc(8,sizeof(unsigned));
while(*str!=eolint)	
	{wordnum= *str / WORDLENGTH;	/* вычисление номера слова, в 	*/
		/* котором находится бит с номером *str			*/
	bitnum= *str % WORDLENGTH;	/* вычисление номера бита в	*/
		/* слове, в котором он находится			*/
	set[wordnum] |= (1<<(WORDLENGTH-1-bitnum));
	str++;
	}
#ifdef DEBPRINT
fprintf(t,"вышли из convert_to_bits\n");
#endif
return((unsigned*)set);
}
