



		/* FILE "LITFUNCS.C" - SMALL FUNCTIONS */
#include <stdio.h>	


typedef int Boolean;

/*---------------------------- B E L O N G -----------------------------*/
/*	Function belong. Performs search of given element in set.
Returns true, if element exists, else - false.
*/
Boolean
belong( element, set )
unsigned element;	/* element to find */
unsigned *set;	/* set where to find */
{  unsigned WordNumber,	/* number of string where element is expected to be found */
       BitNumber;       /* number of bit according element */
   WordNumber = element / WORDLENGTH;
   BitNumber = element % WORDLENGTH;
   return( set[WordNumber] & (1<<(WORDLENGTH-1-BitNumber)) );
}

/*----------------------- S e t D i s j u n c t ------------------------*/
/*	Function SetDisjunct. Performs desjuntion of two sets: set1 and set2.
	set3 is a result.
*/
void SetDisjunct( set1, set2, set3 )
unsigned set1[], set2[], set3[];
{  unsigned i;	/* loop variable */
   for( i=0; i<SET_SIZE; ++i ) set3[i] = set1[i] | set2[i];
}

/*----------------------------- A C C E P T ----------------------------*/
void		 accept		(symbolexpected)	/* checks	*/
			/* if current symbol is expected one	*/
				unsigned symbolexpected;	/* code of 	*/
			/* expected symbol				*/
				/* doesn't return result*/
			/* if current symbol is expected one	*/
			/* function takes the following symbol ( calls nextsym() ),	*/
			/* else - nothing			*/
{if(symbol == symbolexpected) nextsym();
	else
	  { Error(symbolexpected);
	    fprintf(t," Error: %d (%d)",symbolexpected,symbol);
	  }
}

/*---------------------------- S K I P T O 1 ---------------------------*/
void		 skipto1	(set)	/*  skips symbols until it mets symbol from given set*/
				unsigned *set;	
{
while((!belong(symbol,set))&&(symbol!=endoffile)) nextsym();
}

/*---------------------------- S K I P T O 2 ---------------------------*/
void		 skipto2	(set1,set2)	/* skips symbols until it mets symbol from given sets		        */
				unsigned *set1;	
				unsigned *set2;	
{
while((!belong(symbol,set1))&&(!belong(symbol,set2))&&(symbol!=endoffile))
nextsym();
}
