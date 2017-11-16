/*
       convert.h

       author: Mikhail Kshevetskiy
        email: <kl@laska.dorms.spbu.ru>
  
  last update: 2004-08-30

*/

#ifndef _CONVERT_H
#define _CONVERT_H

/* see convert.c for declaration */
struct iconv_param;

/*    Allocate and initialise iconv_param structure used to   */
/* character translation from from_charset to to_charset.     */
/* If conv=0 then there are no translation at all.            */
/*    On success return a pointer to allocated iconv_param    */ 
/* structure or NULL in other case                            */
struct iconv_param* iconv_init(int conv, 
	const char *to_charset, const char *from_charset);

/* Release allocated memory and destroy structure pointed by  */
/* param.                                                     */
void   iconv_done(struct iconv_param *param);

/* Return a translation of src from one charset to another    */
/* using convertation defined by structute *param. Structure  */
/* should be created using iconv_init().                      */
const char* iconv_convert(struct iconv_param *param, const char *src);

#endif /* _CONVERT_H */


