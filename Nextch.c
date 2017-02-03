
/*
			���� ����� - ������
			   ���� NEXTCH.C
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern FILE *t;

extern struct E Errlist[ ERRMAX ];
static int printed;		/* ����� ��������� ������������ ������  */
int    lineindex,	/* ������ ������� ������ � ������� line */
       indent;   	/* ������ ������ ������� ������ (��� ���������) */



int ReadNextLine ()      /* ������ ��������� ������ ��������� ����� */
{ char *result;
  indent=0; 		/* ������ ��������� ��� */
  result = fgets( line, MAXLEN, srcfile );	/* ������� ������ */
  positionnow.linenumber++;			/* �������� N ������ */

if(feof(srcfile) || result == NULL || line == NULL ||  result==0)		/* ���� ��������� ����� ����� */
     {    
   fprintf( dstfile, "\n�o�������� ��������: ������");
   if ( ErrorCount == 0 )
      fprintf( dstfile, " ��� !\n");
      else fprintf( dstfile, " - %3d !\n",ErrorCount );

   fclose( srcfile );
   fclose( dstfile );
   return( endoffile );
} 			/* ������� �� ���� */
  LastInLine = strlen( line );
  if( line[ LastInLine-1 ]=='\n' )
     line[ LastInLine-1 ]=' ';  		/* ������� \n */

  positionnow.charnumber = lineindex = 0;     	/* ���. ������ = 0 */
  return( 0 );                        /* ��� ��������� */
}

void ListErrors () /* ������ ��������� �� ������� */
{ short k,l;
  if( (ErrorCount+ErrInx) > 99 ) {
     fputs("***** ������� ����� ������ � ��������� !", dstfile);
     fprintf(dstfile, "\n���������� �������� : ������ - %d !\n", ErrorCount );
     exit(0);
  }
  ErrorCount += ErrInx;

     /* ----- ������ ������ (� ����� �� k) ----- */
  for( k=1; k <= ErrInx; ++k ) {
    fprintf( dstfile, "**%.2d**", ErrorCount - ErrInx + k );
	 /* ----- ��������� ��������� � ������ ������� ----- */
       for( l=1; l < (Errlist[  k ].errorposition.charnumber-indent); l++ )
	  fprintf( dstfile, " ");
       fprintf( dstfile, "^ ������ ��� %4d \n",Errlist[ k ].errorcode );
	 /* ----- ������ ������ ��������� �� ������ ----- */
    if(( Errlist[ k ].errorcode < 0 )||( Errlist[ k ].errorcode > MAXMES ))
       fputs( "****** ����� ��������� ��� ���������� �������� !\n", dstfile );
    else if( messages[Errlist[k].errorcode]==NULL )
       fputs( "****** ������ ��������� ��� � ����� ������ !\n", dstfile );
	  else {fprintf( dstfile, "****** %s\n", messages[ Errlist[ k ].errorcode ] );
//printf("%d %s\n",Errlist[ k ].errorcode,messages[Errlist[ k ].errorcode]);
}
  }

  if( ErrorOverflow )
    fprintf( dstfile, "***** ������� ����� ������ � ���� ������ ! \n");
  ErrInx=0;
  ErrorOverflow=FALSE;
}

void ListThisLine ()   /* ������ ������ */
{ if( positionnow.linenumber==printed ) return;
  printed=positionnow.linenumber;
  fprintf( dstfile, "%4d  ", positionnow.linenumber);
  fprintf( dstfile, "%s\n", line );
  if( ErrInx ) ListErrors();
}
char nextch () /* ������ ���������� ������� */
{
  if( lineindex == LastInLine ) {
    ListThisLine() ;  
int i=ReadNextLine();   
printf("%d",i);           	/* ������ ������� ������ */
    if( i == endoffile ) {	/* ������ ��������� ������ */
       ch=endoffile;return( ch);
    }
  }
  if((ch=line[ lineindex++ ])==9) {
     indent=6;
     positionnow.charnumber=((positionnow.charnumber+8)/8)*8;  /* ��������� */
  }
  else positionnow.charnumber++;   /* ����� ������ ������ */
  return( ch );
}
