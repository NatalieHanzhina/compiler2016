
/*
			ERROR.C - adding error to list of errors
*/


#include <stdio.h>	 
#include <stdlib.h>
extern struct textposition token;			/* current token */
extern short ErrInx, ErrorOverflow;			/* errors counts */
//extern unsigned nocode;
struct E Errlist[ ERRMAX ];      			/* list of errors */

void Error (unsigned errorcode)   /* adding error to list */
{
	
  if ( ErrInx == ERRMAX )
  ErrorOverflow=1;  /* true */
  else {
    ++ ErrInx;
    Errlist[ ErrInx ].errorcode = errorcode;
    Errlist[ ErrInx ].errorposition.linenumber = token.linenumber;
    Errlist[ ErrInx ].errorposition.charnumber = token.charnumber;
  }
  //nocode=1;

}
