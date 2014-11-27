
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "struct.h"
#include "msgs.h"
#include "ansiout.h"
#include "msgapi.h"
#include "msg_read.h"
#include "menu_func.h"
#include "misc.h"
#include "msg_fse.h"
#include "language.h"


typedef enum {LOCAL, NETMAIL, ECHOMAIL, EMAIL, NEWS} MSGTYPE;
typedef enum {PUBLIC, PRIVATE} POSTTYPE;


#define MSG_QUOTE    0
#define MSG_TEXT    1
#define MSG_TEAR    2
#define MSG_ORIGIN    3

#define BUFLEN 64000
#define CTLLEN 1024

char          buff[BUFLEN];
static   char cinfbuf[CTLLEN];  // control info buffer
unsigned long buflen = 0L, cinflen = 0L;

unsigned long MH,ML,MO,MR;

msg_read::msg_read() {

    memset(&pass,0,sizeof(PASSING));
    mh      = NULL;
    AHandle = NULL;

    Top = 1;
    Bot = 1;

}

void msg_read::start(PASSING *passing, UserRec *user) {

    pass     = passing;
    thisuser = user;

    // Do quick Message Reader ini parsing
    if (!_mread_ini.msg_exists()) {
        _mread_ini.msg_create();
    }

    _mread_ini.msg_parse();
    Top = _mread_ini.Top;
    Bot = _mread_ini.Bot;
}


void msg_read::ParseMRead(char *filename) {

    char szDateFormat[128];    // System Date
    char szTimeFormat[128];    // System Time

    int c;
    long int size;
    char *AnsiBuf;
    std::string temp;

    std::string path = ANSIPATH;
    path += filename;
    path += ".ans";

    // Get Filesize to Allocate Buffer
    FILE *inStream;
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL) {
        return;
    }
    fseek(inStream, 0L, SEEK_END);    /* Position to end of file */
    size = ftell(inStream);           /* Get file length */
    fclose(inStream);

    // Open and Read Ansi file
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL) {
        return;
    }

    char MCI[3]; // Holds MCI Codes to Parse
    temp = "";   // Holds Ansi
    do {
        memset(&MCI,0,sizeof(MCI));
        c = getc(inStream);

        if (c == '%') {
            MCI[0] = getc(inStream);
            MCI[1] = getc(inStream);

            if (strcmp(MCI,"TI") == 0)      { temp += mHead.time;   }
            else if (strcmp(MCI,"FM") == 0) { temp += mHead.from;   }
            else if (strcmp(MCI,"TO") == 0) { temp += mHead.to;     }
            else if (strcmp(MCI,"FL") == 0) { temp += mHead.flags;  }
            else if (strcmp(MCI,"MA") == 0) { temp += mHead.area;   }
            else if (strcmp(MCI,"SU") == 0) { temp += mHead.subj;   }
            else if (strcmp(MCI,"CM") == 0) { temp += mHead.curmsg; }
            else if (strcmp(MCI,"HM") == 0) { temp += mHead.totmsg; }
            else { temp += c; temp += MCI; }

            temp += getc(inStream);
        }

        else if (c == '\n') temp += '\r';
        else temp += c;

    }
    while (c != EOF);
    fclose(inStream);

    temp += "\r\n"; // Extra Space to Fix Next Ansi Sequence
    while ( pass->session->isActive() ) {
        if (pass->session->puts((char *)temp.c_str()) > 1 ) break;
        Sleep(10);
    }

    return;
}


// Message Posting Functions

