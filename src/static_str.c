/*
       static_str.c
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.org
    home page: http://smbc.airm.net
*/
// -------------------------------------------------------------------
#define _GNU_SOURCE

#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

#include "../config.h"

#include <libintl.h>
#include <stdlib.h>
#include "static_str.h"
#include "fnet.h"

void 
RinitStrings()
{
  Rasprintf(&Rm1, _("Please change screen"));
  Rasprintf(&Rm2, _("size to minimum 80x25"));
  Rasprintf(&Rm3, _("A workgroups list is empty."));
  Rasprintf(&Rm4, _("Founded workgroups."));
  Rasprintf(&Rm5, _("A hosts list in workgroup is empty."));
  Rasprintf(&Rm6, _("Founded hosts in workgroup."));
  Rasprintf(&Rm7, _("A share list in host is empty."));
  Rasprintf(&Rm8, _("Founded shares in host."));
  Rasprintf(&Rm9, _("Can't read files/dirs from host."));
  Rasprintf(&Rm10, _("A files/dirs list in host is empty."));
  Rasprintf(&Rm11, _("Founded files/dirs in host."));

  Rasprintf(&Rm18, _("Workgroups not found."));
  Rasprintf(&Rm19, _("Host is empty."));
  Rasprintf(&Rm20, _("Share is empty."));
  Rasprintf(&Rm21, _("Dir is empty."));
  Rasprintf(&Rm22, _("Workgroup is empty."));
  Rasprintf(&Rm23, _(" is empty."));
  Rasprintf(&Rm24, _("Share "));
  Rasprintf(&Rm25, _("[ Retry ]"));
  Rasprintf(&Rm26, _("[ Wait ]"));
  Rasprintf(&Rm27, _("[ Overwrite ]"));
  Rasprintf(&Rm28, _("[ Retry without asking ]"));
  Rasprintf(&Rm29, _("[ Delete ]"));
  Rasprintf(&Rm30, _("retry delay time : "));
  Rasprintf(&Rm31, _("    found :"));
  Rasprintf(&Rm32, _("Probably "));
  Rasprintf(&Rm33, _(" disk is full or quota exceeded."));
  Rasprintf(&Rm34, _("Retry read workgroups list "));
  Rasprintf(&Rm35, _("Retry read hosts list "));
  Rasprintf(&Rm36, _("Retry read shares list "));
  Rasprintf(&Rm37, _("Can't read workgroup."));
  Rasprintf(&Rm38, _("Can't read workgroups list."));
  Rasprintf(&Rm39, _("Dir "));
  Rasprintf(&Rm40, _("Download list is empty."));
  Rasprintf(&Rm41, _("I can't enter to"));
  Rasprintf(&Rm42, _("Do you want delete"));
  Rasprintf(&Rm43, _("Group "));
  Rasprintf(&Rm44, _("Host "));
  Rasprintf(&Rm45, _("Access denied."));
  Rasprintf(&Rm46, _("Do you want copy"));
  Rasprintf(&Rm48, _("Index for find file now is empty, try letter."));
  Rasprintf(&Rm49, _("Loading find index file from disk to memory, please wait...")); 
  Rasprintf(&Rm50, _("New message")); 
  Rasprintf(&Rm51, _("Do you want quit from smbc?")); 
  Rasprintf(&Rm52, _("S D size  remai per file/directory"));
  Rasprintf(&Rm53, _("time remaing | delay | from | to | group"));
  Rasprintf(&Rm54, _("Try connect to : "));
  Rasprintf(&Rm55, _("Permission denied to"));

  Rasprintf(&Rm58, _("Adding files to download list..."));
  Rasprintf(&Rm59, _("Adding files to upload list..."));
  Rasprintf(&Rm60, _("q-quit | h-help | (d)F5-copy | s-download list | f - search remote file"));
  Rasprintf(&Rm61, _("Enter workgroup name, keys Enter,TAB - next field, Ctrl+p enable/disable interactive filtering, Ctrl+k - quit"));
  Rasprintf(&Rm62, _("Enter hostname name, Enter,TAB - next field, Ctrl+p enable/disable interactive filtering, Ctrl+k - quit"));
  Rasprintf(&Rm63, _("Enter dir name, Enter,TAB - next field, Ctrl+p enable/disable interactive filtering, Ctrl+k - quit"));
  Rasprintf(&Rm64, _("Enter file name, Enter,TAB - next field, Ctrl+p enable/disable interactive filtering, Ctrl+k - quit"));
  Rasprintf(&Rm65, _("q - quit | Enter - go to file | TAB - next field | PageUp,PageDown,Up,Down - manage to list"));

  asprintf(&Rm67, "%s", _("Preparing find index."));
  asprintf(&Rm68, "%s", _("Sort %s panel by filename."));
  asprintf(&Rm69, "%s", _("Sort %s panel by extension."));
  asprintf(&Rm70, "%s", _("Sort %s panel by size."));
  asprintf(&Rm71, "%s", _("%s panel without sorting."));
  asprintf(&Rm72, "%s", _("Reverse sorting in %s panel."));
  asprintf(&Rm73, "%s", _("Ungroup files and directories in %s panel."));
  asprintf(&Rm74, "%s", _("Group files and directories in %s panel."));
  asprintf(&Rm75, "%s", _("Warning. Can't copy %s workgroup."));
  asprintf(&Rm76, "%s", _("Warning. Can't copy %s host."));
  Rasprintf(&Rm77, _("Warning. Can't copy from share."));
  Rasprintf(&Rm78, _("Warning. Can't copy to network."));
  Rasprintf(&Rm79, _("(q)F3-quit | (w)F8-wrap/unwrap"));
  Rasprintf(&Rm80, _("Interactive filtering is off."));
  Rasprintf(&Rm81, _("Interactive filtering is on."));
  Rasprintf(&Rm82, _("q - quit | b - add bookmark and quit | d(DEL)/u - delete/undelete | x(Enter) - go to item"));
  Rasprintf(&Rm83, _("MOVE q - quit | b - add bookmark and quit | d(DEL)/u - delete/undelete | x(Enter) - go to item"));
  Rasprintf(&Rm84, _("q - exit | d(DEL) - delete | u - undelete | space - pause/resume | CTRL-d - delete dir | CTRL-f - flush list"));
  Rasprintf(&Rm85, _("MOVE q - exit | d(DEL) - delete | u - undelete | space - pause/resume | CTRL-d - delete dir | CTRL-f - flush list"));
  Rasprintf(&Rm86, _("right"));
  Rasprintf(&Rm87, _("left"));
  Rasprintf(&Rm88, _("from"));
  Rasprintf(&Rm90, _("o,arrow down - down              u,end - tail off list"));
  Rasprintf(&Rm91, _("p,arrow up   - up               y,home - head off list"));
  Rasprintf(&Rm92, _("i,tab        - change column      d,F5 - download/upload"));
  Rasprintf(&Rm93, _("t            - group/ungroup         w - reverse sorting"));
  Rasprintf(&Rm94, _("x,Enter           - execute a directory"));
  Rasprintf(&Rm95, _("a,insert          - select/unselect one file"));
  Rasprintf(&Rm96, _("l,page up         - one screen up"));
  Rasprintf(&Rm97, _("k,page down       - one screen down"));
  Rasprintf(&Rm98, _("CTRL-d,F8         - delete file or directory"));
  Rasprintf(&Rm99, _("shift-/shift=     - select/unselect all files"));
  Rasprintf(&Rm100, _("numeric *,shift-8 - reverse select"));
  Rasprintf(&Rm101, _("c,F7              - create directory"));
  Rasprintf(&Rm102, _("v,F3              - view text file"));
  Rasprintf(&Rm103, _("g                 - go to group/host/share/dir"));
  Rasprintf(&Rm104, _("e                 - change type of sorting"));
  Rasprintf(&Rm105, _("s                 - download list status"));
  Rasprintf(&Rm106, _("f - search file     r - reload     q - quit     h - help"));
  Rasprintf(&Rm107, _("              More details in README file."));
  Rasprintf(&Rm108, _("Inspiration is my wife Magda (Arida)"));

  Rasprintf(&Rm111, _("Files"));

  Rasprintf(&Rm113, _("to"));
  Rasprintf(&Rm114, _("are different or connection broken."));
  Rasprintf(&Rm115, _("Click on Wait to download letter."));
  Rasprintf(&Rm116, _("Warning"));
  Rasprintf(&Rm117, _("Can't "));
  Rasprintf(&Rm118, _("Connection broken."));
  Rasprintf(&Rm119, _("deleting"));

  Rasprintf(&Rm121, _("file or directory name"));

  Rasprintf(&Rm123, _("T"));
  Rasprintf(&Rm124, _("workgroup, share, file or directory name"));
  Rasprintf(&Rm125, _("size"));

  Rasprintf(&Rm129, _("error saving download/upload list\n"));

  Rasprintf(&Rm131, _("workgroup:"));

  Rasprintf(&Rm133, _("path:"));
  Rasprintf(&Rm134, _("username:"));
  Rasprintf(&Rm135, _("password:"));
  Rasprintf(&Rm136, _("[ OK ]"));
  Rasprintf(&Rm137, _("[ Cancel ]"));
  Rasprintf(&Rm138, _("Go to"));
  Rasprintf(&Rm139, _("Create directory"));
  Rasprintf(&Rm140, _("workgroup"));
  Rasprintf(&Rm141, _("hostname"));
  Rasprintf(&Rm142, _("dir"));
  Rasprintf(&Rm143, _("filename"));
  Rasprintf(&Rm144, _("Workgroup:"));
  Rasprintf(&Rm145, _("Hostname:"));
  Rasprintf(&Rm146, _("Dir:"));
  Rasprintf(&Rm147, _("Filename:"));
  Rasprintf(&Rm148, _("Finding files on the network"));
  Rasprintf(&Rm149, _("Workgroup :"));
  Rasprintf(&Rm150, _("     Host :"));
  Rasprintf(&Rm151, _("      Dir :"));
  Rasprintf(&Rm152, _("[ Quick find (with index) ]"));
  Rasprintf(&Rm153, _("[ Slow find (without index) ]"));
  Rasprintf(&Rm154, _("Find files"));

  Rasprintf(&Rm156, _("Your terminal cannot change defined color.\n"));
  Rasprintf(&Rm157, _("Any color definition from ~/.smbc/smbcrc was ignored.\n"));
  asprintf(&Rm158, "%s", _("Minimum recommended screen size is 80x25, now is %ix%i.\n"));
  Rasprintf(&Rm159, _("Please send raports with bugs to smbc@airm.net.\n"));
  Rasprintf(&Rm162, _("New find index is ready. You can use quick find file."));
  Rasprintf(&Rm163, _("hosts "));

  Rasprintf(&Rm168, _("host:"));
  Rasprintf(&Rm169, _("message:"));
  Rasprintf(&Rm170, _("Send message"));
  Rasprintf(&Rm171, _("S R path to remote and local files"));
  Rasprintf(&Rm172, _("This bookmark already exists."));
  Rasprintf(&Rm173, _("smbc [OPTIONS]*"));
  asprintf(&Rm175, "%s", _("Unable to initialize libsmbclient, error numer %i\n"));
  Rasprintf(&Rm176, _("Probably you don't have a smb config file in ~/smb/smb.conf or ~/etc/samba/smb.conf\n"));
  Rasprintf(&Rm177, _("samba without smbc_set_context function, better is samba 3.0.1 or letter\n"));
  Rasprintf(&Rm178, _("selected files/directories"));

  Rasprintf(&Rm179, _(" new, "));
  Rasprintf(&Rm180, _(" offline, "));
  Rasprintf(&Rm181, _(" new: "));
  Rasprintf(&Rm182, _(" offline: "));

  Rasprintf(&Rm231, _(" complete file "));
  Rasprintf(&Rm232, _("Try later connect to "));
  Rasprintf(&Rm233, _(". Delay time : "));
  Rasprintf(&Rm234, _("downloading"));
  Rasprintf(&Rm235, _("uploading"));
  Rasprintf(&Rm236, _("/s, remaing "));
  Rasprintf(&Rm237, _("m, "));
  Rasprintf(&Rm238, _("read"));
  Rasprintf(&Rm239, _("write"));
  Rasprintf(&Rm240, _("local"));
  Rasprintf(&Rm241, _("remote"));
  asprintf(&Rm242, "%s", _("Can't open config file: %s\n"));
  Rasprintf(&Rm243, _("More functions and parameters you can set up in this configuration file.\n"));
  Rasprintf(&Rm244, _("Example smbcrc file you can find in `docs` directory.\n"));
  asprintf(&Rm245, "%s", _("Can't open RC file %s, probably access denied.\n"));

  Rasprintf(&Rm300, _("Displays this help message"));
  Rasprintf(&Rm301, _("Flush all download lists"));
  Rasprintf(&Rm302, _("Show all download lists"));
  Rasprintf(&Rm303, _("Displays all items form dulistX"));
  Rasprintf(&Rm304, _("Sleep, don't download/upload any files"));
  Rasprintf(&Rm305, _("Receive message from samba."));
  Rasprintf(&Rm306, _("File with samba message, usage only with -m."));
  Rasprintf(&Rm307, _("Displays the current version"));
}

