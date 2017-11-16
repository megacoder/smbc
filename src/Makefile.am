bin_PROGRAMS = smbc

smbc_SOURCES = smbc.c \
              smb-pack.c smb-pack.h \
              fnet.c fnet.h \
              ierror.c ierror.h \
              find-index-manage.c find-index-manage.h \
              debug.c debug.h \
              dulist.c dulist.h \
              winpopup.c winpopup.h \
              convert.c convert.h \
              convertm.c convertm.h \
              bookmarks.c bookmarks.h \
              ttlist.c ttlist.h \
              static_str.c static_str.h \
              windows.c windows.h \
              downloadlist.c downloadlist.h \
              smb-ext.c smb-ext.h

smbc_CFLAGS = -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -Wall
CFLAGS = -O3

if GDB
  smbc_CFLAGS += -ggdb -g
endif

smbc_LDADD = @LIBINTL@

localedir = $(prefix)/share/locale
mandir = $(prefix)/share/man
INCLUDES = -I../intl -DLOCALEDIR=\"$(localedir)\"