/*

void PostNewMsg(int mbnum)
{
    make_msg_tmp();

    MakeCtrlHdr((char *)NULL);

    if(!OpenMsgArea(mbnum)){
        mci("~SMUnable to open message area.~SM~SP");
        return;
        }

    if(mr.Allowalias)
        fill_xmsg(thisuser.alias,xmsg.to,"");
    else
        fill_xmsg(thisuser.realname,xmsg.to,"");

    if(mr.Pubpriv==PRIVATE) {
        *xmsg.to='\0';
        language(1104);
        }
    else{
        strcpy(xmsg.to,"All");
        char2A(1,xmsg.to);
        language(1102);
        }

    get_str(xmsg.to,XMSG_TO_SIZE);
    if(strlen(xmsg.to)<1)   return;

    PostMsg(mbnum);
}



void PostReplyMsg(int mbnum)
{
    int i,fix=FALSE;
    char reply[51];

    *reply='\0';
    GetMsgID(reply);
    strrepl(reply,50,"MSGID","REPLY");

    MakeCtrlHdr(reply);

    MsgExtractTxt();
    if(mr.Allowalias)
        fill_xmsg(thisuser.alias,xmsg.from,xmsg.subj);
    else
        fill_xmsg(thisuser.realname,xmsg.from,xmsg.subj);
    for(i=0;i<9;i++){
        if(!xmsg.replies[i]){
            xmsg.replies[i]=thisuser.lastmsg;
            fix=TRUE;
            }
        }
    if(!fix) xmsg.replies[9]=thisuser.lastmsg;
    MI.replyto=xmsg.replies[9];

    char2A(1,xmsg.to);
    if(mr.Pubpriv==PRIVATE){
        language(1104);
        }
    else{
        language(1102);
        }

    get_str(xmsg.to,XMSG_TO_SIZE);
    if(strlen(xmsg.to)<1)   return;

    PostMsg(mbnum);
}
*/


void msg_read::get_address(XMSG *xm) {

    char *zone,*net,*node,*pt,faddr[21];

    xm->dest.zone = 0;
    xm->dest.net  = 0;
    xm->dest.node = 0;
    xm->dest.point= 0;

    *faddr='\0';

    //language(1113);
    //get_str(faddr,20);

    zone = strtok(faddr,":/.\n\r");
    if(zone) {
        xm->dest.zone = atoi(zone);
        net = strtok(NULL,":/.\n\r");
        if(net){
            xm->dest.net = atoi(net);
            node=strtok(NULL,":/.\n\r");
            if(node) {
                xm->dest.node = atoi(node);
                pt = strtok(NULL,":/.\n\r");
                if(pt) {
                    xm->dest.point = atoi(pt);
                }
            }
        }
    }
}


int msg_read::get_subject(XMSG *xm) {

    char text[100]={0};
    language _lang;
    _lang.lang_get(text,19);
    pipe2ansi(pass,text);

    get_str(pass,(char*)xm->subj);
    if(strlen((const char*)xm->subj) < 1)
        return FALSE;
    else
        return TRUE;
}

int msg_read::get_to(XMSG *xm) {

    char text[100]={0};
    language _lang;
    _lang.lang_get(text,20);
    pipe2ansi(pass,text);

    get_str(pass,(char *)xm->to);
    if(strlen((const char*)xm->to) < 1)
        return FALSE;
    else
        return TRUE;
}


int msg_read::SaveMsg(unsigned long msgarea, unsigned long msgnum, int New) {

    if(OpenMsgArea(msgarea)){
        if(New)
            hmsg = MsgOpenMsg(AHandle, MOPEN_CREATE, 0);
        else
            hmsg = MsgOpenMsg(AHandle, MOPEN_RW, msgnum);

        MsgWriteMsg(hmsg, 0, &xmsg, (byte*)buff, strlen(buff), strlen(buff),
                strlen(cinfbuf), (byte*)cinfbuf);

        MsgCloseMsg(hmsg);
    }
    OpenMsgArea(msgarea);
    return 0;
}


