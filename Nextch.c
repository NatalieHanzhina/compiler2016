
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
  positionnow.linenumber++;			/* прибавим N строки */

if(feof(srcfile) || result == NULL || line == NULL ||  result==0)		/* если достигнут конец файла */
     {    
   fprintf( dstfile, "\nКoмпиляция окончена: ошибок");
   if ( ErrorCount == 0 )
      fprintf( dstfile, " нет !\n");
      else fprintf( dstfile, " - %3d !\n",ErrorCount );

   fclose( srcfile );
   fclose( dstfile );
   return( endoffile );
} 			/* сообщим об этом */
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
//printf("%d %s\n",Errlist[ k ].errorcode,messages[Errlist[ k ].errorcode]);
}
  }

  if( ErrorOverflow )
    fprintf( dstfile, "***** Слишком много ошибок в этой строке ! \n");
  ErrInx=0;
  ErrorOverflow=FALSE;
}

void ListThisLine ()   /* печать строки */
{ if( positionnow.linenumber==printed ) return;
  printed=positionnow.linenumber;
  fprintf( dstfile, "%4d  ", positionnow.linenumber);
  fprintf( dstfile, "%s\n", line );
  if( ErrInx ) ListErrors();
}
char nextch () /* взятие следующего символа */
{
  if( lineindex == LastInLine ) {
    ListThisLine() ;  
int i=ReadNextLine();   
printf("%d",i);           	/* печать текущей строки */
    if( i == endoffile ) {	/* чтение следующей строки */
       ch=endoffile;return( ch);
    }
  }
  if((ch=line[ lineindex++ ])==9) {
     indent=6;
     positionnow.charnumber=((positionnow.charnumber+8)/8)*8;  /* табуляция */
  }
  else positionnow.charnumber++;   /* любой другой символ */
  return( ch );
}﻿
