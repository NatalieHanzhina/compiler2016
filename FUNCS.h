
		 /* FILE "FUNCS.H" - Function TYpes definition*/



typedef int BOOLEAN;

void	  /* lexer functions: */ 	/* files:     */
    nextsym(),                                          /* nextsym4.c */
    addsym(),                                           /* nextsym4.c */
    testkey(),                                          /* nextsym4.c */
    number();                                           /* nextsym4.c */
void      /* syntax functions  */
    accept(),                                           /* litfuncs.c */
    programme(),                                        /* last.c */
    SearchInTable(),                                    /* last.c */
    skipto1(),                                          /* litfuncs.c */
    skipto2();                                          /* litfuncs.c */


unsigned
   *convert_to_bits();					/* main.c	*/

void
   SetDisjunct();					/* litfuncs.c	*/