void msg_read::PostMsg(int mbnum, bool Reply) {

    char msgtmp[81];
    struct tm *tm;
    time_t timet;

    OpenMsgArea(mbnum);

    xmsg.orig.zone  = mr.aka.zone;
    xmsg.orig.net   = mr.aka.net;
    xmsg.orig.node  = mr.aka.node;
    xmsg.orig.point = mr.aka.point;

    time(&timet);
    tm = localtime(&timet);
    xmsg.date_written = *timeTToStamp(timet);
    xmsg.date_arrived = *timeTToStamp(timet);

    xmsg.attr=0;
    xmsg.attr |= MSGLOCAL;

    if(mr.Kind == NETMAIL){
        //get_address(&xmsg);  Disable for Now
        xmsg.attr |= MSGPRIVATE;
        xmsg.attr |= MSGCRASH;
    }

    if(!get_subject(&xmsg)) return;
    if(!get_to(&xmsg)) return;

    strcpy((char*)xmsg.from, thisuser->Handle);

    // Start Full Screen Editor... Gets Message Into Buff
    msg_fse _mpost;
    _mpost.start(pass);
    memset(&buff,0,sizeof(buff));
    _mpost.poll_chr(buff);

    language _lang;
    char text[100]={0};
    unsigned char c;

    // Do we really want to save this message?
    _lang.lang_get(text,21);
    pipe2ansi(pass,text);

    // Set this up for lightbar later! MMM MMM
    get_chr(pass,c);
    if((int)c == 13 || toupper(c) == 'Y') {
        // Display Saving Message Text! Add Abort, Continue, Save etc..
    }
    else return;


    // If this is a reply, update the header of the current msg
    if(Reply) {
        // Reply
        SaveMsg(mbnum,thisuser->lastmsg,FALSE);
        xmsg.replyto = thisuser->lastmsg;
        for(int i = 0; i < 10; i++)
            xmsg.replies[i] = 0;
    }

    /*
    if(mr.Type != LOCAL)
        add_origin(&mr); */

    SaveMsg(mbnum,0,TRUE);
}



void msg_read::MakeCtrlHdr(char *reply)
{
    char adrs[21];
    long somenum;

    (void) time(&somenum);

    faddr2char(adrs, &mr.aka);
    if(reply==(char *)NULL){
        sprintf(cinfbuf,"\01MSGID: %s %ld\01PID: %s",
            adrs, somenum, VERSION);
        }
    else{
        sprintf(cinfbuf,"\01MSGID: %s %ld\01%s\01PID: %s",
            adrs, somenum, reply, VERSION);
        }
}

void msg_read::GetMsgID(char *reply)
{
    long lbeg, lend;
    char line[121];
    int  cnt = 0;

    lbeg = 0;
    lend = 0;

    while(lend<cinflen && lbeg<cinflen){
        memset(line,0,120);
        while(( lend<cinflen && lend<(80+lbeg)) && cinfbuf[lend]!='\01') {
            lend++;
        }
        while(( lend>lbeg           &&
                lend>0              &&
                lend>(80+lbeg-10))  &&
                cinfbuf[lend]!='\01'){
                lend--;
        }
        if((lend >= lbeg) && lend <= cinflen){
            memcpy(line,cinfbuf+lbeg,lend-lbeg);
            stripCR(line);
            strcpy(reply,line);
            if(strncmp(line,"MSGID",5) == 0){
                break;
            }
            cnt++;
        }
        else{
            return ;
        }
        lend++;
        lbeg=lend;
    }
}

void msg_read::fill_xmsg(char *from, char *to, char *subj)
{
    strcpy((char*)xmsg.to,to);
    strcpy((char*)xmsg.from,from);
    strcpy((char*)xmsg.subj,subj);
}

void msg_read::PostReplyMsg(int mbnum)
{
    int i, fix = FALSE;
    char reply[51];

    *reply = '\0';
    GetMsgID(reply);
    strrepl(reply,50,"MSGID","REPLY");

    MakeCtrlHdr(reply);

    // Get Message Text that we are replying to here
    // MsgExtractTxt();

    if(mr.Allowalias)
        fill_xmsg(thisuser->Handle,(char*)xmsg.from,(char*)xmsg.subj);
    else
        fill_xmsg(thisuser->Name,(char*)xmsg.from,(char*)xmsg.subj);

    for(i = 0; i < 9; i++){
        if(!xmsg.replies[i]){
            xmsg.replies[i] = thisuser->lastmsg;
            fix = TRUE;
        }
    }
    if(!fix) xmsg.replies[9] = thisuser->lastmsg;
    MI.replyto = xmsg.replies[9];

    //char2A(1,xmsg.to);
    //Get Language prompt here for confirmation of user
    //pass->session->printf(

    /*
    if(mr.Pubpriv == PRIVATE){
        language(1104);
    }
    else{
        language(1102);
    }*/

    /*
    if(!get_to(&xmsg)) return;
    if(strlen((char*)xmsg.to) < 1) return;
    */

    PostMsg(mbnum, TRUE);
}



// Normal Message Reader Functions

