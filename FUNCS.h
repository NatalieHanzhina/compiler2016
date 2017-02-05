
		 /* ФАЙЛ "FUNCS.H" - ОПИСАНИЕ НЕЦЕЛЫХ ФУНКЦИЙ */
/* ПОДКЛЮЧАЕТСЯ ПРЕНЕПРЕМЕННО ОПОСЛЯ ПОДКЛЮЧЕНИЙ ФАЙЛОВ TYPES.H И LISTREC.H */


typedef int BOOLEAN;

void	  /* Функции лексического анализатора: */ 	/* файлы:     */
    nextsym(),                                          /* nextsym.c */
    issymbol(),                                         /* nextsym.c */
    isstring(),                                         /* nextsym.c */
    letter(),                                           /* nextsym.c */
    letter1(),                                          /* nextsym.c */
    addsym(),                                           /* nextsym.c */
    testkey(),                                          /* nextsym.c */
    testkey1(),                                         /* nextsym.c */
    number();                                           /* nextsym.c */

void      /* Функции синтаксиса  */
    accept(),                                           /* rumltlfn.c */
    programme(),                                        /* rumstcom.c */
    SearchInTable(),                                    /* rumtabl3.c */
    skipto1(),                                          /* rumltlfn.c */
    skipto2();                                          /* rumltlfn.c */



unsigned
   *convert_to_bits();					/* main.c	*/

void
   SetDisjunct();					/* litfuncs.c	*/


﻿
