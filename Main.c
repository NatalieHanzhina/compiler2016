
/*
			  ������� ���������.
*/
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <locale.h>
//#include <conio.h>
#include "DECW.h"        /* �����������  ������������             */
#include "DECKEY.h"      /* ����������� ����� �������� ����       */
#include "TREE.h"        /* c������ ������������� ��������������� */
#include "TYPES.h"       /* ���������� ����� �����		  */
#include "TYPET.h"       /* ���� ������� ����������               */
#include "FUNCS.h"       /* �������� ������� �������              */
#include "IODEFS.h"      /* o������� ��� ������ �����/������      */
#include "RUMERCOD.h"    /* ���� ������                           */
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
extern int lineindex;	/* �� ���y�� NEXTCH			*/

FILE *mesfile,      /* ������ ��������� �� ������� 		 */
     *srcfile,      /* ���� � �������� ������� 			 */
     *dstfile;      /* ���� � ��������� � � ����������� �� ������� */

  FILE *t;            /* ��������� ���� � ����������� �����������    */

char  line[ MAXLEN ],           /* ������� ������ 			 */
      *messages[ MAXMES ],  /* ��������� �� �������   */
      sfile[ FNAME ],      /* ����� ������ */
      dfile[ FNAME ],      /* ��� ���� 	*/
      fulls[ MAXLEN ] = "",	/* ������� */
      fulld[ MAXLEN ] = "";	/* - ����  */

short LastInLine,              /*   �������� ������� � ������ 		 */
      ErrInx,                  /*   ����� ��������� � ������ ������      */
      ErrorOverflow,           /*   1-������� ����� ������, 0-���������  */
      ErrorCount;              /*   ����� ��������� � ��������� ������   */

struct textposition positionnow,
       token,
       next;
unsigned symbol,         /* ��� �������,���������� ����.����.	 */
	 nextsymbol,     /* ��� ������shche� �������(������ [3..5]	 */
	 errorcode;

	/*    �������, ���������� ��  ������������ ����������� */

char               name[79];     /* �������������                  */
int		   nmbi;         /* �����  �����                   */
double	      	   nmbf;         /* �����  �  ���������  ������    */
int   	           onesymbol;    /* ������                         */
unsigned  short    cnt;          /* ���  �������                   */
unsigned  short    lname;
unsigned           sw;
int	     	   ch;           /* ������� ������                 */
char strings[MAXLEN];

	       /* ���������� ��� ��������� ���� */

unsigned long dstr[SLENGTH];


/* �������� ����������� � ���� ������� ����� �������� ��������, 	*/
/* ��������� ��� ��������� �������������� �����������:			*/

unsigned

*idstarters,		/* ��������� �� ������ ���������� ������� ident */
*begpart,		/* ��������� ������� ������� block()		*/
*rpar,			/* ������ ������ 				*/
*st_typepart,		/* ��������� ������� ������� typepart()		*/
*st_varpart,		/* ��������� ������� ������� varpart()		*/
*st_procfuncpart,	/* ��������� ������� ������� procfuncpart()	*/
*st_statpart,		/* ��������� ������� ������� statpart()		*/
*st_constant,	 	/* ��������� ������� ������� constant()		*/
*st_conaftersign,	/* ��������� ������� ����������� ���������, 	*/
	/* ������ ����� ����� + ��� -					*/
*st_typ,		/* ��������� ������� ������� typ()		*/
*st_simpletype,	/* ��������� ������� ������� simpletype()	*/
*st_statement,          /* ��������� ������� ����������� <��������>     */
*st_startstatement,     /* ��������� ������� ��������� ��� �������������*/
*st_express,		/* ... ��������� 				*/
*st_termfact;		/* ... ���������� � ��������� 			*/


/* �������� ����������� � ���� ������� ����� �������� ��������, 	*/
/* ��������� ����� ����� ��������� ��������� �����������:		*/

unsigned