void msg_read::Add2MsgInfo() {

    strcpy(MI.AreaName, mr.mbdisplay);
    MI.colors[0]    = mr.colors[0];
    MI.colors[1]    = mr.colors[1];
    MI.colors[2]    = mr.colors[2];
    MI.colors[3]    = mr.colors[3];

    strcpy(MI.From, (const char*)xmsg.from);
    strcpy(MI.To,   (const char*)xmsg.to);
    strcpy(MI.Subj, (const char*)xmsg.subj);
    MI.orig.zone    = xmsg.orig.zone;
    MI.orig.net     = xmsg.orig.net;
    MI.orig.node    = xmsg.orig.node;
    MI.orig.point   = xmsg.orig.point;
    MI.dest.zone    = xmsg.dest.zone;
    MI.dest.net     = xmsg.dest.net;
    MI.dest.node    = xmsg.dest.node;
    MI.dest.point   = xmsg.dest.point;
    MI.attr         = xmsg.attr;
    MI.date_written = xmsg.date_written;
    MI.date_arrived = xmsg.date_arrived;
    MI.replyto      = xmsg.replyto;

    MI.high_water   = AHandle->high_water;
    MI.high_msg     = MsgHighMsg(AHandle);
    MI.cur_msg      = MsgCurMsg(AHandle);
    MI.num_msg      = MsgNumMsg(AHandle);
    MO = MI.cur_msg;
    MH = MI.high_msg;
    ML = 1L;

    for(int i = 0; i < 10; i++) {
        if(xmsg.replies[i] != 0){
            MI.replies[i]   = xmsg.replies[i];
        }
    }
}

void msg_read::CloseMsgArea() {

    if(AHandle != NULL) {
        MsgCloseArea(AHandle);
        AHandle = NULL;
    }
    AHandle = NULL;

}

int msg_read::OpenMsgArea(unsigned long mbnum) {

    char path[81];
    int retval = true;

    if(!_menuf.read_mbaselist(&mr,mbnum)){
        pass->session->printf("\r\nUnable to read message base #%d", mbnum);
        printf("\r\nUnable to read message base #%d", mbnum);
        return FALSE;
    }
    sprintf(path,"%s%s",mr.mbpath, mr.mbfile);

    if (AHandle != NULL) {
        MsgCloseArea(AHandle);
    }

    AHandle = MsgOpenArea((byte *)path, MSGAREA_CRIFNEC, mr.Type);
    if (AHandle == NULL) {
        retval = false;
        printf("\r\nAHandle == NULL");
    }
    else Add2MsgInfo();

    return retval;
}

time_t msg_read::stampToTimeT(struct _stamp *st) {

    time_t tt;
    struct tm tms;

    tms.tm_sec = st->time.ss << 1;
    tms.tm_min = st->time.mm;
    tms.tm_hour = st->time.hh;
    tms.tm_mday = st->date.da;
    tms.tm_mon = st->date.mo - 1;
    tms.tm_year = st->date.yr + 80;
    tms.tm_isdst = -1;
    tt = mktime(&tms);
    return tt;
}

struct _stamp *msg_read::timeTToStamp(time_t tt) {

    struct tm *tmsp;
    static struct _stamp st;

    tmsp = localtime(&tt);
    st.time.ss = tmsp->tm_sec >> 1;
    st.time.mm = tmsp->tm_min;
    st.time.hh = tmsp->tm_hour;
    st.date.da = tmsp->tm_mday;
    st.date.mo = tmsp->tm_mon + 1;
    st.date.yr = tmsp->tm_year - 80;

    return (&st);
}


void msg_read::FidoFlags(char *fflags) {

    if(MI.attr & MSGPRIVATE)
        strcat(fflags,"Private");
    if(MI.attr & MSGCRASH)  {
        if(strlen(fflags)>0)    strcat(fflags,", ");
        strcat(fflags,"Crash");
    }
    if(MI.attr & MSGREAD){
        if(strlen(fflags)>0)    strcat(fflags,", ");
        strcat(fflags,"Read");
    }
    if(MI.attr & MSGLOCAL){
        if(strlen(fflags)>0)    strcat(fflags,", ");
        strcat(fflags,"Local");
    }
}

