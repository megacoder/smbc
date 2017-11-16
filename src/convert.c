/*
       convert.h

       author: Mikhail Kshevetskiy
        email: <kl@laska.dorms.spbu.ru>

     modyfied: Rafal Michniewicz
  
  last update: 2004-08-30

*/

#include "../config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <iconv.h>
#include "convert.h"
#include "debug.h"
#include "fnet.h"

struct iconv_param
{
    int		conv;
    iconv_t	cd;
    int		dst_len;
    char 	*dst;
};

struct 
iconv_param* iconv_init(int conv, const char *to_charset, const char *from_charset){
		
    struct iconv_param *param = NULL;
    
    param = (struct iconv_param*) Rmalloc(sizeof(struct iconv_param));
    if (param == NULL) 
      return NULL;	

    if (to_charset == NULL) 
      conv = 0;
    if (from_charset == NULL) 
      conv = 0;
    if (conv)
    {
      param->cd = iconv_open(to_charset, from_charset);
      if (param->cd == (iconv_t) -1) 
      {
        conv = 0; 
        perror("iconv_open() failed:");
      }
    }
    param->conv = conv;
    param->dst_len = 0;
    param->dst = NULL;
    return param;
}

void 
iconv_done(struct iconv_param *param)
{
  if (param == NULL) 
    return;	
  if (param->conv) 
    iconv_close(param->cd);
  if (param->dst != NULL) 
    Rfree(param->dst);
  param->dst_len = 0;
  param->dst = NULL;
  Rfree(param);
}

const char* 
iconv_convert(struct iconv_param *param, const char *src)
{
  if (param == NULL) 
    return src;
  if (!param->conv) 
    return src;
  if (!strlen(src)) 
    return src;
    
  int src_len = strlen(src) + 1;
  if (param->dst_len < 2*src_len )
  {                                    // for local to UTF-8 conversion
  // Need some error handling here
    param->dst = (char*) realloc(param->dst, 2*src_len);
    param->dst_len = 2*src_len; 
  }

  char *inbuf  = (char*) src;
  char *outbuf = param->dst;
  int  inbytesleft  = src_len - 1;
  int  outbytesleft = param->dst_len - 1;
  bzero(param->dst, param->dst_len);
  // Need some error handling here
  iconv(param->cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    
  iconv(param->cd, NULL, 0, NULL, 0);
  return param->dst;
}

