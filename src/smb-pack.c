/*
       smb-pack.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/

#include "../config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else  /* not HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#endif /* not HAVE_STRING_H */

#include <sys/types.h>
#include <signal.h>
#include <curses.h>
#include <libsmbclient.h>
#include <unistd.h>
#include <stdlib.h>
#include "smb-pack.h"
#include "debug.h"

#ifdef HAVE_SMBCCTX

void
RmsgOff(int *RfdNull, int *RfdOut, int *RfdErr)
{
  *RfdErr = dup(STDERR_FILENO);
  *RfdOut = dup(STDOUT_FILENO);

  if ((*RfdNull = open("/dev/null", O_WRONLY)) < 0) 
    kill(0, SIGTERM);
  dup2(*RfdNull, STDERR_FILENO);
  dup2(*RfdNull, STDOUT_FILENO);
}

void
RmsgOn(int *RfdNull, int *RfdOut, int *RfdErr)
{
  dup2(*RfdErr, STDERR_FILENO);
  close(*RfdErr);
  *RfdErr = -1;

  dup2(*RfdOut, STDOUT_FILENO);
  close(*RfdOut);
  *RfdOut = -1;

  close(*RfdNull);
  *RfdNull = -1;
}

#ifdef HAVE_SET_CONTEXT
SMBCCTX * Rsmbc_set_context(SMBCCTX * new_context)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  SMBCCTX *Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_set_context(new_context);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}
#endif /* HAVE_SET_CONTEXT */

int Rsmbc_free_context(SMBCCTX * context, int shutdown_ctx)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  int Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_free_context(context, shutdown_ctx);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

SMBCCTX * Rsmbc_init_context(SMBCCTX * context)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  SMBCCTX *Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_init_context(context);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

SMBCCTX * Rsmbc_new_context(void)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  SMBCCTX *Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_new_context();
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}
#endif

int Rsmbc_init(smbc_get_auth_data_fn fn, int debug)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  int Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_init(fn, debug);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

int Rsmbc_rmdir(const char *durl)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  ssize_t Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_rmdir(durl);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

ssize_t Rsmbc_write(int fd, void *buf, size_t bufsize)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  ssize_t Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_write(fd, buf, bufsize);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

int Rsmbc_unlink(const char *furl)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  int Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_unlink(furl);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

ssize_t Rsmbc_read(int fd, void *buf, size_t bufsize)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  ssize_t Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_read(fd, buf, bufsize);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

off_t Rsmbc_lseek(int fd, off_t offset, int whence)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  off_t Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_lseek(fd, offset, whence);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

int Rsmbc_mkdir(const char *durl, mode_t mode)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  int Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_mkdir(durl, mode);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}


int Rsmbc_fstat(int fd, struct stat *st)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  int Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_fstat(fd, st);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

int Rsmbc_open(const char *furl, int flags, mode_t mode)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  int Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_open(furl, flags, mode);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

int Rsmbc_closedir(int Rfd)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  int Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_closedir(Rfd);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

int Rsmbc_close(int Rfd)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  int Rret;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rret = smbc_close(Rfd);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rret);
}

int Rsmbc_opendir(char *Rstr)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;
  int Rfd;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  Rfd = smbc_opendir(Rstr);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  return(Rfd);
}

int Rsmbc_readdir(int Rfd, struct smbc_dirent **Rdirent)
{
  int RfdNull = -1;
  int RfdOut = -1;
  int RfdErr = -1;

  RmsgOff(&RfdNull, &RfdOut, &RfdErr);
  *Rdirent = smbc_readdir(Rfd);
  RmsgOn(&RfdNull, &RfdOut, &RfdErr);

  if (*Rdirent)
    return(1);
  else
    return(0);
}