void msg_read::SetupMsgHdr() {

    time_t tmt;
    struct tm *mtm;
    char faddr[81],timestr[81],fflags[41],namestr[81],tostr[81];
    unsigned int repf,rept;

    rept    = MI.replyto;
    repf    = MI.replies[0];
    *fflags = '\0';
    FidoFlags(fflags);

    if(mr.Kind != LOCAL && mr.Kind != EMAIL){
        if(MI.orig.point)
            sprintf(faddr,"%d:%d/%d.%d",
            MI.orig.zone, MI.orig.net, MI.orig.node,MI.orig.point);
        else
            sprintf(faddr,"%d:%d/%d", MI.orig.zone, MI.orig.net, MI.orig.node);
            sprintf(namestr,"%s@%s",MI.From,faddr);
        }
    else{
        sprintf(namestr,"%s",MI.From);
    }

    if(mr.Kind == NETMAIL){
        if(MI.dest.point)
            sprintf(faddr,"%d:%d/%d.%d",
                MI.dest.zone, MI.dest.net, MI.dest.node,MI.dest.point);
        else
            sprintf(faddr,"%d:%d/%d", MI.dest.zone, MI.dest.net, MI.dest.node);
        sprintf(tostr,"%s@%s",MI.To,faddr);
    }
    else
        sprintf(tostr,"%s",MI.To);

    tmt = stampToTimeT(&MI.date_written);
    mtm = localtime(&tmt);
    strftime(timestr,81,"%H:%M  %m/%d/%Y",mtm);

    sprintf(mHead.curmsg,"%i", thisuser->lastmsg);
    sprintf(mHead.totmsg,"%i", MsgNumMsg(AHandle));
    strcpy(mHead.from,  namestr);
    strcpy(mHead.to,    tostr);
    strcpy(mHead.subj,  MI.Subj);
    strcpy(mHead.flags, fflags);
    strcpy(mHead.time,  timestr);
    strcpy(mHead.area,  MI.AreaName);

}

char *msg_read::strrepl(char *Str, size_t BufSiz, const char *OldStr, const char *NewStr) {

    int OldLen, NewLen;
    char *p, *q;

    if(NULL == (p = strstr(Str, OldStr)))
        return Str;
    OldLen = strlen(OldStr);
    NewLen = strlen(NewStr);
    if ((strlen(Str) + NewLen - OldLen + 1) > BufSiz)
        return NULL;
    memmove(q = p+NewLen, p+OldLen, strlen(p+OldLen)+1);
    memcpy(p, NewStr, NewLen);
    return q;
}

void msg_read::stripCR(char *ostr) {

    while(strchr(ostr,'\n')) strrepl(ostr,500,"\n","");
    while(strchr(ostr,'\r')) strrepl(ostr,500,"\r","");
    while(strchr(ostr,'\b')) strrepl(ostr,500,"\b","");
    while(strchr(ostr,'\x1b')) strrepl(ostr,500,"\x1b","");
}

void msg_read::MsgSetupTxt() {

    std::string MsgText = buff;
    std::string Line;
    int id1 = 0;
    int i = 0;

    // Setup the Message Header
    SetupMsgHdr();

    while(pass->session->isActive()) {

        Line = "";
        id1  = MsgText.find("\r", 1);  // Each Line ends with '\r'

        // Make Sure only to Add New Lines when being used,
        // First Line is already Setup! So Skip it
        if (id1 != -1 && i != 0) mLink.add_to_list("");

        if (id1 == -1) break;
        else {
            Line = MsgText.substr(0,id1);
            MsgText.erase(0,id1);

            if (Line.size() > 0) {
                Line += "\r\n";
                mLink.current_node->data = Line;
                printf("\nSetup Line: %i, Size: %i", i+1, Line.size());
                ++i;
            }
        }
    }
}

void msg_read::GetMsg() {

    strcpy(buff,"");
    cinflen = MsgGetCtrlLen(mh);
    MsgReadMsg(mh,&xmsg,0L,0L,NULL,cinflen,(byte *)cinfbuf);
    cinfbuf[cinflen] = '\0';

    buflen = MsgGetTextLen(mh);
    MsgReadMsg(mh,NULL,0L,buflen,(byte *)buff,0L,NULL);
    Add2MsgInfo();

}