void 
RdoneStrings()
{
  free(Rm1),  free(Rm2),  free(Rm3),  free(Rm4),  free(Rm5),  free(Rm6),  free(Rm7),  free(Rm8),  free(Rm9),  free(Rm10),
  free(Rm11),

  free(Rm18), free(Rm19),
  free(Rm20), free(Rm21), free(Rm22), free(Rm23), free(Rm24), free(Rm25), free(Rm26), free(Rm27), free(Rm28), free(Rm29), 
  free(Rm30), free(Rm31), free(Rm32), free(Rm33), free(Rm34), free(Rm35), free(Rm36), free(Rm37), free(Rm38), free(Rm39),
  free(Rm40), free(Rm41), free(Rm42), free(Rm43), free(Rm44), free(Rm45), free(Rm46),             free(Rm48), free(Rm49), 
  free(Rm50), free(Rm51), free(Rm52), free(Rm53), free(Rm54), free(Rm55),                         free(Rm58), free(Rm59),
  free(Rm60), free(Rm61), free(Rm62), free(Rm63), free(Rm64), free(Rm65),             free(Rm67), free(Rm68), free(Rm69), 
  free(Rm70), free(Rm71), free(Rm72), free(Rm73), free(Rm74), free(Rm75), free(Rm76), free(Rm77), free(Rm78), free(Rm79), 
  free(Rm80), free(Rm81), free(Rm82), free(Rm83), free(Rm84), free(Rm85), free(Rm86), free(Rm87), free(Rm88), 
  free(Rm90), free(Rm91), free(Rm92), free(Rm93), free(Rm94), free(Rm95), free(Rm96), free(Rm97), free(Rm98), free(Rm99), 
  free(Rm100), free(Rm101), free(Rm102), free(Rm103), free(Rm104), free(Rm105), free(Rm106), free(Rm107), free(Rm108), 
               free(Rm111),              free(Rm113), free(Rm114), free(Rm115), free(Rm116), free(Rm117), free(Rm118), free(Rm119),
               free(Rm121),              free(Rm123), free(Rm124), free(Rm125),                                        free(Rm129),
               free(Rm131),              free(Rm133), free(Rm134), free(Rm135), free(Rm136), free(Rm137), free(Rm138), free(Rm139),
  free(Rm140), free(Rm141), free(Rm142), free(Rm143), free(Rm144), free(Rm145), free(Rm146), free(Rm147), free(Rm148), free(Rm149),
  free(Rm150), free(Rm151), free(Rm152), free(Rm153), free(Rm154),              free(Rm156), free(Rm157), free(Rm158), free(Rm159),
                            free(Rm162), free(Rm163),                                                     free(Rm168), free(Rm169),
  free(Rm170), free(Rm171), free(Rm172), free(Rm173),              free(Rm175), free(Rm176), free(Rm177), free(Rm178), free(Rm179),
  free(Rm180), free(Rm181), free(Rm182),

  free(Rm231), free(Rm232), free(Rm233), free(Rm234), free(Rm235), free(Rm236), free(Rm237), free(Rm238), free(Rm239), 
  free(Rm240), free(Rm241), free(Rm242), free(Rm243), free(Rm244), free(Rm245),

  free(Rm300), free(Rm301), free(Rm302), free(Rm303), free(Rm304), free(Rm305), free(Rm306), free(Rm307);
}

