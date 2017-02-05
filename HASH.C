
		   /* ФАЙЛ "HASH.C" - ХЭШ-ФУНКЦИЯ */
hash(name)
char *name;
{
   int i=0, result=0;

   while( name[i]!=0 ) result+=name[i++]|i;
   return(abs( result%997 ));
}
﻿
