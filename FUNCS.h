
		 /* ФАЙЛ "FUNCS.H" - ОПИСАНИЕ НЕЦЕЛЫХ ФУНКЦИЙ */
/* ПОДКЛЮЧАЕТСЯ ПРЕНЕПРЕМЕННО ОПОСЛЯ ПОДКЛЮЧЕНИЙ ФАЙЛОВ TYPES.H И LISTREC.H */


typedef int BOOLEAN;

void	  /* Функции лексического анализатора: */ 	/* файлы:     */
    nextsym(),                                          /* nextsym4.c */
    addsym(),                                           /* nextsym4.c */
    testkey(),                                          /* nextsym4.c */
    number();                                           /* nextsym4.c */

void      /* Функции синтаксиса  */
    accept(),                                           /* litfuncs.c */
    programme(),                                        /* last.c */
    SearchInTable(),                                    /* last.c */
    skipto1(),                                          /* litfuncs.c */
    skipto2();                                          /* litfuncs.c */



unsigned
   *convert_to_bits();					/* main.c	*/

void
   SetDisjunct();					/* litfuncs.c	*/


﻿
