
/*
			Файл ERROR.C - занесение
		   cведений об ошибке в список ошибок
*/


#include <stdio.h>	 /* отладка					*/
#include <stdlib.h>
extern struct textposition token;			/* тек. лексема */
extern short ErrInx, ErrorOverflow;			/* счетчики ошибок */
//extern unsigned nocode;
struct E Errlist[ ERRMAX ];      			/* список ошибок */

void Error (unsigned errorcode)   /* занесение ошибки в список */
{
	
  if ( ErrInx == ERRMAX )
  ErrorOverflow=1;  /* истина */
  else {
    ++ ErrInx;
    Errlist[ ErrInx ].errorcode = errorcode;
    Errlist[ ErrInx ].errorposition.linenumber = token.linenumber;
    Errlist[ ErrInx ].errorposition.charnumber = token.charnumber;
  }
  //nocode=1;

}
