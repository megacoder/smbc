/*
       debug.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/
#define _GNU_SOURCE

#include "../config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "debug.h"
#include "fnet.h"

char *RdbgFile = NULL;

void 
RinitDebugm(char *Rstr)
{
  int Ri;

  Ri = strlen(Rstr)+1;
  RdbgFile = Rmalloc(Ri);
  memcpy(RdbgFile, Rstr, Ri);
}

void 
RfreeDebugm()
{
  free(RdbgFile);
  RdbgFile = NULL;
}

void 
RDBGm(char *Rstr)
{
  int Rdesc;
  char *Rms = "!!! debug message >> Rstr == NULL << debug message !!!\n";

  Rdesc = open(RdbgFile, O_CREAT | O_NONBLOCK | O_RDWR | O_APPEND, 0600); 
  if (Rdesc > 0) 
  {
    if (Rstr) 
      write(Rdesc, Rstr, strlen(Rstr));
    else
      write(Rdesc, Rms, strlen(Rms));
    close(Rdesc);
  }
}

int 
RDBGim(int Ri)
{
  char *Rstr;

  asprintf(&Rstr, "%i", Ri);
  RDBGm(Rstr);
  free(Rstr);
  return(Ri);
}

void 
RDBGcm(char Rc)
{
  char *Rstr;

  asprintf(&Rstr, "%c", Rc);
  RDBGm(Rstr);
  free(Rstr);
}
