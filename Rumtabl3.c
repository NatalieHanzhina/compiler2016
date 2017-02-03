
		/* ���� "RUMTABL3.C" - ������ � �������� ���� */
#include <stdio.h>
#include <malloc.h>

void writename();


#define MAXHASH  997    /* ������������ �������� hash-�������		*/
#define MAXNUM   500    /* ���������� �����      			*/
				/* � ������ ��� ���������� ��������	*/

/* ���������� ���������� lname - ��� ����� �������� ����� � �������     */
unsigned hashresult;    /* ��������� hash-������� ��� �������� �����	*/
char *addrname;		/* ����� ����� � ������� ���� 			*/

struct zveno     	/* ������ ��� ���������� ��������		*/
	{int firstfree; /* ����� ������ ��������� ������� ������        */
							/*    � ������  */
	char namelist[MAXNUM];	/* ������������������ ����		*/
	struct zveno *next;	/* ��������� �� ��������� ������        */
			/* � ��� �� ��������� hash-�������		*/
	};

typedef struct zveno str;

static str *nametable[MAXHASH],/* ������ ���������� �� ������ ������ ���*/
		/* ���������� �������� �������������� �������� �������� */
		/* hash-�������						*/
    *ptrstr; 	/* ��������� �� ������� ������ ��� ���������� ��������  */

static unsigned chcount,	/* ������� ����� �������� �����		*/
		loccount;	/* ������� ����� � ���������� ������	*/

extern FILE *d;		/* ��� ������� 					*/

   /* ������� ������ ������ ��������� ���������� ������ -- � ����� ! */

/*------------------------- S E A R C H I N T A B L E ------------------*/
void		 SearchInTable	(name)	/* ����� �������� ����� � ���-	*/
			/* ���� ����          				*/
				char *name;	/* �������� �������� 	*/
			/* �����					*/
				/* ���������� - ������� ����������:	*/
			/* addrname - ����� �������� ����� � ������� 	*/
			/* ����,  hashresult - ��������� hash-�������	*/
			/* �� �������� �����				*/
{int ind;	/* ���� ind ����� 0, ������, ������� ��� �� ���������   */
			/* � ��������� ������ � ������			*/
hashresult=hash(name);
#ifdef RDB
fprintf(d,"\n��� - %s,     hashresult==%d\n",name,hashresult);
#endif
if(nametable[hashresult]==NULL) 	/* ���������� ������ ������     */
					/* ��� ��������� �������� 	*/
	{ptrstr=(str*) malloc(sizeof(str)); /* ��������� ������� ������ */
					/* ��� ��������� str            */
	nametable[hashresult]=ptrstr;
	(*ptrstr).firstfree=0;
	(*ptrstr).next=NULL;
	writename(name);/* ��������� �������� ����� � ������� ����      */
#ifdef RDB
	fprintf(d,"C����� ������!\n");
#endif
	return;			/* ���������� ����� ����� � �������     */
	}
else    {
#ifdef RDB
	fprintf(d,"��������! ������� �����.\n");
#endif
	ptrstr=nametable[hashresult];   /* ��������; ���� ��� � ������� */
	loccount=0;
	search:
	do	{/* ��������� ��������� ���������� ��� ���������        */
		/* �������� ����� � ��������� ������ � �������:         */
		ind=1;chcount=0;
		addrname=&((*ptrstr).namelist[loccount]);
		/* (����� ��������������� �� ������ ������ �����)       */

/*======================== ������ ��� �������: =========================*/
#ifdef RDB
fprintf(d,"�������� �� ��������� ��� � ������: %s; ������� %d",addrname,
								loccount);
fprintf(d," ���� ���: %s\n",name);
#endif
/*====================== ����� ������ ��� �������! =====================*/

		while((addrname[chcount] !='\0')&&(ind==1)
				&&(lname>=chcount))
			{if(addrname[chcount] !=name[chcount])
				ind=0; /* ����� �� ���������		*/
			else {loccount++;chcount++;}
			}
		if(ind==1)
			{
#ifdef RDB
			fprintf(d,"��� �������!\n");
#endif
			return;}		/* ��� �������		*/
	/* ������� � ���������� ����� � ������:				*/
		while((*ptrstr).namelist[loccount++] !='\0');
		}
	while(loccount<(*ptrstr).firstfree); /* ���� �� �������� ������ */
					     /* ��������� ������        */
	/* (� ������, ����� �� ����� ����� MAXNUM, ��� ������ � ������  */
	/* ������)							*/
	if((*ptrstr).next !=NULL) /* ���� ���������� ��������� ������   */
				  /* � ��� �� ��������� hash-�������    */
		{ptrstr=(*ptrstr).next; /* ...�� ��������� �� ���       */
		loccount=0;
#ifdef RDB
		fprintf(d,"��������� ����� � ��������� ������!\n");
#endif
		goto search;            /* ...� ���������� �����        */
		}
	else    /* ����� �������, ��� �� �������			*/
		{if(lname<MAXNUM-(*ptrstr).firstfree)
		/* (���� ���� ��� ��������� � ������� ���� ������...)   */
			{writename(name);/* ...�� �� ��� ���� � �������!*/
#ifdef RDB
			fprintf(d,"�������� ��� � ��� �� ������!\n");
#endif
			return;
			}
		else    {/* ����� ��� ������ ����� ���,-                */
			/* �������� ����� ������			*/
#ifdef RDB
			fprintf(d,"������� ����� ������...   ");
#endif
			(*ptrstr).next=(str*) malloc(sizeof(str));
			ptrstr=(*ptrstr).next;
			(*ptrstr).next=NULL;
			(*ptrstr).firstfree=0;
			writename(name);/* ������� ��� � ����� ������	*/
#ifdef RDB
			fprintf(d,"�������� � ����� ������ ������!\n");
#endif
			return;
			}
		}
	}
}

/*--------------------------- W R I T E N A M E ------------------------*/
void		 writename	(name)	/* ��������� �������� ����� 	*/
			/* � ������� ����		   		*/
				char name[];	/* ��������� ���	*/
				/* ���������� �� SearchInTable(), ���	*/
			/* ������ ��������� ����� �� ��� - ������� ���- */
			/* �������� ���������� � ����� RUMTABL3.C	*/
{
#ifdef RDB
fprintf(d,"writename   ");
#endif
addrname=&((*ptrstr).namelist[(*ptrstr).firstfree]);
	/* ���������� ����� ���������� ����� 				*/
#ifdef RDB
fprintf(d,"������� ���: ������� ���������:  ");
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

/*-------------------------- � � � � �   � � � � � --------------------*/

