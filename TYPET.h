


      /* fILE   TYPET.H               */

      /* FILE OF EXTERNAL VARIABLES */


#define TOUT		/* t.out		*/
#include <stdio.h>

#define maxint 	   32767/* Maximum integer value 				*/
#define SET_SIZE   8	/* Number of words for storing the bit string 	*/

#define LATDIF     32	/* The difference between the codes corresponding to the lowercase and uppercase letters in coding ASCII		*/
#define RUSDIF1    32	/* */
#define RUSDIF2    80   /* */
#define WORDLENGTH 32	/* The length of the minimum addressable information unit in bits		*/
#define STR_LENGTH 4	/* The length of the minimum addressable information unit in bytes	*/
#define INT_LENGTH 4	/* The length of the minimum addressable information unit in bytes		*/
#define BOOL_LENGTH 4	/* The length of the minimum addressable information unit in bytes	*/
#define MAXORDER   308	/* The maximum permissible order of a real constant with a normalized mantissa 					*/
#define labelquality 2
#include "IODEFS.h"
#include "ERR.h"


      extern   struct  textposition   positionnow,/* Current letter position */
				      token,     /* Current token position */
				      next;    /* Next token position */
      extern   char    name[79];                                     /* name */
      extern   int     nmbi;                    /* The value of an integer constant */
      extern   double  nmbf;             /* Value of the real constant */
				
      extern   int     onesymbol;          /* Character constant value */

      extern   char    strings[MAXLEN];     /* String constant value */
					 
      extern   int     ch;                    /* Current scanned letter */
					 
      extern   char   *addrname;     /* Address of the current name in the table of names */
				     
      extern   unsigned          errorcode;                   /* error code */
					 
      extern   unsigned          symbol;             /* Current token code */
					 
      extern   unsigned          hashresult;  /* The result of the hash function for the current name */
				     
      extern   unsigned          nextsymbol;       /* Next token code */
					 
      extern   unsigned short    cnt;                       
					 
      extern   unsigned short    lname;
				/* Length of identifier or keyword */
					 
      extern   FILE 	    	*t,     
				*mesfile,
				*dstfile,
				*srcfile;

      extern   char  		 line[ MAXLEN ],
				*messages[ MAXMES ],
				 sfile[ FNAME ],
				 dfile[ FNAME ];

      extern   short 		 LastInLine,   /* significative characters in a string */
				 ErrInx, /* The number of errors made in the line */
				 ErrorOverflow,/* TRUE-Too many errors,*/
							 /* FALSE- normal */
				 ErrorCount; /* The number of errors made in the program */

      extern   struct E 	 Errlist[ ERRMAX ];        /* errors list */

      extern   struct textposition positionnow;
      extern
	   int labeltable[labelquality],
	       meetlabel[labelquality],
	       meetgoto[labelquality],g_coun;



/* Description of the sets of characters implemented in the form of bit strings, starting symbols for various processed structures:			*/


extern unsigned

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


/* Описание реализуемых в виде битовых строк множеств символов, 	*/
/* ожидаемых сразу после обработки различных конструкций:		*/
/* первое описание - в файле main.c					*/

extern unsigned

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

extern unsigned

*op_rel,		/* Operations of relation over simple expressions	*/
*op_add,		/* Additive operations on terms		*/
*op_mult;		/* Multiplicative operations on factors	*/

/* Definition of sets presented as bit sets for cases of identifiers usage	*/

extern unsigned

*set_VARCONFUNCS,	/* Acceptable use of VARS, CONSTS, FUNCS */
*set_VARS,	/* Acceptable use of  VARS		*/
*set_TYPES, 	/*Acceptable use of  TYPES 		*/
*set_CONSTS,	/* Acceptable use of  CONSTS 		*/
*set_TYCON,     /* Acceptable use of  TYPES,CONSTS	*/
*set_FUNCS,     /* Acceptable use of  FUNCS              */
*set_PROCS,     /* Acceptable use of  PROCS              */
*set_FUNPR,     /* Acceptable use of  FUNCS,PROCS       */
*set_VARFUNPR;  /* Acceptable use of  VARS,FUNCS,PROCS  */


