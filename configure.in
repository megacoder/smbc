#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT([smbc],[1.2.2],[rafim@data.pl])
AC_CONFIG_HEADER(config.h)

AC_COPYRIGHT("SMBC - Samba Commander - GNU Licence - http://smbc.airm.net")
AC_REVISION($Revision: 1.2 $)

AC_LANG(C)

echo
echo "Samba Commander version $PACKAGE_VERSION"
echo "Configuring..."
echo

#AC_PROG_CC
AC_ISC_POSIX

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/smbc.c)
AM_INIT_AUTOMAKE

AC_PROG_INSTALL

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT

AC_HEADER_STDC([])

AC_CHECK_HEADERS([libsmbclient.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([sys/param.h])
AC_CHECK_HEADERS([sys/file.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([libintl.h])
AC_CHECK_HEADERS([popt.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([termios.h])
AC_CHECK_HEADERS([curses.h])
AC_CHECK_HEADERS([dirent.h])
AC_CHECK_HEADERS([signal.h])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([ctype.h])
AC_CHECK_HEADERS([stdbool.h])
AC_CHECK_HEADERS([iconv.h])

ALL_LINGUAS="pl ru"
AM_GNU_GETTEXT

GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
#AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["${GETTEXT_PACKAGE}"], [gettext domain])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_TYPE_SIGNAL
AC_FUNC_STAT

AC_CHECK_FUNC(alarm)
AC_CHECK_FUNC(dup2)
AC_CHECK_FUNC(mkdir)
AC_CHECK_FUNC(rmdir)
AC_CHECK_FUNC(setlocale)
AC_CHECK_FUNC(strdup)
AC_CHECK_FUNC(strstr)

AC_CHECK_LIB(m,
             main, 
             [], 
             [AC_CHECK_LIB(m, main)]
            )

AC_CHECK_LIB(popt,  
             main, 
             [], 
             [AC_CHECK_LIB(popt, main)]
            )

AC_CHECK_LIB(ncurses,  
             main, 
             [], 
             [AC_CHECK_LIB(ncurses, main)]
            )

AC_CHECK_LIB(smbclient, 
             main, 
             [], 
             [AC_CHECK_LIB(smbclient, main)]
            )

AC_CHECK_LIB(smbclient, 
             smbc_set_context,
             [AC_DEFINE(HAVE_SET_CONTEXT, 1, [Define to 1 of libsmbclient have smbc_set_context function.])],
            )

AC_CHECK_LIB(smbclient, 
             smbc_new_context,
             [AC_DEFINE(HAVE_SMBCCTX, 1, [Define to 1 of libsmbclient have SMBCCTX type.])],
            )

AC_ARG_WITH(debug,
  [  --with-debug            create debug message in to ~/.smbc/debug (default disabled)],
  [ac_cv_func_debug=yes AC_DEFINE(
                          [HAVE_DEBUG], 
                          [], 
                          [HAVE_DEBUG - activate debug message]
                          )
  ],
  ac_cv_func_debug=no
  )

AC_ARG_ENABLE(gdb,
  [  --enable-gdb            turn on debugging for ggdb],
  [case "${enableval}" in
     yes) gdb=yes ;;
      no) gdb=no ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-gdb) ;;
   esac],
  [gdb=no])

AM_CONDITIONAL(GDB, test x$gdb = xyes)

AC_CONFIG_FILES([ Makefile src/Makefile po/Makefile.in intl/Makefile m4/Makefile doc/Makefile ])
AC_OUTPUT

echo
echo "Configure finished."
echo "*** setting ****************************************"
echo "Install prefix           : $prefix"
echo "smbc_set_context         : $ac_cv_lib_smbclient_smbc_set_context"
echo "smbc_new_context         : $ac_cv_lib_smbclient_smbc_new_context"
echo "****************************************************"
echo 
echo "*** for developers *********************************"
echo "debug message            : $ac_cv_func_debug"
echo "compile for ggdb message : $gdb"
echo "****************************************************"
echo "Now you can run"
echo "make"
echo
