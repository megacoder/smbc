/*
       winpopup.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <libsmbclient.h>
#include <unistd.h>
#include "winpopup.h"
#include "debug.h"

/*
    libsmbclient dont have function to send WinPopup message.
    RsendWinpopup is a frontend for smbclient.
*/

void RsendWinpopup(char *RpathToSmbclient, char *Rrcp, char *Rtxt)
{
  char *Rstring;
  int RfdNull;
  int RfdOut; 
  int RfdErr; 

  RfdErr = dup(STDERR_FILENO);
  RfdOut = dup(STDOUT_FILENO);

  if ((RfdNull = open("/dev/null", O_WRONLY)) < 0) {
     printf("Can't open /dev/null\n");
     exit(0);
  };
  dup2(RfdNull, STDERR_FILENO);
  dup2(RfdNull, STDOUT_FILENO);

  asprintf(&Rstring, "/bin/echo \"%s\" | %s -M %s", Rtxt, RpathToSmbclient, Rrcp);
  system(Rstring);
  free(Rstring);

  dup2(RfdErr, STDERR_FILENO);
  dup2(RfdOut, STDOUT_FILENO);

  close(RfdErr);
  close(RfdOut);

  close(RfdNull);
}

