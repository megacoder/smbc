/*
       dulist.h
       GNU Licence

       author: Rafal Michniewicz 
        email: <rafim@data.pl>
       jabber: rafim@jabber.airm.net
    home page: http://smbc.airm.net
*/

#include "../config.h"

int RstrCmpIncludeFirst(const char *srcBig, const char *dstSmall);
void RlistAllDownloadLists();
void RflushAllDownloadLists();
void RdisplayVersion();
void RShowAllItemsFromDUlist(int RnrDulist);
int RsaveMessage(char *Rstr, char *RfileName);
void RreturnHumanDelayTime(char **RtimeText, time_t Rtime);


