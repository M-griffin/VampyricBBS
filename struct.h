#ifndef STRUCT_H
#define STRUCT_H

#include <windows.h>
#include "ztelnetserver.h"

#define VERSION               "Vampyric BBS v1.0 Pre-Alpha 1 (c) Mercyful Fate 2004"
#define MAX_MENU_COMMANDS     100



    /* POIX semaphores */
	typedef HANDLE sem_t;
	#define sem_init(psem,ps,v)			ResetEvent(*(psem))
	#define sem_wait(psem)				WaitForSingleObject(*(psem),INFINITE)
	#define sem_post(psem)				SetEvent(*(psem))
	#define sem_destroy(psem)			CloseHandle(*(psem))
	int		sem_getvalue(sem_t*, int* val);

	/* POIX mutexes */
	typedef HANDLE pthread_mutex_t;
	#define PTHREAD_MUTEX_INITIALIZER	CreateMutex(NULL,FALSE,NULL)
	#define pthread_mutex_init(pmtx,v)	*(pmtx)=CreateMutex(NULL,FALSE,NULL)
	#define pthread_mutex_lock(pmtx)	WaitForSingleObject(*(pmtx),INFINITE)
	#define pthread_mutex_unlock(pmtx)	ReleaseMutex(*(pmtx))
	#define	pthread_mutex_destroy(pmtx)	CloseHandle(*(pmtx))


// This holds the info for each Telnet Session
// For passing Between Classes
//--------------------------------------------------------------------


extern char BBSPATH[255];
extern char DATAPATH[10];
extern char MENUPATH[10];
extern char ANSIPATH[10];
extern char MESGPATH[10];

#define BitSet(arg,posn) ((arg) | (1L << (posn)))
#define BitClr(arg,posn) ((arg) & ~(1L << (posn)))
#define BitTst(arg,posn) BOOL((arg) & (1L << (posn)))
#define BitFlp(arg,posn) ((arg) ^ (1L << (posn)))


typedef struct __attribute__((packed)) LangREc {

    char Lang[200];    

}LangRec;

typedef struct __attribute__((packed)) UserIdx {

    char Handle[30];
    long int Number; 

}UserIdx;



typedef struct __attribute__((packed)) MenuFlags {

    bool ClrScrBefore;  // clear screen before menu display
    bool NoMenuPrompt;  // no menu prompt whatsoever?
    bool ForcePause;    // force a pause before menu display?
    bool ClrScrAfter;   // clear screen after command received
    bool UseGlobal;     // use global menu commands?

}MenuFlags;

typedef struct __attribute__((packed)) MenuRec {

    char MenuName[20];        // menu name
    char Directive[20];       // normal menu text file
    char MenuPrompt[200];     // menu prompt
    char Acs[10];             // access requirements
    char Password[20];        // password required

    short ForceInput;         // 0 user defaults, 1 Hotkeys 1 ENTER
    short ForceHelpLevel;     // 0 user defaults, 1 force display, 0 force expert
    bool  Lightbar;           // Lightbar Menu?
    MenuFlags MFlags;         // menu status variables }

}MenuRec;

typedef struct __attribute__((packed)) CommandRec {

    char  LDesc[80];          // command description
    char  SDesc[40];          // command string
    char  CKeys[10];          // command execution keys
    char  Acs[10];            // access requirements
    char  CmdKeys[3];         // command keys
    char  MString[200];       // command data
    bool  SText;              // Is this a Scroll Test Comamnd
    short STLen;              // Length of Scroll Text
    bool  LBarCmd;            // Is This a Lighbar Cmd
    char  HiString[40];       // Highlighed
    char  LoString[40];       // Unhighlighted
    short Xcoord;             // Lightbar X coord
    short Ycoord;             // Lightbar Y coord
    

}CommandRec;

// Struct for passing Session Handle Between Classes / Functions
typedef struct PASSING {
    class ZTelnetServerSession  *session;
    ZTelnetServerCmdHandler     *handler;
    Zocket                      *z;
    
    // This is For passing Text Scroll to new Thread
    CommandRec                  *cmdr2;
    
    pthread_mutex_t	            thread_mutex;
    bool                        mutex_initalized;

    
}PASSING;

// Message Base Structes
typedef struct __attribute__((packed)) ACSrecL {

	unsigned long onn,off;
	
}ACSrecL;

typedef struct __attribute__((packed)) ACSrecS {

	unsigned short onn,off;
	
}ACSrecS;

typedef struct __attribute__((packed)) ACSrec {

	ACSrecL mgroup,fgroup;
	ACSrecS eflags,fflags;
	unsigned long since_mn;
	unsigned int dotm, dotw, min_logons, min_nodes, tI01, tI02, tI03;
	unsigned char min_sl, gender, min_age, tC01, tC02, tC03, tC04;
	
}ACSrec;

typedef struct __attribute__((packed)) UserRec {

    char  Handle[30];
    long  Number;
    char  Name[30];
    char  Password[30];
    char  Sex;
    char  Email[40];
    bool  EmailPrivate;
    char  UserNote[40];    
    short SL;    
    long  lastmbarea;
    long  lastmsg;
    short linelen;
    short pagelen;
    char  Word[40];
    char  BDay[9];
    ACSrec ACS;

}UserRec;

typedef struct __attribute__((packed)) fidoaddr {

	unsigned short zone;
	unsigned short net;
	unsigned short node;
	unsigned short point;
	char	 domain[13];
	
}fidoaddr;

typedef struct __attribute__((packed)) mbareaflags {

	char mbrealname;
	char mbvisible;
	char mbansi;
	char mb8bitx;
	char mbstrip;
	char mbaddtear;
	char mbnopubstat;
	char mbnocreadit;
	char mbinternet;
	char mbfileattach;
	char mbstripcolor;
 	
}mbareaflags;

typedef struct __attribute__((packed)) mb_list_rec {

	int idx;
	int Type;
	int Kind;
	int Active;
	int Pubpriv;
	int Allowalias;
	int scantype;
	int fidoaddr;
	int sigtype;
	int reserved_int[15];
	char mbdisplay[61];
	char mbfile[36];
	char mbpath[81];
	char password[16];
	ACSrec ReadACS;
	ACSrec WriteACS;
	ACSrec SysopACS;
	ACSrec NetworkACS;
	char OriginLine[66];
	unsigned char colors[4];
	char infofile[36];
	char reserved_char[149];
	unsigned long MaxMsgs;
	unsigned long MaxAge;
	unsigned long first;
	unsigned long last;
	unsigned long current;
	unsigned long lastread;
	unsigned long status;
	long reserved_long[3];
	mbareaflags flags;
	struct fidoaddr aka;
	
}mb_list_rec;




#endif

