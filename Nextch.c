
/*
			Input-output module
			   NEXTCH.C
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern FILE *t;

extern struct E Errlist[ ERRMAX ];
static int printed;		/* Number of last printed line  */
int    lineindex,	/* Current literal index in array line */
       indent;   	/* The indentation of the output of the current line (for tabulation) */



int ReadNextLine ()      /* Reading the next line of the source file */
{ char *result;
  indent=0; 		/* No tab shift */
  result = fgets( line, MAXLEN, srcfile );	/* Read the line */
  ++positionnow.linenumber;			/* Add the line number */

if(result == NULL || line == NULL)		/* If the end of the file is reached */
     { return( 1 );} 			
  LastInLine = strlen( line );
  if( line[ LastInLine-1 ]=='\n' )
     line[ LastInLine-1 ]=' ';  		/* remove \n */

  positionnow.charnumber = lineindex = 0;     	/* cur symbol = 0 */
  return( 0 );                        
}

void ListErrors () /* Printing of error messages */
{ short k,l;
  if( (ErrorCount+ErrInx) > 99 ) {
     fputs("***** Too many errors in the program!", dstfile);
     fprintf(dstfile, "\nThe compilation is over: number of errors is %d !\n", ErrorCount );
     exit(0);
  }
  ErrorCount += ErrInx;

     /* ----- Printing errors ----- */
  for( k=1; k <= ErrInx; ++k ) {
    fprintf( dstfile, "**%.2d**", ErrorCount - ErrInx + k );
	 /* ----- Setting the pointer to the desired position ----- */
       for( l=1; l < (Errlist[  k ].errorposition.charnumber-indent); l++ )
	  fprintf( dstfile, " ");
       fprintf( dstfile, "^ error code %4d \n",Errlist[ k ].errorcode );
	 /* ----- Printing the text of the error message ----- */
    if(( Errlist[ k ].errorcode < 0 )||( Errlist[ k ].errorcode > MAXMES ))
       fputs( "****** Message number is out of range!\n", dstfile );
    else if( messages[Errlist[k].errorcode]==NULL )
       fputs( "****** There is no such message in the data file!\n", dstfile );
	  else {fprintf( dstfile, "****** %s\n", messages[ Errlist[ k ].errorcode ] );
}
  }

  if( ErrorOverflow )
    fprintf( dstfile, "***** Too many errors in this line! \n");
  ErrInx=0;
  ErrorOverflow=FALSE;
}

int ListThisLine ()   /* line printing */
{ if( positionnow.linenumber==printed ) return 0;
  printed=positionnow.linenumber;
  fprintf( dstfile, "%4d  ", positionnow.linenumber);
  fprintf( dstfile, "%s\n", line );
  if( ErrInx ) ListErrors();
return 0;
}

int nextch () /* next character getting*/
{
  if( lineindex == LastInLine ) {
    ListThisLine() ;  
    int i=ReadNextLine();   
           	/* current line printing */
    if( i ) {	/* nexr line reading */
       ch=endoffile;return( ch);
    }
  }
  if((ch=line[ lineindex++ ])==9) {
     indent=6;
     positionnow.charnumber=((positionnow.charnumber+8)/8)*8;  /* tab */
  }
  else positionnow.charnumber++;   /* any other character */
  return( ch );
}