*blockfol,		/* ...����� ��������� ����� �������� ���������  */
*af_1constant,		/* ...����� ������� ��������� ��� ������ ����-	*/
	/* ��� constant() �� ������� constdeclaration(), � ����� �����  */
	/* ������� ����������� ���������� ���������� ��� ������ ������� */
	/* vardeclaration() �� ������� varpart()			*/
*af_3const1,		/* ...����� ������� ������ ��������� �������	*/
	/* ��� ��������� ����� � ������� simpletype()			*/
*af_4const2,		/* ...����� ������� ������ ��������� �������	*/
	/* ��� ��������� ����� � ������� simpletype()			*/
*af_1typ,		/* ...����� ������� ����������� �������� ����	*/
	/* ��� ������ ������� typ() �� typedeclaration()		*/
*af_2typ,		/* ...����� ������� ����������� �������� ����	*/
	/* ��� ������ ������� typ() �� fixpart()			*/
*af_proclistparam,      /* ...����� ������� ������ ���������� ��������� */
*af_funclistparam,      /* ...����� ������� ������ ���������� �������   */
*af_blockprocfunc,      /* ...����� ������� ����� ��������� ��� ������� */
*af_sameparam,          /* ...����� ������� ���������� ����������       */
*af_factparam,          /* ...����� ������� ����������� ����������      */
	/* �������� � �������                                           */
*af_oneparam,           /* ...����� ������� ��������� �����������       */
	/* �������� � �������, ������� ���� ��������                    */
*af_writeparam,         /* ...����� ������� ��������� �����������       */
	/* �������� write � writeln                                     */

*af_assignment,         /* ...����� ������� ���������� � ���������      */
	/* ������������							*/
*af_compstatement,      /* ...����� ������� ��������� � ��������� ��-�� */
*af_iftrue,             /* ...����� ������� ��������� ��������� � �����-*/
	/*���� if							*/
*af_iffalse,            /* ...����� ������� ��������� ����� "������" �  */
	/* ��������� if							*/
*af_whilefor,		/* ...����� ������� ��������� ��������� � �����-*/
	/* ���� while � ���������-������ ������� ��������� ���������    */
	/* ����� � ��������� for					*/
*af_repeat,             /* ...����� ������� ��������� � ���� ����� repeat*/
*af_with,               /* ...����� �������  ����������  �   ���������  */
	/* ��������� with						*/
*af_case1,              /* ...����� ������� ����������� ��������� � case*/
*af_case2,              /* ...����� ������� �������� � ��������� case   */
*af_forassign,	        /* ...����� ������� ���������� � ��������� for	*/
*af_for1, 		/* ...����� ������� ���������-������ ������� ��-*/
	/* ������� ��������� ����� � ��������� for			*/
*af_ident;		/* ...����� �������������� � "����������"	*/

/* �������� ����������� � ���� ������� ����� �������� ���������� �����- */
/* ��� �������� � ������� ���������� ��������� 				*/

unsigned

*op_rel,		/* �������� ��������� ��� �������� �����������	*/
*op_add,		/* ���������� �������� ��� ����������		*/
*op_mult;		/* ����������������� �������� ��� �����������	*/

/* �������� ����������� � ���� ������� ����� �������� �������� ������-	*/
/* ������� ���������������:						*/

unsigned

*set_VARCONFUNCS,	/* ���. ������� ������������� - VARS, CONSTS, FUNCS */
*set_VARS,	/* ���������� ������ ������������� - VARS		*/
*set_TYPES, 	/* ���������� ������ ������������� - TYPES 		*/
*set_CONSTS,	/* ���������� ������ ������������� - CONSTS 		*/
*set_TYCON,     /* ���������� ������� ������������� - TYPES,CONSTS	*/
*set_FUNCS,     /* ���������� ������ ������������� - FUNCS              */
*set_PROCS,     /* ���������� ������ ������������� - PROCS              */
*set_FUNPR,     /* ���������� ������� ������������� - FUNCS,PROCS       */
*set_VARFUNPR;  /* ���������� ������� ������������� - VARS,FUNCS,PROCS  */

