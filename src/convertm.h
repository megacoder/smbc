/*
       convertm.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.org
    home page: http://smbc.airm.net
  
    last update: 2004-09-16
*/

#include "../config.h"

void RinitIconv();
void RdoneIconv();

char * to_scr(char *str);
char * to_sys(char *str);
char * from_sys(char *str);

int RDoCharConvertGet();
void RDoCharConvertSet(int Rs);

