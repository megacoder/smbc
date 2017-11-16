/*
       smb-ext.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/

#define _GNU_SOURCE

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

#include <signal.h>
#include <stdlib.h>
#include "../config.h"
#include "rtypedef.h"
#include "downloadlist.h"
#include "fnet.h"
#include "debug.h"
#include "smb-ext.h"
#include "smb-pack.h"

char *RusernameInter = NULL;
char *RpasswordInter = NULL;
char *Rusername = NULL;
char *Rpassword = NULL;
char *Ragroup = NULL;

char *
RgetStrVariable(int RtypeVariable)
{
  switch (RtypeVariable)
  {
    case V_USERNAME_INTER: return(Rusername);
    case V_PASSWORD_INTER: return(Rpassword);
    case V_USERNAME      : return(Rusername);
    case V_PASSWORD      : return(Rpassword);
    case V_GROUP         : return(Ragroup);
    default              : return(NULL);
  }
}

void
RsetStrVariable(int RtypeVariable, char *Rstr)
{
  switch (RtypeVariable)
  {
    case V_USERNAME_INTER: RstrdupN(RusernameInter, Rstr);
                           break;
    case V_PASSWORD_INTER: RstrdupN(RpasswordInter, Rstr);
                           break;
    case V_USERNAME      : RstrdupN(Rusername, Rstr);
                           break;
    case V_PASSWORD      : RstrdupN(Rpassword, Rstr);
                           break;
    case V_GROUP         : RstrdupN(Ragroup, Rstr);
                           break;
  }
}

void
RsetTwoUserAndPassword(int RtypeVariable, char *RuserI, char *RpassI)
{
  switch (RtypeVariable)
  {
    case VV_USER_AND_PASS:
         RsetStrVariable(V_USERNAME, RuserI);
         RsetStrVariable(V_PASSWORD, RpassI);
         break;
    case VV_USER_AND_PASS_INTER:
         RsetStrVariable(V_USERNAME_INTER, RuserI);
         RsetStrVariable(V_PASSWORD_INTER, RpassI);
         break;
  }
}

void
RcopyInterToStOrOtherWay(int RtypeVariable)
{
  switch (RtypeVariable)
  {
    case VI_ST_TO_INTER:
         RstrdupN(RusernameInter, Rusername);
         RstrdupN(RpasswordInter, Rpassword);
         break;
    case VI_INTER_TO_ST:
         RstrdupN(Rusername, RusernameInter);
         RstrdupN(Rpassword, RpasswordInter);
         break;
  }
}

void 
Rget_auth_data(const char *srv, const char *shr, char *wg, int wglen, char *un, int unlen, char *pw, int pwlen)
{
  Rstrcpy(wg, -1, Ragroup, -1);
  Rstrcpy(un, -1, Rusername, RMAXGHLEN);
  Rstrcpy(pw, -1, Rpassword, RMAXGHLEN);
}

int
RsmbCheckFileExist(TTfind *RrecordI)
{
  char *RpathCFE = NULL;
  int RfhCFE = 0;

  RpathCFE = RreturnFullSmbPath(FULLPATH_WITH_SMB, 
                                "",
                                RrecordI->host,
                                RrecordI->dir,
                                RrecordI->file);
  RfhCFE = Rsmbc_open(RpathCFE, O_RDONLY, 0755);
  if (RfhCFE > 0)
  {
    Rsmbc_close(RfhCFE);
    RfhCFE = 1;
  }
  else
    RfhCFE = 0;
  Rfree(RpathCFE);
  return(RfhCFE);
}

char *
RreturnFullSmbPath(char RwithSmb, char *Rgroup, char *Rhost, char *Rpath, char *Rfile)
{
  char *Rstr = NULL;

  if (RwithSmb == FULLPATH_WITH_SMB)
    RstrdupN(Rstr, "smb://")
  else
    RstrdupN(Rstr, "//");

  if (strlen(Rgroup))
  {
    Rstraddtoend(&Rstr, Rgroup);
    Rcharaddtoend(&Rstr, '/');
  }
  Rstraddtoend(&Rstr, Rhost);
  Rcharaddtoend(&Rstr, '/');
  Rstraddtoend(&Rstr, Rpath);
  if (strlen(Rfile))
    Rstraddtoend(&Rstr, Rfile);

  return(Rstr);
}
