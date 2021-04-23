/*************************************************************************
 *
 * © 1998-2012 TIBCO Software Inc. All rights reserved. 
 * Confidential & Proprietary 
 *
 *************************************************************************/

#include "timeUtils.h"

R_len_t i ;

/* get the list element named str, or return NULL */  
SEXP getListElement(SEXP list, const char *str)
{
  SEXP elmt = R_NilValue, names = getAttrib(list, R_NamesSymbol);
  
  for (i = 0; i < length(list); i++)
    if(strcmp(CHAR(STRING_ELT(names, i)), str) == 0) {
      elmt = VECTOR_ELT(list, i);
      break;
    }
  return elmt;
}

int checkClass(SEXP x, const char **valid, const int P)
{

  int match=0L;
  int ret=-1;
  int i;
  SEXP cl=getAttrib(x, R_ClassSymbol);

  const char* cl_str = CHAR(STRING_ELT(getAttrib(x, R_ClassSymbol), 0));

  for(i=0; i<P; i++){
    match = strcmp(cl_str, valid[i]);
    if(match==0) break;
  }
  if(match!=0) ret=0;
  else ret=1;

  return ret;
}