/* �������� ������������ � ���� ������� ������ ��������� ����� �����,	*/
/* ������������ ��� ������������� � ��� ��� ���� ���������:		*/

unsigned *illegalcodes;

/*----------------------------- M A I N --------------------------------*/
void main( argc, argv )
int argc;
char *argv[];
{
setlocale(LC_ALL,"rus");
/* ��������� ����� ��������, ��������� ��� ��������� �������������� 	*/
/* �����������:								*/

unsigned

codes_idstart[]=	{ident,eolint},	/* ��������� ������� ��������� 	*/
		/* �������������� ����������� 				*/
codes_block[]=		{labelsy,constsy,typesy,varsy,functionsy,
	proceduresy,beginsy,eolint},	/* ��������� ������� ��������	*/
		/* �������� � ������� ����������			*/
codes_rightpar[]=	{rightpar,eolint},
		/* ������ ������ 					*/
codes_constant[]=	{plus,minus,charc,stringc,ident,intc,floatc,
	eolint},	/* ��������� ������� ����������� constant       */
codes_typ[]=		{packedsy,arrow,arraysy,filesy,setsy,recordsy,
	plus,minus,ident,leftpar,intc,charc,stringc,eolint},
			/* ��������� ������� ����������� �������� ����;	*/
		/* {plus,minus,ident,leftpar,intc,charc,stringc,eolint}-*/
		/* ��������� ������� ����������� �������� �������� ���� */

codes_statement[]=      {intc,endsy,elsesy,untilsy,ident,beginsy,ifsy,
	whilesy,repeatsy,forsy,casesy,withsy,semicolon,gotosy,eolint},
		/* ��������� ������� ����������� <��������>             */
		/* (��� ������������� ������ ������������ �������:      */
		/* {beginsy,ifsy,whilesy,repeatsy,forsy,casesy,withsy,  */
		/* semicolon,gotosy,eolint} )                           */
codes_express[]=	{ plus, minus, leftpar, lbracket, notsy, ident,
	intc, floatc, charc, stringc, nilsy, eolint },
		/* ��������� ������� ��������� � �������� ��������� 	*/
codes_termfact[]=	{ ident, leftpar, lbracket, notsy, intc, floatc,
	charc, stringc, nilsy, eolint };
		/* ��������� ������� ���������� � ��������� 		*/


/* ��������� ����� ��������, ��������� ����� ��������� ��������� ���-	*/
/* ��������:								*/

unsigned

acodes_block[]=			{point,endoffile,eolint},	/* ���-	*/
		/* ����, ��������� �� ������������ ����� � �������� 	*/
		/* ���������			 			*/
acodes_simpletype[]=		{comma,rbracket,eolint}, 	/* ���- */
		/* ����, ��������� ����� ����� ������ simpletype() �� 	*/
		/* ����� ������� ���� "������" 				*/
acodes_typ[]=		{endsy,rightpar,semicolon,eolint},	/* ���-	*/
		/* ����, ��������� ����� ����� ������� ����������� typ 	*/
		/* ��� ������ ������� typ() �� fixpart()		*/
acodes_3const[]=	{twopoints,comma,rbracket,eolint},	/* ���� */
		/* ��������, ��������� ����� ����� ������� ��������� 	*/
acodes_listparam[]=     {colon,semicolon,forwardsy,constsy,varsy,eolint},
		/* �������, ��������� ����� ����� ������ ����������     */
		/* ( ������� functionsy,proceduresy,beginsy ��� ���� �  */
		/*   followers)                                         */
acodes_factparam[]=      {comma,rightpar,eolint},
		/* �������, ��������� ����� ����� ������� �����������   */
		/* ���������� �������� � �������                        */
acodes_assign[]=         {assign,eolint},
		/* ������, ��������� ����� ����� ���������� � ��������� */
		/* ������������ � � ��������� for                       */
acodes_compcase[]=      {semicolon,endsy,eolint},
		/* �������, ��������� ����� ����� ��������� � ��������� */
		/* ���������     */
acodes_iftrue[]=        { thensy,eolint},
		/* ������, ��������� ����� ����� ��������� ��������� �  */
		/* ��������� if						*/
acodes_iffalse[]=       { elsesy,eolint},
		/*������, ��������� ����� ����� ��������� ����� "������"*/
		/* � ��������� if					*/
acodes_wiwifor[]=       {comma,dosy,eolint},
		/* �������, ��������� ����� ����� ���������� � ���������*/
		/* ��������� with;  { dosy,eolint} - ������, ���������  */
		/* ����� ����� ��������� ��������� � ��������� while �  */
		/* ����� ����� ���������-������ ������� ��������� ����- */
		/* ����� ����� � ��������� for				*/
acodes_repeat[]=         { untilsy, semicolon, eolint },
		/* c�����s, ��������� ����� ����� ��������� � ����      */
		/* ��������� repeat					*/
acodes_case1[]=          { ofsy,eolint },
		/* ������, ��������� ����� ����� ����������� ���������  */
		/* � ��������� case					*/
acodes_for1[]=           { tosy,downtosy,eolint},
		/* �������, ��������� ����� ����� ���������-������ ���- */
		/* ���� ��������� ��������� ����� � ��������� for       */
acodes_ident[]=		{ lbracket, arrow, point, eolint };
		/* ... ����� �������������� � ���������� 		*/

/* ��������� ����� �������� � ������� ���������� ���������: 		*/

unsigned

codes_rel[]=		{ later, laterequal, greater, greaterequal,
	equal, latergreater, insy, eolint },
		/* �������� ��������� 					*/
codes_add[]=		{ plus, minus, orsy, eolint },
		/* ���������� �������� 					*/
codes_mult[]=		{ star, slash, divsy, modsy, andsy, eolint };
		/* ����������������� ��������				*/

/* ��������� ����� ���������� �������� ������������� ���������������:	*/

unsigned

codes_VARCONFUNCS[]=	{VARS,FUNCS,CONSTS,eolint},
codes_VARS[]=		{VARS,eolint},
codes_CONSTS[]=		{CONSTS,eolint},
codes_TYPES[]=		{TYPES,eolint},
codes_TYCON[]=		{TYPES,CONSTS,eolint},
codes_FUNCS[]=          {FUNCS,eolint},
codes_VARFUNPR[]=       {VARS,FUNCS,PROCS,eolint};



/* ��������� ����� �����, ������������ ��� ������������� � ��� ��� ���� */
/* ���������:								*/

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
   positionnow.linenumber = 0;       /* ��������� ������� */
   positionnow.charnumber = 1;       /* ������� ������� ������ */
   lineindex = 0;                    /* ������ ������ */
   ErrorCount = ErrInx = 0;
   ErrorOverflow = FALSE;
   ReadNextLine();
   cnt=0;
   char ch=nextch( );
   nextsym();

   programme();
   dstfile=fopen(dfile,"a+");
	
   fprintf( dstfile, "\n�o�������� ��������: ������");
   if ( ErrorCount == 0 )
      fprintf( dstfile, " ��� !\n");
      else fprintf( dstfile, " - %3d !\n",ErrorCount );
#ifdef TOUT
   fclose(t);
#endif
   fclose( srcfile );
   fclose( dstfile );
}


