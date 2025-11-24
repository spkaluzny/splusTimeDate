#include "sptd_utils.h"
char *sptd_acopy_string(const char *instring) {  
char *outstring ;
size_t len = strlen(instring) ;
if (len > 0) { 
  outstring = (char *) R_alloc(1 + len, sizeof(char)) ;
  strcpy(outstring, instring) ;
} else {
  outstring = "" ;
}
return outstring ;
}

