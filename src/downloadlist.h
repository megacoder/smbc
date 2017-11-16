/*
       downloadlist.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/

#include "../config.h"

TTdownload *RgetdownloadListNr(TTdownload *RdownloadList, int Rnr);
TTdownload *RgetdownloadListLast(TTdownload *RdownloadList);
TTdownload *RgetdownloadListFirst(TTdownload *RdownloadList);
int RgetdownloadListCount(TTdownload *RdownloadList);
void RfreeAllTTdownload(TTdownload **RdownloadList);
void RfreeOneTTdownloadAndGoToFirst(TTdownload **RdownloadList);

TTdownload *RcreateEmptyTTdownload();

int RgetHumanSize(char *Rdst, int RstrMaxLen, unsigned int Rsrc);
int RgetHumanSizeMalloc(char **Rdst, int RstrMaxLen, unsigned int Rsrc);

void RaddOneTTdownloadToLast(TTdownload **RdownloadList,
                             char *username, char *password,
                             char *groupnameR, char *hostname, char *pathRLbase, char *pathR, char *pathL,
                             char *fullfilename, char *filename, unsigned int sizeR,
                             char *sizeDisplayR, char *sizeDisplayDownR, 
                             char *percent, char active, char upload, 
                             time_t delayRetry, time_t nextRetry);

