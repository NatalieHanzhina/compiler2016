
/*
			Блок ввода - вывода
			   Файл NEXTCH.C
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern FILE *t;

extern struct E Errlist[ ERRMAX ];
static int printed;		/* номер последней напечатанной строки  */
int    lineindex,	/* индекс текущей литеры в массиве line */
       indent;   	/* отступ выдачи текущей строки (для табуляции) */



int ReadNextLine ()      /* чтение следующей строки исходного файла */
{ char *result;
  indent=0; 		/* сдвига табуляции нет */
  result = fgets( line, MAXLEN, srcfile );	/* прочтем строку */
  ++positionnow.linenumber;			/* прибавим N строки */

if(result == NULL || line == NULL)		/* если достигнут конец файла */
     { return( 1 );} 			/* сообщим об этом */
  LastInLine = strlen( line );
  if( line[ LastInLine-1 ]=='\n' )
     line[ LastInLine-1 ]=' ';  		/* удалить \n */

  positionnow.charnumber = lineindex = 0;     	/* тек. символ = 0 */
  return( 0 );                        /* все нормально */
}

void ListErrors () /* печать сообщений об ошибках */
{ short k,l;
  if( (ErrorCount+ErrInx) > 99 ) {
     fputs("***** Слишком много ошибок в программе !", dstfile);
     fprintf(dstfile, "\nКомпиляция окончена : ошибок - %d !\n", ErrorCount );
     exit(0);
  }
  ErrorCount += ErrInx;

     /* ----- Печать ошибок (в цикле по k) ----- */
  for( k=1; k <= ErrInx; ++k ) {
    fprintf( dstfile, "**%.2d**", ErrorCount - ErrInx + k );
	 /* ----- установка указателя в нужную позицию ----- */
       for( l=1; l < (Errlist[  k ].errorposition.charnumber-indent); l++ )
	  fprintf( dstfile, " ");
       fprintf( dstfile, "^ ошибка код %4d \n",Errlist[ k ].errorcode );
	 /* ----- печать текста сообщения об ошибке ----- */
    if(( Errlist[ k ].errorcode < 0 )||( Errlist[ k ].errorcode > MAXMES ))
       fputs( "****** Номер сообщения вне допустимых пределов !\n", dstfile );
    else if( messages[Errlist[k].errorcode]==NULL )
       fputs( "****** Такого сообщения нет в файле данных !\n", dstfile );
	  else {fprintf( dstfile, "****** %s\n", messages[ Errlist[ k ].errorcode ] );
}
  }

  if( ErrorOverflow )
    fprintf( dstfile, "***** Слишком много ошибок в этой строке ! \n");
  ErrInx=0;
  ErrorOverflow=FALSE;
}

int ListThisLine ()   /* печать строки */
{ if( positionnow.linenumber==printed ) return 0;
  printed=positionnow.linenumber;
  fprintf( dstfile, "%4d  ", positionnow.linenumber);
  fprintf( dstfile, "%s\n", line );
  if( ErrInx ) ListErrors();
return 0;
}

int nextch () /* взятие следующего символа */
{
  if( lineindex == LastInLine ) {
    ListThisLine() ;  
    int i=ReadNextLine();   
           	/* печать текущей строки */
    if( i ) {	/* чтение следующей строки */
       ch=endoffile;return( ch);
    }
  }
  if((ch=line[ lineindex++ ])==9) {
     indent=6;
     positionnow.charnumber=((positionnow.charnumber+8)/8)*8;  /* табуляция */
  }
  else positionnow.charnumber++;   /* любой другой символ */
  return( ch );
}