/*****				FILESWORK				*****/
fileswork( argc, argv )	/* ������������ ��� ��������� ����������� � ������� */
int argc;
char *argv[];
{  int index=0;			/* ����� ���������������� ������ */
   char hotline[80]; 		/* ���������������� ������ */

      /* ----- �������� ��������� �� ������� ----- */
   if((mesfile=fopen("Err.msg", "r"))==NULL)
      { puts("������ �������� ERR.MSG\n"); exit(3); }
   rewind( mesfile );
   if( fgets(hotline,MAXLEN,mesfile)==NULL || fgets(hotline,MAXLEN,mesfile)==NULL)
      puts("������ ������ ��������� ����� ���������");
   while( !feof(mesfile) ) {
      fgets( hotline, MAXLEN, mesfile );	/* ������ ������ � ������� ��������� */
      index=atoi(strncpy(hotline,hotline,MAXLEN));	/* ��������� �������         */
      messages[index]=(char*)malloc( sizeof(char) * MAXMES );
messages[index] = strcpy(messages[index],hotline);//strdup( strchr(hotline,':')+2 );
	//printf("%d %s\n",index,messages[index]);
 //     if( strlen(strchr( hotline, *(strchr(hotline,':')+2))) > 0) 
//	  {	 if((messages[index] = strdup( strchr(hotline,':')+2 ))==NULL) 
//		{printf("������=%d, ������������ ������\n",index);
//	     exit(0);	 
//		}
  ///    }
    //  else messages[index] = NULL;
   }
   fclose( mesfile );
      /* ----- ��������� �� ������� ��������� ��� ������������� ������ ----- */
      /* ----- �������� ������ ----- */
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
      printf( "���������� ������� ���� TESTS\\%s � �������� ������� !\n", sfile );
	  exit(1);
   }
   if(( dstfile = fopen( strcat(fulld,dfile), "w")) == NULL ) {
      printf( "���������� ������� ���� TESTS\\%s � ��������� ��������� !\n", dfile );

      exit(2);
   }
   }
