/*
       smb-pack.h
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

#include <libsmbclient.h>

#define RstrCmp(src, dst) (!strcmp(src, dst))

#ifdef HAVE_SMBCCTX
SMBCCTX * Rsmbc_set_context(SMBCCTX * new_context);
int Rsmbc_free_context(SMBCCTX * context, int shutdown_ctx);
SMBCCTX * Rsmbc_init_context(SMBCCTX * context);
SMBCCTX * Rsmbc_new_context(void);
#endif

int Rsmbc_init(smbc_get_auth_data_fn fn, int debug);
int Rsmbc_rmdir(const char *durl);
ssize_t Rsmbc_write(int fd, void *buf, size_t bufsize);
int Rsmbc_unlink(const char *furl);
ssize_t Rsmbc_read(int fd, void *buf, size_t bufsize);
off_t Rsmbc_lseek(int fd, off_t offset, int whence);
int Rsmbc_mkdir(const char *durl, mode_t mode);
int Rsmbc_fstat(int fd, struct stat *st);
int Rsmbc_open(const char *furl, int flags, mode_t mode);
int Rsmbc_closedir(int Rfd);
int Rsmbc_close(int Rfd);
int Rsmbc_opendir(char *Rstr);
//struct smbc_dirent *Rsmbc_readdir(int Rfd);
int Rsmbc_readdir(int Rfd, struct smbc_dirent **Rdirent);

