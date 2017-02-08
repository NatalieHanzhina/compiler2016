



		/* ФАЙЛ "LITFUNCS.C" - МАЛЕНЬКИЕ ФУНКЦИИ */
#include <stdio.h>	



typedef int Boolean;

/*---------------------------- B E L O N G -----------------------------*/
/*	Функция belong. Осуществляет поиск указанного элемента в множестве.
	Возвращает истину в случае наличия элемента, иначе ложь.
*/
Boolean
belong( element, set )
unsigned element;	/* номер элемента, который ищем */
unsigned *set;	/* множество, в котором ищем элемент */
{  unsigned WordNumber,	/* номер слова строки, в к-м может быть элемент */
       BitNumber;       /* номер бита, соответствующего элементу */
   WordNumber = element / WORDLENGTH;
   BitNumber = element % WORDLENGTH;
   return( set[WordNumber] & (1<<(WORDLENGTH-1-BitNumber)) );
}

/*----------------------- S e t D i s j u n c t ------------------------*/
/*	Функция SetDisjunct. Осуществляет дизъюнкцию множеств set1 и set2.
	Результатом становится множество set3.
*/
void SetDisjunct( set1, set2, set3 )
unsigned set1[], set2[], set3[];
{  unsigned i;	/* параметр цикла */
   for( i=0; i<SET_SIZE; ++i ) set3[i] = set1[i] | set2[i];
}

/*----------------------------- A C C E P T ----------------------------*/
void		 accept		(symbolexpected)	/* проверяет,	*/
			/* совпадает ли сканируемый символ с ожидаемым	*/
				unsigned symbolexpected;	/* код 	*/
			/* ожидаемого символа				*/
				/* результат не возвращает, а разбирает-*/
			/* ся сама: если обнаружено совпадение - берет	*/
			/* следующий символ ( вызывает nextsym() ),	*/
			/* иначе - ничего не делает			*/
{if(symbol == symbolexpected) nextsym();
	else
	  { Error(symbolexpected);
	    fprintf(t," Обнаружена ошибка %d (%d)",symbolexpected,symbol);
	  }
}

/*---------------------------- S K I P T O 1 ---------------------------*/
void		 skipto1	(set)	/* пропуск символов, пока не	*/
			/* встречен символ, принадлежащий данному мно-	*/
			/* жеству                			*/
				unsigned *set;	/* указатель на данное 	*/
			/* множество					*/
{
while((!belong(symbol,set))&&(symbol!=endoffile)) nextsym();
}

/*---------------------------- S K I P T O 2 ---------------------------*/
void		 skipto2	(set1,set2)	/* пропуск символов,	*/
			/* пока не встречен символ, принадлежащий одно-	*/
			/* му из данных множеств 		        */
				unsigned *set1;	/* первое из данных	*/
			/* множеств					*/
				unsigned *set2;	/* второе из данных	*/
			/* множеств					*/
{
while((!belong(symbol,set1))&&(!belong(symbol,set2))&&(symbol!=endoffile))
nextsym();
}

/*======================== К О Н Е Ц   Ф А Й Л А =======================*/﻿
