/*
       smb-ext.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/

#include "../config.h"

#include <unistd.h>
#include <stdlib.h>

#define FULLPATH_WITH_SMB 1
#define FULLPATH_WITHOUT_SMB 0

#define VV_USER_AND_PASS 0
#define VV_USER_AND_PASS_INTER 1

#define VI_ST_TO_INTER 0
#define VI_INTER_TO_ST 1

#define V_USERNAME 0
#define V_PASSWORD 1
#define V_GROUP 2
#define V_USERNAME_INTER 3
#define V_PASSWORD_INTER 4

char *RgetStrVariable(int RtypeVariable);
void RsetStrVariable(int RtypeVariable, char *Rstr);
void RsetTwoUserAndPassword(int RtypeVariable, char *RuserI, char *RpassI);
void RcopyInterToStOrOtherWay(int RtypeVariable);
void Rget_auth_data(const char *srv, const char *shr, char *wg, int wglen, char *un, int unlen, char *pw, int pwlen);

void Rget_auth_data(const char *srv, const char *shr, char *wg, int wglen, char *un, int unlen, char *pw, int pwlen);
int RsmbCheckFileExist(TTfind *RrecordI);
char *RreturnFullSmbPath(char RwithSmb, char *Rgroup, char *Rhost, char *Rpath, char *Rfile);

