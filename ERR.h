

      /* ФАЙЛ  ERR.H                    */
       #include "TEXTP.h"

      /* структура - данные об  ошибках */
      struct E
	    {
	    struct textposition errorposition;
	      /* место  ошибки */
	    unsigned errorcode;                 /* код    ошибки */
	    };

