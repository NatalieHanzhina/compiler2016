
		/* ФАЙЛ "RUMTABL3.C" - РАБОТА С ТАБЛИЦЕЙ ИМЕН */
#include <stdio.h>
#include <malloc.h>

void writename();


#define MAXHASH  997    /* максимальное значение hash-функции		*/
#define MAXNUM   500    /* количество литер      			*/
				/* в строке для разрешения коллизий	*/

/* Встретится переменная lname - это длина текущего имени в литерах     */
unsigned hashresult;    /* результат hash-функции для текущего имени	*/
char *addrname;		/* адрес имени в таблице имен 			*/

struct zveno     	/* строка для разрешения коллизий		*/
	{int firstfree; /* номер первой свободной позиции литеры        */
							/*    в строке  */
	char namelist[MAXNUM];	/* последовательность имен		*/
	struct zveno *next;	/* указатель на следующую строку        */
			/* с тем же значением hash-функции		*/
	};

typedef struct zveno str;

static str *nametable[MAXHASH],/* массив указателей на первые строки для*/
		/* разрешения коллизий соответственно текущему значению */
		/* hash-функции						*/
    *ptrstr; 	/* указатель на текущую строку для разрешения коллизий  */

static unsigned chcount,	/* счетчик литер текущего имени		*/
		loccount;	/* счетчик литер в выделенной памяти	*/

extern FILE *d;		/* для отладки 					*/


/*------------------------- S E A R C H I N T A B L E ------------------*/
void		 SearchInTable	(name)	/* поиск текущего имени в таб-	*/
			/* лице имен          				*/
				char *name;	/* значение текущего 	*/
			/* имени					*/
				/* результаты - внешние переменные:	*/
			/* addrname - адрес текущего имени в таблице 	*/
			/* имен,  hashresult - результат hash-функции	*/
			/* от текущего имени				*/
{int ind;	/* если ind равен 0, значит, текущее имя не совпадает   */
			/* с очередным именем в строке			*/
hashresult=hash(name);
#ifdef RDB
fprintf(d,"\nИмя - %s,     hashresult==%d\n",name,hashresult);
#endif
if(nametable[hashresult]==NULL) 	/* размещение первой строки     */
					/* для обработки коллизий 	*/
	{ptrstr=(str*) malloc(sizeof(str)); /* выделение участка памяти */
					/* под структуру str            */
	nametable[hashresult]=ptrstr;
	(*ptrstr).firstfree=0;
	(*ptrstr).next=NULL;
	writename(name);/* занесение текущего имени в таблицу имен      */
#ifdef RDB
	fprintf(d,"Cтрока чистая!\n");
#endif
	return;			/* возвращаем адрес имени в таблице     */
	}
else    {
#ifdef RDB
	fprintf(d,"Коллизия! Начинаю поиск.\n");
#endif
	ptrstr=nametable[hashresult];   /* коллизия; ищем имя в таблице */
	loccount=0;
	search:
	do	{/* установка начальных параметров для сравнения        */
		/* искомого имени с очередным именем в таблице:         */
		ind=1;chcount=0;
		addrname=&((*ptrstr).namelist[loccount]);
		/* (адрес устанавливается по первой литере имени)       */

		while((addrname[chcount] !='\0')&&(ind==1)
				&&(lname>=chcount))
			{if(addrname[chcount] !=name[chcount])
				ind=0; /* имена не совпадают		*/
			else {loccount++;chcount++;}
			}
		if(ind==1)
			{
#ifdef RDB
			fprintf(d,"Имя найдено!\n");
#endif
			return;}		/* имя найдено		*/
	/* переход к следующему имени в строке:				*/
		while((*ptrstr).namelist[loccount++] !='\0');
		}
	while(loccount<(*ptrstr).firstfree); /* пока не встретим первую */
					     /* свободную ячейку        */
	/* (в случае, когда ее номер равен MAXNUM, все ячейки в строке  */
	/* заняты)							*/
	if((*ptrstr).next !=NULL) /* если существует следующая строка   */
				  /* с тем же значением hash-функции    */
		{ptrstr=(*ptrstr).next; /* ...то переходим на нее       */
		loccount=0;
#ifdef RDB
		fprintf(d,"Продолжаю поиск в следующей строке!\n");
#endif
		goto search;            /* ...и продолжаем поиск        */
		}
	else    /* поиск окончен, имя не найдено			*/
		{if(lname<MAXNUM-(*ptrstr).firstfree)
		/* (если наше имя вместится в остаток этой строки...)   */
			{writename(name);/* ...то мы его туда и запишем!*/
#ifdef RDB
			fprintf(d,"Добавила имя в эту же строку!\n");
#endif
			return;
			}
		else    {/* места для записи имени нет,-                */
			/* выделяем новую строку			*/
#ifdef RDB
			fprintf(d,"Выделяю новую строку...   ");
#endif
			(*ptrstr).next=(str*) malloc(sizeof(str));
			ptrstr=(*ptrstr).next;
			(*ptrstr).next=NULL;
			(*ptrstr).firstfree=0;
			writename(name);/* заносим имя в новую строку	*/
#ifdef RDB
			fprintf(d,"Записала в новой чистой строке!\n");
#endif
			return;
			}
		}
	}
}

/*--------------------------- W R I T E N A M E ------------------------*/
void		 writename	(name)	/* занесение текущего имени 	*/
			/* в таблицу имен		   		*/
				char name[];	/* заносимое имя	*/
				/* вызывается из SearchInTable(), все	*/
			/* прочие аргументы берет из нее - внешние ста- */
			/* тические переменные в файле RUMTABL3.C	*/
{
#ifdef RDB
fprintf(d,"writename   ");
#endif
addrname=&((*ptrstr).namelist[(*ptrstr).firstfree]);
	/* установили адрес заносимого имени 				*/
#ifdef RDB
fprintf(d,"заносим имя: печатаю политерно:  ");
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

/*-------------------------- К О Н Е Ц   Ф А Й Л А --------------------*/