int msg_read::SquishAreaSetLast(unsigned long usr,unsigned long lr) {

    char path[91];
    FILE *fptr;
    int  x = false;
    unsigned long lr1;

    lr1=lr;

    sprintf(path,"%s%s.sql",mr.mbpath,mr.mbfile);
    fptr = fopen(path,"rb+");
    if(fptr == NULL) {
        fptr = fopen(path,"wb");
        if(fptr == NULL)
            return -1;
    }
    if(fseek(fptr,usr * sizeof(unsigned long),SEEK_SET)==0)
        x=fwrite(&lr1,sizeof(unsigned long),1,fptr);
    fclose(fptr);
    return(x);
}


void msg_read::SetLastRead(unsigned long usr, unsigned long lr) {

    switch(mr.Type){
        case MSGTYPE_JAM: break;
        case MSGTYPE_SQUISH:
            if(SquishAreaSetLast(usr,lr) < 0){
                //mci("~SMErrer setting last read.~SM~SP");
                printf("\nError setting last read.");
            }
            break;
    }
}

unsigned long msg_read::SquishAreaGetLast(unsigned long usr) {

    char path[91];
    FILE *fptr;
    unsigned long lr1;

    sprintf(path,"%s%s.sql",mr.mbpath,mr.mbfile);
    fptr = fopen(path,"rb");
    if(fptr == NULL)
        lr1 = 1L;
    else{
        if(fseek(fptr,usr * sizeof(unsigned long),SEEK_SET)==0)
            fread(&lr1,sizeof(unsigned long),1,fptr);
        fclose(fptr);
        }
    return lr1;
}

unsigned long msg_read::GetLastRead(unsigned long usr) {

    unsigned long lr = 0L;

    switch(mr.Type) {
        case MSGTYPE_JAM:   break;
        case MSGTYPE_SQUISH:
            lr = SquishAreaGetLast(usr)+1;
            break;
    }
    return lr;
}

int msg_read::ReadMsg(unsigned long mbnum, unsigned long  msgnum, int showit) {

    int retval=true;

    if(!OpenMsgArea(mbnum)){
        if(showit)
            printf("\nMessage Not Avialable");
            //language(1318);  Message not available.
        retval = false;
    }

    if (msgnum < 1) msgnum = 1;
    if(msgnum < 1 || msgnum > MI.high_msg)  {
        retval = false;
    }

    if(retval){
        mh = MsgOpenMsg(AHandle, MOPEN_RW, msgnum);
        if(mh != NULL){
            GetMsg();
            if(showit)
                MsgSetupTxt(); //(mbnum);
            SetLastRead(thisuser->Number,msgnum);
            MsgCloseMsg(mh);
        }
        else
            retval = false;
    }
    return retval;
}

int msg_read::ReadOrScanMsgs(int ros, int multi) {

    int total_mbases = _menuf.msg_count();
    int retval = true;

    do{
        if(ros) {
            retval = ReadMsg(thisuser->lastmbarea, thisuser->lastmsg, TRUE);
        }
        //else NextTenMsgs(thisuser->lastmbarea);

        if(!retval && multi && thisuser->lastmbarea < total_mbases){
            thisuser->lastmbarea++;

            if(OpenMsgArea(thisuser->lastmbarea)) {
                thisuser->lastmsg = 1L;
                thisuser->lastmsg = GetLastRead(thisuser->Number);
                //language(1250);  Scanning Mesasge Base. ...
            }
            else
                thisuser->lastmbarea--;
            }

        }while(!retval && (thisuser->lastmbarea < total_mbases-1) && multi);

    if(!retval && multi){
        thisuser->lastmbarea--;
    }

    return retval;
}

bool msg_read::ReadMessages(unsigned long marea) {

    if(!OpenMsgArea(marea)) {
        pass->session->puts("\r\nno messages.. .");
        printf("\r\nno messages.. .");
        return false;
    }
    else {
        ReadOrScanMsgs(true,false);
    }
    return true;
}

