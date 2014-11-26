#ifndef MSG_READ_H
#define MSG_READ_H

#include <time.h>

#include "struct.h"
#include "msgapi.h"
#include "msgs.h"
#include "msgread_ini.h"
#include "msg_ll.h"

typedef struct {

	char From[XMSG_FROM_SIZE];
	char To[XMSG_TO_SIZE];
	char Subj[XMSG_SUBJ_SIZE];
	char AreaName[61];
	struct _stamp date_written;
	struct _stamp date_arrived;
	char *Text;
	int colors[4];
	unsigned int replyto, replies[MAX_REPLY];
	unsigned int num_msg, cur_msg, high_msg, high_water;
	unsigned long attr;
	NETADDR orig;
	NETADDR dest;
	
}MsgInfoRec;

class msg_read {

    private:
    PASSING      *pass;       // Socket Info
    UserRec      *thisuser;   // User Info
    
    msg_func     _menuf;      // Message Base I/O
    msgread_ini  _mread_ini;  // Message Reader INI for Text box
    
    // Message API Functions
    MsgInfoRec    MI;
    MSGA         *AHandle;
    XMSG          xmsg;
    mb_list_rec   mr;
    MSGH         *mh;   
    HMSG          hmsg;    
    
    typedef struct MsgHead {

        char curmsg[10];
        char totmsg[10];
        char from[30];
        char to[30];
        char subj[80];
        char flags[10];
        char time[20];
        char area[30];
    
    }MsgHead;
    
    MsgHead mHead;    
    msg_ll  mLink;
            
    int Top;
    int Bot;
    
    public:      
    msg_read();
    void start(PASSING *passing, UserRec *user);
    
    // Parsing the Message Header
    void ParseMRead(char *filename);
          
    // Message Posting Functions
    int  get_subject(XMSG *xm);
    int  get_to(XMSG *xm);
    void get_address(XMSG *xm);
    int  SaveMsg(unsigned long msgarea, unsigned long msgnum, int New);    
    void PostMsg(int mbnum, bool Reply);
    void MakeCtrlHdr(char *reply);
    void GetMsgID(char *reply);
    void fill_xmsg(char *from, char *to, char *subj);
    void PostReplyMsg(int mbnum);
          
    // Message Reader Functions
    void Add2MsgInfo();
    void CloseMsgArea();
    int  OpenMsgArea(unsigned long mbnum);
    time_t msg_read::stampToTimeT(struct _stamp *st);
    struct _stamp *msg_read::timeTToStamp(time_t tt);
    void FidoFlags(char *fflags);
    void SetupMsgHdr();
    char *strrepl(char *Str, size_t BufSiz, const char *OldStr, const char *NewStr);
    void msg_read::stripCR(char *ostr);
    void MsgSetupTxt();
    void MsgShowTxt2();
    void GetMsg();
    int  SquishAreaSetLast(unsigned long usr,unsigned long lr);
    void SetLastRead(unsigned long usr, unsigned long lr);
    unsigned long GetLastRead(unsigned long usr);
    unsigned long SquishAreaGetLast(unsigned long usr);
    int  ReadMsg(unsigned long mbnum, unsigned long  msgnum, int showit);
    int  ReadOrScanMsgs(int ros, int multi);
    bool ReadMessages(unsigned long marea);
    void start_reading();
    

};




#endif
