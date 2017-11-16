/*
       convertm.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.org
    home page: http://smbc.airm.net
  
    last update: 2004-09-16
*/

#include "../config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

#include <langinfo.h>
#include "convert.h"

struct iconv_param *iconv_utf_to_screen=NULL;
struct iconv_param *iconv_utf_to_system=NULL;
struct iconv_param *iconv_system_to_utf=NULL;

int RDoCharConvert = 1;

void 
RinitIconv()
{
 iconv_utf_to_screen=iconv_init(RDoCharConvert, nl_langinfo(CODESET), "UTF-8");
 iconv_utf_to_system=iconv_init(RDoCharConvert, nl_langinfo(CODESET), "UTF-8");
 iconv_system_to_utf=iconv_init(RDoCharConvert, "UTF-8", nl_langinfo(CODESET));
}

void 
RdoneIconv()
{
  iconv_done(iconv_utf_to_screen);
  iconv_done(iconv_utf_to_system);
  iconv_done(iconv_system_to_utf);
}

char *
to_scr(char *str)
{
  if (str == NULL)
    return(NULL);
  return (char *)iconv_convert(iconv_utf_to_screen, str);
}

char *
to_sys(char *str)
{
  if (str == NULL)
    return(NULL);
  return (char *)iconv_convert(iconv_utf_to_system, str);
}

char *
from_sys(char *str)
{
  if (str == NULL)
    return(NULL);
  return (char *)iconv_convert(iconv_system_to_utf, str);
}

int 
RDoCharConvertGet()
{
  return RDoCharConvert;
}

void
RDoCharConvertSet(int Rs)
{
  RDoCharConvert = Rs;
}
