#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"


MODULE = Teste		PACKAGE = Teste		

long
teste(entrada, n)
	char* entrada
	int n
	CODE:
		printf("Recebi %s e %ld \n", entrada, n);
		RETVAL = 42;
	OUTPUT:
		RETVAL

