
	  /* ERROR CODES LIST  */

#define E_BSIMTYPE	1	/* Error in a simple type		*/
#define E_IDENT 	2	/* The name is expected			*/
#define E_PRG		3	/* There must be a word PROGRAM is expected	*/
#define E_COLON		5 	/* ':' is expected		*/
#define E_BSYM		6	/* Forbidden character			*/
#define E_OF		8 	/* OF is expected	*/
#define E_BTYPE		10 	/* Simple type error		*/
#define E_DESPART	18	/* Error in declaration part		*/
#define E_FLIST		19 	/* Error in fields list		*/
#define E_BCONST	50 	/* Error in constant			*/
#define E_ASSIGN        51      /* := is expected                */
#define E_TWOPOI	74 	/* '..' is expected		*/
#define E_CHARC		75	/* ' is expected			*/
#define E_STRC		76 	/* Too long string constant	*/
#define E_NRCOMM	86	/* Comment is not closed		*/
#define E_REDEC		101	/* Name is redeclared			*/
#define E_LOWUPP	102	/* The lower limit exceeds the upper limit	*/
#define E_NOREC		105	/* Invalid recursive definition */
#define E_NOFILE	108	/* File can not be used here	*/
#define E_NOREAL	109	/* Type must not be  REAL		*/
#define E_CNFLWDIS	111	/* Incompatibility with discriminant type*/
#define E_BLIMTYPE	112	/* Invalid limited type	*/
#define E_REIBAS	114	/* Basis type should not be REAL	*/
					/* or INTEGER			*/
#define E_BDIS		118	/* Invalid discriminant type	*/
#define E_TYPCNFL	145	/* Type conflict			*/
#define E_REIIND	149	/* Type of index can not be REAL 	*/
#define E_SETBAS 	169	/* Error in the base of the set		*/
#define E_NOPACK	170 	/* Type can not be packed		*/
#define E_SCOUT		195	/* Constant out of the described range		*/
#define E_BSC		196	/* Invalid type of constant				*/
#define E_BINTC		203	/* Integer constant exceeds the limit	*/
#define E_LREALC	206	/* Too small real constant			*/
#define E_BREALC	207     /* Too large real constant			*/

/*======================== END OF FILE =======================*/