void msg_read::start_reading() {

    //s_fidoconfig *cfg;
    struct _minf m;
    m.req_version = 2;

    /*
    mi.req_version = 0;
    mi.def_zone = 2;
    //m.haveshare   = 1;
    //m.def_zone  = cfg->addr[0].zone;
    */

    // Startup the Husky Message API
    if (MsgOpenApi(&m)!= 0) {
        printf("\nMsgOpenApi Error.\n");
    }

    // Initalize Menu Function Class
    menu_func _mf;
    _mf.start(pass,thisuser);

    bool done = false;
    bool same = true;
    bool more = false;
    char mString[10] = {0};
    unsigned char ch;
    int count;

    std::string _output;
    char outBuffer[200];

    // Startup Message Reader Link List for Holding Message Lines
    mLink.start(pass);
    mLink.Top = Top;
    mLink.Bot = Bot;

    mLink.add_to_list("");
    mLink.move_up();

    // Run through Mesage Look untill exit from reader prompt
    while (pass->session->isActive() && !done) {

        memset(&mHead,0, sizeof(MsgHead));
        same = true;

        // Read in Current Users Message
        if (!ReadMessages(thisuser->lastmbarea)) {
            printf("\nErr: Unable to read messages");
            done = true;
            mLink.dispose_list();
            same = false;
            break;
        }

        /*
        // Check if message base is empty
        if (strcmp(mHead.totmsg,"") == 0) {
            printf("\r\nErr: Unable to open message area");
            done = true;
            mLink.dispose_list();
            same = false;
            break;
        }*/

        // Reset Message Handler!
        if (AHandle != NULL) {
            CloseMsgArea();
        }


        // Setup Screen Display Ansi Header
        pass->session->puts("\x1b[0m\x1b[2J");
        ParseMRead("mread");

        // Draw Message Inside of Box
        mLink.box_start();

        while(pass->session->isActive() && !done && same) {

            _output = "";
            more = false;
            if (mLink.line_count() > 0) more = true;

            // Show Down Arrow More!
            if (more) sprintf(outBuffer,"\x1b[24;3H|15\x19");
            else sprintf(outBuffer,"\x1b[24;3H ");
            _output += outBuffer;

            // Show up Arrow More
            if (mLink.Page > 1) sprintf(outBuffer,"\x1b[24;10H|15\x18");
            else sprintf(outBuffer,"\x1b[24;10H ");
            _output += outBuffer;

            // Show Current/Total Pages
            sprintf(outBuffer,"\x1b[24;72H|15%i",mLink.Page);
            _output += outBuffer;
            sprintf(outBuffer,"\x1b[24;76H|15%i",mLink.TotPages);
            _output += outBuffer;

            pipe2ansi(pass,(char *)_output.c_str());

            // If more, Select Menu Prompt with PGDN as Default
            // Otherwise Select Prompt with Next as Default!
            if (more) strcpy(_mf._curmenu,"msgp2");   // Setup Message Prompt Menu
            else strcpy(_mf._curmenu,"msgp");              // Setup Message Prompt Menu

            // Readin the Menu Prompt
            _mf.menu_readin();

            // Process Messeage Promt with lightbar menu
            _mf.menu_proc(mString);

            // Process Return Input from Lightbars, Next, Prev, Quit ...
            ch = mString[0];

            switch (toupper(ch)) {

                case 'N': // Next Message
                    mLink.Lines = 0;
                    count = atoi(mHead.totmsg);
                    if (thisuser->lastmsg != count) ++thisuser->lastmsg;
                    mLink.dispose();
                    same = false;
                    break;

                case 'P': // Previous Message
                    mLink.Lines = 0;
                    if (thisuser->lastmsg != 1) --thisuser->lastmsg;
                    mLink.dispose();
                    same = false;
                    break;

                case 'Q': // Quit Message Reading
                    done = true;
                    mLink.dispose_list();
                    same = false;
                    break;

                case 'U': // Page UP
                    mLink.box_pgup();
                    break;

                case 'D': // Page Down
                    mLink.box_pgdn();
                    break;

                case 'A': // Scroll Up 1 Line
                    mLink.box_pgup();
                    break;

                case 'B': // Scroll down 1 Line
                    mLink.box_pgdn();
                    break;

                case 'O': // Post a New Message
                    PostMsg(thisuser->lastmbarea, true);
                    same = false; // Refresh Message
                    break;

                default :
                    break;

            }
        }
    }
}



