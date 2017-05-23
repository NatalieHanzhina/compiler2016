
		/* FILE "RUMTABL3.C" WORKS WITH THE NAME TABLE */
#include <stdio.h>
#include <malloc.h>

void writename();


#define MAXHASH  997    /*The maximum value of the hash function		*/
#define MAXNUM   500    /* Number of characters per line to resolve collisions	*/

/* There will be a variable lname - this is the length of the current name in the letters     */
unsigned hashresult;    /* The result of the hash function for the current name	*/
char *addrname;		/* Address of name in the table of names 			*/

struct zveno     	/* String for resolution collisions		*/
	{int firstfree; /* The number of the first free position of the letter in the line  */
	char namelist[MAXNUM];	/* Sequence of names		*/
	struct zveno *next;	/* Pointer to the next line with the same value of the hash function		*/
	};

typedef struct zveno str;

static str *nametable[MAXHASH],/* Array of pointers to the first lines to resolve collisions according to the current value of the hash function						*/
    *ptrstr; 	/* A pointer to the current line to resolve collisions  */

static unsigned chcount,	/* Counter of the current name		*/
		loccount;	/* Character counter in dedicated memory	*/

extern FILE *d;		/* debug file					*/


/*------------------------- S E A R C H I N T A B L E ------------------*/
void		 SearchInTable	(name)	/* Search for the current name in the table of names          				*/
				char *name;	/* The value of the current name, the results are external variables: addrname - the address of the current name in the name table, hashresult - the result of the hash function from the current name				*/
{int ind;	/* If ind is 0, then the current name does not match the next name in the string			*/
hashresult=hash(name);
#ifdef RDB
fprintf(d,"\nИмя - %s,     hashresult==%d\n",name,hashresult);
#endif
if(nametable[hashresult]==NULL) 	/* Placing the first row to handle collisions 	*/
	{ptrstr=(str*) malloc(sizeof(str)); /* Allocating a space under the structure str            */
	nametable[hashresult]=ptrstr;
	(*ptrstr).firstfree=0;
	(*ptrstr).next=NULL;
	writename(name);/* Entering the current name in the table of names      */
#ifdef RDB
	fprintf(d,"Cтрока чистая!\n");
#endif
	return;			/* Return the name in the table     */
	}
else    {
#ifdef RDB
	fprintf(d,"Коллизия! Начинаю поиск.\n");
#endif
	ptrstr=nametable[hashresult];   /* Collision; Looking for a name in the table */
	loccount=0;
	search:
	do	{/* Setting the initial parameters for comparing the searched name with the next name in the table:         */
		ind=1;chcount=0;
		addrname=&((*ptrstr).namelist[loccount]);
		/* (The address is set by the first letter of the name)       */

		while((addrname[chcount] !='\0')&&(ind==1)
				&&(lname>=chcount))
			{if(addrname[chcount] !=name[chcount])
				ind=0; /* The names do not match		*/
			else {loccount++;chcount++;}
			}
		if(ind==1)
			{
#ifdef RDB
			fprintf(d,"Name found!\n");
#endif
			return;}		
	/* Go to next name in row:				*/
		while((*ptrstr).namelist[loccount++] !='\0');
		}
	while(loccount<(*ptrstr).firstfree); /* Until we meet the first free cell (in the case where its number is MAXNUM, all cells in the line are occupied)							*/
	if((*ptrstr).next !=NULL) /* If there is a following line with the same value of the hash function    */
		{ptrstr=(*ptrstr).next; /* ...Then go to it       */
		loccount=0;
#ifdef RDB
		fprintf(d,"continue searching in the next line!\n");
#endif
		goto search;           
		}
	else    /* Search is over, name not found			*/
		{if(lname<MAXNUM-(*ptrstr).firstfree)
		/* (If  name fits into the rest of this line)   */
			{writename(name);/* ...Then we will take it there and write it down*/
#ifdef RDB
			fprintf(d,"Added a name in the same line!\n");
#endif
			return;
			}
		else    {/* There is no place for recording the name, -                */
			/* Select a new line			*/
#ifdef RDB
			fprintf(d,"Select a new line...   ");
#endif
			(*ptrstr).next=(str*) malloc(sizeof(str));
			ptrstr=(*ptrstr).next;
			(*ptrstr).next=NULL;
			(*ptrstr).firstfree=0;
			writename(name);/* Enter the name in a new line	*/
#ifdef RDB
			fprintf(d,"Recorded in a new line!\n");
#endif
			return;
			}
		}
	}
}

/*--------------------------- W R I T E N A M E ------------------------*/
void		 writename	(name)	/* Entering the current name in the table of names		   		*/
				char name[];	/* The stored name is called from SearchInTable ()	*/
{
#ifdef RDB
fprintf(d,"writename   ");
#endif
addrname=&((*ptrstr).namelist[(*ptrstr).firstfree]);
	/* Set the address of the recorded name 				*/
#ifdef RDB
fprintf(d,"Write a name:  ");
#endif
for(chcount=0,loccount=(*ptrstr).firstfree;name[chcount] !='\0';
		chcount++,loccount++)
	{
	(*ptrstr).namelist[loccount]=name[chcount];
#ifdef RDB
	fprintf(d,"%c",name[chcount]);
#endif
	}
(*ptrstr).namelist[loccount++]='\0';
(*ptrstr).firstfree=loccount;
#ifdef RDB
fprintf(d,"\n");
#endif
}

/*-------------------------- END OF FILE--------------------*/