#ifdef TOUT
   t=fopen( "TESTS\\t.out", "w" );
   rewind( t );
   fprintf( t, "                      ���� ���������� ������� T.OUT\n\n");
#endif
   rewind( srcfile );
   rewind( dstfile );
   fprintf( dstfile, "               �������� Pascal - ���������� \n" );
   fprintf( dstfile, "                   ������� ���������\n" );
}

/*-------------------- C O N V E R T _ T O _ B I T S -------------------*/
unsigned 	*convert_to_bits	(unsigned *intstr)
				/* ��������� ���������� ������ � ����-	*/
			/* ��� ����� 0..127 � 128-������� ������, ��-	*/
			/* ������� ������� ���� � ������� �������� ��-	*/
			/* �������� ��� ������� � �������� ������� ���-	*/
			/* �����, �������� ������� ������ ����� ����	*/
				/* ������������ ��� ������������� �	*/
			/* ���� ������� ����� ��������� �������� ���-	*/
			/* ������ �������� ����������� �������, �����-	*/
			/* ���, ��������� ����� ����� ��������� ��� ��� */
			/* ���� �����������, � ����� �������� �������-	*/
			/* ��� �������� ������������� �������������� � 	*/
			/* ������ ������������� �����			*/
				/* �������� - ����� �������, ���-	*/
			/* �������� ���������������			*/
				/* ���������� ����� ������� �������, -	*/
			/* ������� ����� ����� � ��������� ������ ���-	*/
			/* ������ � 128 ���, ������� � ������� 128-��-	*/
			/* ����� �������				*/
{
unsigned     *set,
	*str,		/* ��������������� ��������� �� �������� ������ */
	wordnum,	/* ����� ����� - ������������� ������� ������	*/
	bitnum;		/* ����� ���� � �����, ������������ � ������	*/
str=intstr;
#ifdef DEBPRINT
fprintf(t," ����� � convert_to_bits\n");
#endif
set=(unsigned*)calloc(8,sizeof(unsigned));
while(*str!=eolint)	
	{wordnum= *str / WORDLENGTH;	/* ���������� ������ �����, � 	*/
		/* ������� ��������� ��� � ������� *str			*/
	bitnum= *str % WORDLENGTH;	/* ���������� ������ ���� �	*/
		/* �����, � ������� �� ���������			*/
	set[wordnum] |= (1<<(WORDLENGTH-1-bitnum));
	str++;
	}
#ifdef DEBPRINT
fprintf(t,"����� �� convert_to_bits\n");
#endif
return((unsigned*)set);
}
