/*

    Vampyric BBS (c) Mercyful Fate 2004
    Inital Ztelnet Source code (c) Zack Booth Simpson http://www.mine-control.com/zack/
    Adapted to a bbs frontend.

    Inital Message and JAM interface recoded from GryphonBBS for linux from Darryl Perry.

    01/29/04 - Init Coding
    01/30/04 - Changed 1 line input to 1 char input, changed write to printf
               for way faster display, fixed buffering to reset after 80 chars
               to avoid buffer overflows. Added Terminal Detection Sequence
               Ansi display working great.
    02/01/04 - Split each Incomming connection into it's own thread! Woot!
    02/02/04 - Split Poll into 2, 1 for incomming c, other for incomming lines buffers
             - also figued out functions for 2 key hot key combo's
             - Added Config file for BBS Configuration Settings
             - Gota Redo input key system so backspace doesn't go before init position.
               gota parse and send escape code for cursor position.. argh!!
    02/03/04 - Took all day but fixed ofstream bug, was start thread wrong passing without
               allocating new! Also don't put std::string in structs.. :)
    02/04/04 - Fixed cpu usage in menu editor thread, don't forget a Sleep(10) for each
               poll loop, this keeps the cpu useage down but doesn't slow the program down.
    02/09/04 - Finished most of the Menu Editor, Working on Lightbar Menu Parseing,
               added new pipe codes to ansi sequences functions.
               adding init ansi getx,y from user.. and finishing menu lightbars
    02/10/04 - Completed Matrix Menu and Lightbars, working really fast!
               Added Text Scrolling Menu Command, Trying to break it off into a
               Seperate thread so it doesn't affect the ligthbar speed from being
               executed from the polling loop in that.
    02/13/04 - Split Up menu functions into 3 files, Menus, Menu Editor, Menu Func
               Menus - Basic menu File i/o, Menu_Edit - Menu and Commands Editing
               Menu Proc - Menu Processing and Main System.
    02/14/04 - Working on basic logon/application, created language file and user database
               with fast index user file
    02/15/04 - Fixed Multinode Bug, had to move main class from new pointer to normal,
               Complete Menu loaded and switching to new menus, and fixed reloading of
               text scrolling routines when switch back and forth from menus.
               New User Application is 60-70% complete with language parser full working,
               added language compile to startup of the program, and fixed menu reloading
               after making changes in the menu editor.
             - Added and Compile Statically Husky SMAPI for JAM Message Base Support
               Working on the Message Implimentation
    02/17/04 - Working on Message Base Editor, Also Recoding getting input into 2
               seperate functions with it's own polling loop for more effenciency
               Did some minor code Optimizations, and Cleaned up Include Dependencies
               cut code from 670 to 418, g++ 2.95.3 also shaves a couple hundred kb in size
               g++ 2.95.3 seems to error out like crazy.. :( gcc 3.2 is working great though
    02/21/04 - Working on Message Reader,, foudn some weird bugs, move menus class over
               to typedef struct, and also make it private instead of protected.
    02/22/04 - Message Reading is Working, Not complete yet, Starting on FSE after this
               I can tweak the message reader more better
    02/23/04 - Message API and Memory Leaks all fixed.. gota working doubly link list now,
               small problem with crash while on 2 nodes going throguh some of the same same
               shit.. gota check into the threading..
    02/24/04 - Memory Leaks Fixed.. FSE is comming along Nicely, got Scrolling Text working
               Need to finish FSE though..
    03/01/04 - Fixed Extended Line Wrap in FSE, and Enter Key Line chopping if in insert mode,
             - Still small bug in line delete if delete 1st line first, ....
             - Fixed Bug, Gota mention to g00r00 to fix 25th line in Net Runner,, also
               Windows Telnet doesn't do the ESC[*;*r top and botom margin .. neither does
               normal linux telnet.. gay gay gay!  hehe mtel and netrunner both do :)
             - Made display a little faster by moving several printfs to he socket in 1 call
               by storing them all in a buffer before sending out..
    03/02/04 - Removed Box Scroll, Wasn't working in Netrunner or other clients other then
               Mtel,  fixing Box Redraw to scroll it self worked much better!
    03/03/04 - Finally rewrote and fixed backspace word wrapping.. , Now Really Fixed!,
               Added Insert Line, now to add the rest of the keys for movements.. Home, End..
    03/08/04 - Most of FSE Completed, Rewriting Message Reader to FullScreen with Scrolling
               And Link List for Moving up and down pages..
    03/12/04 - FSE is finlkly working in think.. Message Reader needs some work with the
               PageUP/DN stuff a bit more.. Working on New User Application and Login, and
               going to work in regular main and message menu's to get things going...
             - Working on Logon, also Added |XY MCI Code for screen position in Language File
    03/31/04 - Working on intergrating the FSE into the Message Reader, Normal posting added,
               now working on reply posting and quoting, also need to convert the user
               information verification into a full menu with mci code ansi parsing.. :)
               make it cooler as a lightbar menu..


               todo, add so message reader can import and dispaly any text files / ansi files


*/

#include <stdio.h>
#include <stdarg.h>
#include <process.h>

// Main Telnet Server / Sockets
#include "ztelnetserver.h"
#include "zocket.h"

// Start of Main System Includes
#include "struct.h"      // System Definitions
#include "main_system.h" // Main System Class
#include "config.h"      // BBS Configuration File Parsing
#include "ansiout.h"     // Ansi Output / MCI Code Parsing
#include "language.h"    // Compile Language file on startup

using namespace std;

HANDLE    exec_mutex;

#ifndef min
#define min(a,b) ( ( (a) < (b) ) ? (a) : (b) )
#endif
#ifndef max
#define max(a,b) ( ( (a) > (b) ) ? (a) : (b) )
#endif


#define ESC                   '\x1b'

#define SE                    "\xF0"
#define GA                    "\xF9"
#define SB                    "\xFA"
#define WILL                  "\xFB"
#define WONT                  "\xFC"
#define DO                    "\xFD"
#define DONT                  "\xFE"
#define IAC                   "\xFF"
#define CMD_SE                0xF0
#define CMD_GA                0xF9
#define CMD_SB                0xFA
#define CMD_WILL              0xFB
#define CMD_WONT              0xFC
#define CMD_DO                0xFD
#define CMD_DONT              0xFE
#define CMD_IAC               0xFF
#define ECHO                  "\x01"
#define SUPPRESS_GO_AHEAD     "\x03"
#define LINEMODE              "\x22"
#define OLD_ENVIRON           "\x24"
#define OPT_ECHO              0x01
#define OPT_SUPPRESS_GO_AHEAD 0x03
#define OPT_LINEMODE          0x22
#define OPT_OLD_ENVIRON       0x24
#define ST_DEFAULT            0     // receiving regular characters
#define ST_ESCAPE             1     // got an escape, looking for next character
#define ST_COMMAND            2     // got command, looking for command data
#define READ_BUFF_SIZE        512


char BBSPATH[255] = {0};
char DATAPATH[10] = {0};
char MENUPATH[10] = {0};
char ANSIPATH[10] = {0};
char MESGPATH[10] = {0};

// Transfer / Error Logs
void logntp ( char *text )
{

    FILE *fp1;
    if ( ( fp1 = fopen ( "Vamp-bbs.Log", "a" ) ) == NULL )
    {
        printf ( "Fatal! Couldn't write to log file!\n" );
        return;
    }
    fprintf ( fp1, "* %s\n",text );
    fclose ( fp1 );
}

// Printf out in session class
void pf ( char *text )
{

    printf ( "%s",text );
}

// Keeping this for later 2 key hot Key Combo's

/*
unsigned char c;
char rBuffer[1024]={0};
bool result;
char key2[3]={0};
int  cnt = 0;

// Incomming Socket/Message Loop
while (1) {
    // Check conenction and Poll for messages
    if( pass->session->isActive() ) {
        result = pass->session->pollc(c);

         if (result) {
            // Check for Backspace first!
            if (c == 0x08) {
                if (cnt != 0)
                    --cnt;
            }
            else if (c == '\r') {
                cnt = 0;
                memset(&key2,0,sizeof(key2));
            }
            else if (cnt == 0) {
                 key2[0] = c; cnt++;
            }
            else {
                key2[1] = c; cnt++;
            }

            if (cnt < 3) {
                if (strcmp(key2,"/t") == 0) {

                    ansiPrintf(pass, "logon");
                    memset(&key2,0,sizeof(key2));
                    cnt = 0;
                }

                else if (strcmp(key2,"/s") == 0) {
                    logntp("Entering Menu Edit");
                    //menu_edit(pass);
                    pass->session->puts("\x1b[0m");
                    memset(&key2,0,sizeof(key2));
                    cnt = 0;
                }
                else if (strcmp(key2,"/g") == 0) {
                    pass->session->printf("\r\nGoodbye.. .");
                    pass->session->stop();
                    memset(&key2,0,sizeof(key2));
                    cnt = 0;
                }
            }
        }


    }else { delete pass; _endthread(); }

    // Give small delay for for fast ansi and text display
    Sleep(10);

}*/


int handler ( char *line, class ZTelnetServerSession *session )
{

    if ( line == NULL )
    {
        printf ( "\nConnection Established.. .\n" );
    }
    else if ( line == ( char * ) 1 )
    {
        printf ( "\nLost connection\n" );
    }
    else
    {
        //printf( "Received command %s\n", line );
        printf ( "\n" );
        if ( !strcmp ( line, "quit" ) )
        {
            session->printf ( "Goodbye!\n" );
            return 0;
        }
    }
    return 1;
}


// This is the System start for each Seperate Session Thread
//--------------------------------------------------------------------

void StartSessionThread ( void *p )
{

    //PASSING *pass = (PASSING*) p;

    class main_system *ms = ( main_system* ) p;


    // Starts the Session for this Telnet Connection
    ms->pass->session->start ( ms->pass->z, ms->pass->handler );


    unsigned char c;
    while ( ms->pass->session->isActive() )
    {

        // Poll for Ansi Detection
        ms->pass->session->pollc ( c );
        if ( !ms->pass->session->detection )
        {
            // Jumps into main_system
            //system_execute(pass);
            ms->system_execute();
        }
        Sleep ( 10 );
    }


    delete ms;
    _endthread();

}


// ZTelnetServerSession
//--------------------------------------------------------------------

ZTelnetServerSession::ZTelnetServerSession()
{
    z = NULL;
    recvState = ST_DEFAULT;
    inSubOpt = 0;
    appData = 0;
    passwdMode = 0;
    passwdChar = '*';
    charMode = 0;
    addCRMode = 1;
    echoMode = 1;
}

ZTelnetServerSession::~ZTelnetServerSession()
{
    if ( isActive() )
    {
        stop();
    }
}

int ZTelnetServerSession::start ( Zocket *_z, ZTelnetServerCmdHandler *_handler )
{
    // The zocket has just been opened, so reset it
    // and perform the initial Telnet negotiation
    handler     = _handler;
    z = _z;
    inCursor    = 0;
    outCursor   = 0;
    passwdMode  = 0;
    detection   = true;
    ansigetxy   = false;
    ScrollBreak = false;

    if ( puts ( IAC WILL SUPPRESS_GO_AHEAD ) < 0 ) return -1;
    if ( puts ( IAC WILL ECHO ) < 0 ) return -1;

    // Terminal Detection
    if ( puts ( "\r\n"    // locate cursor at column 1
                "\x1b[99B"  // locate cursor as far down as possible
                "\x1b[6n"   // Get cursor position
                "\x1b[!_"   // RIP?
                "\x1b[0t_"  // WIP?
                "\x1b[0m_"  // "Normal" colors
                "\x1b[2J"   // clear screen
                "\x1b[H"    // home cursor
                "\xC"       // clear screen (in case not ANSI)
                "\r"        // Move cursor left (in case previous char printed)
              )  < 0 ) return -1;

    Sleep ( 20 );

    return 1;
}

int ZTelnetServerSession::stop()
{
    // Shutdown and reset.
    if ( z != NULL )
    {
        delete z;
    }
    z = NULL;

    if ( handler != NULL )
    {
        // Tell the session that it has been shutdown.
        // NOTE: This is kind of hacky. -TNB
        ( *handler ) ( ( char * ) 1, this );
    }

    return 1;
}

int ZTelnetServerSession::write ( void *buffer, int len )
{
    if ( !z ) return 0;
    int result = z->write ( buffer, len, 0 );
    if ( result < 0 )
    {
        stop();
        return -1;
    }
    return result;

    /*
    // ZBS replaced with immediate write 26 Apr 2001
    #define OUT_SIZE (4800)
    char *buf = (char *)buffer;
    if( outCursor + len > OUT_SIZE ) {
        return 0;
    }

    int outBytes = 0;
    char *d = outBuffer + outCursor;
    for( int i=0; i<len; i++ ) {
        if( addCRMode && buf[i]=='\n' ) {
            *d++ = '\r';
            outBytes++;
        }
        *d++ = buf[i];
        outBytes++;
        if( outCursor + outBytes >= OUT_SIZE ) {
            return 0;
        }
    }

    outCursor += outBytes;
    return outBytes;
    */
}

int ZTelnetServerSession::puts ( char *s )
{
    int result = 0;
    while ( isActive() )
    {
        result = write ( s, strlen ( s ) );
        if ( result > 0 ) break;
        Sleep ( 10 );
    }
    return ( result );
}

int ZTelnetServerSession::putC ( char c )
{
    int result = 0;
    while ( isActive() )
    {
        result = result = write ( &c, 1 );
        if ( result > 0 ) break;
        Sleep ( 10 );
    }
    return ( result );
}

int ZTelnetServerSession::printf ( char *fmt, ... )
{
    char buffer[1024];
    va_list argptr;
    if ( fmt != buffer )
    {
        va_start ( argptr, fmt );
        vsprintf ( buffer, fmt, argptr );
        va_end ( argptr );
    }
    buffer[1023] = '\0';
    return puts ( buffer );
}

int ZTelnetServerSession::flush()
{

    // Write all the available output to the zocket, blocking.
    while ( outCursor > 0 )
    {
        int result = z->write ( outBuffer, outCursor, 0 );
        if ( result < outCursor )
        {
            stop();
            return 0;
        }
        else
        {
            memmove ( outBuffer, outBuffer+result, outCursor-result );
            outCursor -= result;
        }
    }
    return 1;
}


// Reads and Returns a single char! - Returns false with no data and true with data
// Returns buffer after \r is received <ENTER>

bool ZTelnetServerSession::polls ( char *rBuffer )
{

    struct sockaddr_in Their_addr;
    memset ( &Their_addr,0,sizeof ( Their_addr ) );

    int sin_size;
    sin_size = sizeof ( struct sockaddr_in );

    // Attempt to read some data from the zocket, without blocking.
    char readBuffer[READ_BUFF_SIZE];
    int len = z->read ( readBuffer, READ_BUFF_SIZE, 0 );
    if ( len == -1 )
    {
        stop();
        return 0;
    }

    // If any data was read, scan it for escape codes.
    int index = 0;
    int count = len;
    char tmpBuf[20]; // Hold Escape Sequences

    while ( count-- && inCursor < zTelnetServer_IN_SIZE-1 )
    {
        unsigned char c = readBuffer[index];

        // Handle either as a command or a raw characters
        switch ( recvState )
        {
            case ST_DEFAULT:
                if ( c == CMD_IAC )
                {
                    // Escaped character, change state
                    recvState = ST_ESCAPE;
                }
                else
                {
                    if ( inSubOpt )
                    {
                        // Add the character to the subOpt buffer
                    }
                    else
                    {
                        if ( charMode )
                        {
                            if ( handler != NULL )
                            {
                                if ( echoMode ) printf ( "%c", c );
                                //if( echoMode ) write( &c, 1 );
                                char buf[2];
                                buf[0] = c;
                                buf[1] = 0;
                                if ( ! ( *handler ) ( buf, this ) )
                                {
                                    stop();
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            // Catch all Input Escape Key Sequences from user's terminal
                            if ( c == ESC )
                            {
                                // Handle Inital Terminal Detection / Login to BBS
                                if ( detection )
                                {
                                    bool rec = false;
                                    for ( int i = 0;; i++ )
                                    {
                                        if ( readBuffer[index] == 'R' )
                                        {
                                            printf ( "\r\n%s.. .\r\n\r\n", VERSION );
                                            printf ( "Emulation Detected! Ansi Defaulted.. .\r\n" );
                                            pf ( "Emulation Detected! Ansi Defaulted.. .\r\n" );
                                            getpeername ( z->sockFD, ( struct sockaddr * ) &Their_addr, &sin_size );
                                            printf ( "Peer Address: %s\r\n", inet_ntoa ( Their_addr.sin_addr ) );
                                            //pf("Peer Address: " << inet_ntoa(Their_addr.sin_addr) << endl;
                                            puts ( "\r\n\r\n" );
                                            Sleep ( 1000 );
                                            rec = true;
                                            break;
                                        }
                                        if ( readBuffer[index] == '\0' )
                                        {
                                            len = z->read ( readBuffer, READ_BUFF_SIZE, 0 );
                                            if ( len == -1 )
                                            {
                                                printf ( "\r\n%s.. .\r\n\r\n", VERSION );
                                                printf ( "Emulation Not Detected! Ansi Defaulted.. .\r\n" );
                                                pf ( "Emulation Not Detected! Ansi Defaulted.. .\r\n" );
                                                getpeername ( z->sockFD, ( struct sockaddr * ) &Their_addr, &sin_size );
                                                printf ( "Peer Address: %s\r\n", inet_ntoa ( Their_addr.sin_addr ) );
                                                //cout << "Peer Address: " << inet_ntoa(Their_addr.sin_addr) << endl;
                                                puts ( "\r\n\r\n" );
                                                Sleep ( 1000 );
                                                break;
                                            }
                                        }
                                        tmpBuf[i] = readBuffer[index];
                                        ++index;
                                        if ( readBuffer[index] == ESC )
                                        {
                                            --index;
                                            break;
                                        }
                                    }
                                    memset ( &readBuffer,0,READ_BUFF_SIZE );
                                    detection = false;
                                    return false;
                                }
                                // Get ansi XY Position / Temp only need responce for Ansi display
                                else if ( ansigetxy )
                                {
                                    bool rec = false;
                                    for ( int i = 0;; i++ )
                                    {
                                        if ( readBuffer[index] == 'R' )
                                        {
                                            rec = true;
                                            break;
                                        }
                                        if ( readBuffer[index] == '\0' )
                                        {
                                            len = z->read ( readBuffer, READ_BUFF_SIZE, 0 );
                                            if ( len == -1 )
                                            {
                                                break;
                                            }
                                        }
                                        tmpBuf[i] = readBuffer[index];
                                        ++index;
                                        if ( readBuffer[index] == ESC )
                                        {
                                            --index;
                                            break;
                                        }
                                    }
                                    memset ( &readBuffer,0,READ_BUFF_SIZE );
                                    ansigetxy = false;
                                    return false;
                                }
                                else
                                {
                                    // Capture Arror Key ESC Codes
                                    // If ESC KEy found Capute to Global ESC Bufer so
                                    // Function can pull it in and test it..

                                    strcpy ( EscapeKey,"" );
                                    EscapeKey[0] = readBuffer[index];    // char 27
                                    EscapeKey[1] = readBuffer[++index];  // char [
                                    EscapeKey[2] = readBuffer[++index];  // char Key
                                    //cout << EscapeKey;
                                    memset ( &readBuffer,0,READ_BUFF_SIZE );
                                    return true;
                                }
                            }

                            // If the character is a \r, send a newline as well, then terminate the command buffer and send
                            // the line to the command processor.

                            else if ( c == '\r' )
                            {
                                // All carriage returns get converted to CRLF (on echo only)

                                //if( echoMode ) printf("\r\n");
                                //cout << endl;
                                inBuffer[inCursor] = '\0';

                                /*
                                if( handler != NULL ) {
                                    if( !(*handler)(inBuffer, this) ) {
                                        stop();
                                        return 0;
                                    }
                                }*/

                                inCursor = 0;
                                strcpy ( rBuffer,inBuffer );
                                return true;
                            }
                            else if ( c == 0x08 || int ( c ) == 127 ) // 127 Linux Backspace
                            {
                                // Destructively backspace.
                                if ( inCursor > 0 )
                                {
                                    --inCursor;
                                    if ( echoMode )
                                    {
                                        printf ( "%c %c", 0x08,0x08 );
                                    }
                                }
                                // Standard C++ Console i/o
                                pf ( "\b \b" );

                            }
                            else if ( c == '\n' || c == '\0' )
                            {
                                // HACK: Swallow newlines in the input,
                                // so that Microsoft's POS telnet client will look right.
                                // For some reason, I was getting '\0' characters
                                // being sent down the line. Swallow them too.
                                return false;
                            }
                            else
                            {
                                // This character is to be kept, so add it to the inBuffer

                                // User C++ Iostream for Consle I/O
                                pf ( ( char * ) &c );

                                inBuffer[inCursor] = c;
                                ++inCursor;

                                // Echo this character.
                                if ( passwdMode )
                                {
                                    if ( echoMode ) printf ( "%c", passwdChar );
                                }
                                else
                                {
                                    if ( echoMode ) printf ( "%c", c );
                                }

                                // Reset Buffer at 1 line of data received 80 chars
                                if ( inCursor > READ_BUFF_SIZE-1 )
                                {
                                    inBuffer[inCursor] = '\0';
                                    inCursor = 0;
                                    return false;
                                }
                            }
                        }
                    }
                }
                break;

            case ST_ESCAPE:
                if ( c == CMD_IAC )
                {
                    // This was not a command, but simply an escaped
                    // CMD_IAC character. Store a CMD_IAC in the in buffer and
                    // return to the default state.
                    if ( inSubOpt )
                    {
                        // Add the character to the subOpt buffer
                    }
                    else
                    {
                        inBuffer[inCursor++] = c;
                    }
                    recvState = ST_DEFAULT;
                }
                else
                {
                    // This is a command sequence, store it and change the state.
                    cmdId = c;
                    recvState = ST_COMMAND;
                }
                break;

            case ST_COMMAND:
                {
                    // The last part of a command sequence
                    switch ( c )
                    {
                        case CMD_WILL:
                        case CMD_WONT:
                        case CMD_DO:
                        case CMD_DONT:
                            // A real telnet server would send the correct response here.
                            // We, however, choose to send nothing for now.
                            break;

                        case CMD_SB:
                            // Set sub opt mode
                            inSubOpt = 1;
                            break;

                        case CMD_SE:
                            // Swallow the next character, and reset sub opt mode
                            inSubOpt = 0;
                            break;

                        default:
                            // Fall through
                            break;
                    }
                    recvState = ST_DEFAULT;
                }
                break;
        }

        // Process the next character
        ++index;
    }

    // Write any available output without blocking.
    /* ZBS removed and converted to immediate write
    if( outCursor > 0 ) {
        int result = z->write( outBuffer, outCursor, 0 );
        if( result == -1 ) {
            stop();
            return 0;
        }
        else {
            memmove( outBuffer, outBuffer+result, outCursor-result );
            outCursor -= result;
        }
    }
    */


    return false;
}



// Reads and Returns a single char! - Returns false with no data and true with data
bool ZTelnetServerSession::pollc ( unsigned char &c )
{

    struct sockaddr_in Their_addr;
    memset ( &Their_addr,0,sizeof ( Their_addr ) );

    int sin_size;
    sin_size = sizeof ( struct sockaddr_in );


    // Attempt to read some data from the zocket, without blocking.
    char readBuffer[READ_BUFF_SIZE];
    int len = z->read ( readBuffer, READ_BUFF_SIZE, 0 );
    if ( len == -1 )
    {
        stop();
        return 0;
    }

    // If any data was read, scan it for escape codes.
    int index = 0;
    int count = len;
    char tmpBuf[20]; // Hold Escape Sequences

    while ( count-- && inCursor < zTelnetServer_IN_SIZE-1 )
    {

        //unsigned char c = readBuffer[index];
        c = readBuffer[index];

        // Handle either as a command or a raw characters
        switch ( recvState )
        {
            case ST_DEFAULT:
                if ( c == CMD_IAC )
                {
                    // Escaped character, change state
                    recvState = ST_ESCAPE;
                }
                else
                {
                    if ( inSubOpt )
                    {
                        // Add the character to the subOpt buffer
                    }
                    else
                    {
                        if ( charMode )
                        {
                            // Figure this out more!!
                            if ( handler != NULL )
                            {
                                //if( echoMode ) //printf("%c", c);
                                char buf[2];
                                buf[0] = c;
                                buf[1] = 0;
                                if ( ! ( *handler ) ( buf, this ) )
                                {
                                    stop();
                                    return false;
                                }
                            }
                        }
                        else
                        {

                            // Catch all Input Escape Key Sequences from user
                            if ( c == ESC )
                            {
                                // Handle Inital Terminal Detection / Login to BBS
                                if ( detection )
                                {
                                    bool rec = false;
                                    for ( int i = 0;; i++ )
                                    {
                                        if ( readBuffer[index] == 'R' )
                                        {
                                            printf ( "\r\n%s.. .\r\n\r\n", VERSION );
                                            printf ( "Emulation Detected! Ansi Defaulted.. .\r\n" );
                                            pf ( "Emulation Detected! Ansi Defaulted.. .\r\n" );
                                            getpeername ( z->sockFD, ( struct sockaddr * ) &Their_addr, &sin_size );
                                            printf ( "Peer Address: %s\r\n", inet_ntoa ( Their_addr.sin_addr ) );
                                            //cout << "Peer Address: " << inet_ntoa(Their_addr.sin_addr) << endl;
                                            puts ( "\r\n\r\n" );
                                            Sleep ( 1500 );
                                            rec = true;
                                            break;
                                        }
                                        if ( readBuffer[index] == '\0' )
                                        {
                                            len = z->read ( readBuffer, READ_BUFF_SIZE, 0 );
                                            if ( len == -1 )
                                            {
                                                printf ( "\r\n%s.. .\r\n\r\n", VERSION );
                                                printf ( "Emulation Not Detected! Ansi Defaulted.. .\r\n" );
                                                pf ( "Emulation Not Detected! Ansi Defaulted.. .\r\n" );
                                                getpeername ( z->sockFD, ( struct sockaddr * ) &Their_addr, &sin_size );
                                                printf ( "Peer Address: %s\r\n", inet_ntoa ( Their_addr.sin_addr ) );
                                                //cout << "Peer Address: " << inet_ntoa(Their_addr.sin_addr) << endl;
                                                puts ( "\r\n\r\n" );
                                                Sleep ( 1500 );
                                                break;
                                            }
                                        }
                                        tmpBuf[i] = readBuffer[index];
                                        ++index;
                                        if ( readBuffer[index] == ESC )
                                        {
                                            --index;
                                            break;
                                        }
                                    }
                                    memset ( &readBuffer,0,READ_BUFF_SIZE );
                                    detection = false;
                                    return false;
                                }
                                // Get ansi XY Position / Temp only need responce for Ansi display
                                else if ( ansigetxy )
                                {
                                    //cout << "\nansigetxy ";
                                    bool rec = false;
                                    for ( int i = 0;; i++ )
                                    {
                                        if ( readBuffer[index] == 'R' )
                                        {
                                            rec = true;
                                            break;
                                        }
                                        if ( readBuffer[index] == '\0' )
                                        {
                                            len = z->read ( readBuffer, READ_BUFF_SIZE, 0 );
                                            if ( len == -1 )
                                            {
                                                break;
                                            }
                                        }
                                        tmpBuf[i] = readBuffer[index];
                                        ++index;
                                        if ( readBuffer[index] == ESC )
                                        {
                                            --index;
                                            break;
                                        }
                                    }
                                    memset ( &readBuffer,0,READ_BUFF_SIZE );
                                    ansigetxy = false;
                                    return false;
                                }
                                // Other Escape Code's goes here for input!
                                else
                                {
                                    // Capture Arror Key ESC Codes
                                    // If ESC KEy found Capute to Global ESC Bufer so
                                    // Function can pull it in and test it..

                                    strcpy ( EscapeKey,"" );
                                    EscapeKey[0] = readBuffer[index];    // char 27
                                    EscapeKey[1] = readBuffer[++index];  // char [
                                    EscapeKey[2] = readBuffer[++index];  // char Key
                                    //cout << EscapeKey;
                                    memset ( &readBuffer,0,READ_BUFF_SIZE );
                                    return true;
                                }
                            }

                            // If the character is a \r, send a newline as well, then terminate the command buffer and send
                            // the line to the command processor.
                            else if ( c == '\r' )
                            {

                                //if( echoMode ) printf("\r\n");
                                // User C++ Iostream for Consle I/O
                                //cout << "\r\n";

                                inBuffer[inCursor] = '\0';

                                inCursor = 0;
                                memset ( &inBuffer,0,sizeof ( inBuffer ) );
                                return true;
                            }
                            else if ( c == 0x08 || int ( c ) == 127 ) // 127 Linux Backspace
                            {
                                // Destructively backspace.
                                if ( inCursor >= 0 )
                                {
                                    --inCursor;
                                    if ( echoMode )
                                    {
                                        //printf("%c %c", 0x08,0x08);
                                    }
                                }
                                // Standard C++ Console i/o
                                //cout << "\b \b";
                                inCursor = 0;
                                memset ( &inBuffer,0,sizeof ( inBuffer ) );
                                return true;
                            }
                            else if ( c == '\n' || c == '\0' )
                            {
                                // HACK: Swallow newlines in the input,
                                // so that Microsoft's POS telnet client will look right.
                                // For some reason, I was getting '\0' characters
                                // being sent down the line. Swallow them too.
                                return false;
                            }
                            else
                            {
                                // This character is to be kept, so add it to the inBuffer
                                inBuffer[inCursor] = c;
                                ++inCursor;

                                // Echo this character.
                                if ( passwdMode )
                                {
                                    if ( echoMode ) printf ( "%c", passwdChar );
                                }
                                else
                                {
                                    //if( echoMode ) printf("%c", c);
                                }

                                // User C++ Iostream for Consle I/O
                                //cout << c;


                                // Reset Buffer at 1 line of data received 80 chars
                                if ( inCursor > 79 || len == index )
                                {
                                    inCursor = 0;
                                }
                                inCursor = 0;
                                memset ( &inBuffer,0,sizeof ( inBuffer ) );
                                return true;
                            }
                        }
                    }
                }
                break;

            case ST_ESCAPE:
                if ( c == CMD_IAC )
                {
                    // This was not a command, but simply an escaped
                    // CMD_IAC character. Store a CMD_IAC in the in buffer and
                    // return to the default state.
                    if ( inSubOpt )
                    {
                        // Add the character to the subOpt buffer
                    }
                    else
                    {
                        inBuffer[inCursor++] = c;
                    }
                    recvState = ST_DEFAULT;
                }
                else
                {
                    // This is a command sequence, store it and change the state.
                    cmdId = c;
                    recvState = ST_COMMAND;
                }
                break;

            case ST_COMMAND:
                {
                    // The last part of a command sequence
                    switch ( c )
                    {
                        case CMD_WILL:
                        case CMD_WONT:
                        case CMD_DO:
                        case CMD_DONT:
                            // A real telnet server would send the correct response here.
                            // We, however, choose to send nothing for now.
                            break;

                        case CMD_SB:
                            // Set sub opt mode
                            inSubOpt = 1;
                            break;

                        case CMD_SE:
                            // Swallow the next character, and reset sub opt mode
                            inSubOpt = 0;
                            break;

                        default:
                            // Fall through
                            break;
                    }
                    recvState = ST_DEFAULT;
                }
                break;
        }

        // Process the next character
        ++index;
    }
    // Write any available output without blocking.
    /*  ZBS removed and converted to immediate write
    if( outCursor > 0 ) {
        int result = z->write( outBuffer, outCursor, 0 );
        if( result == -1 ) {
            stop();
            return 0;
        }
        else {
            memmove( outBuffer, outBuffer+result, outCursor-result );
            outCursor -= result;
        }
    }
    */


    return ( false );
}


// ZTelnetServer
//--------------------------------------------------------------------

ZTelnetServer::ZTelnetServer()
{
    handler = NULL;
    listening = NULL;
}

ZTelnetServer::ZTelnetServer ( int port, ZTelnetServerCmdHandler *_handler )
{
    handler = NULL;
    listening = NULL;
    start ( port );
    setCmdHandler ( _handler );
}

ZTelnetServer::~ZTelnetServer()
{
    // Stop all active sessions
    stop ();
    handler = NULL;
}

int ZTelnetServer::start ( int port )
{
    assert ( listening == NULL );

    char zAddress[100];
    sprintf ( zAddress, "tcp(lbr)://*:%d", port );
    listening = new Zocket ( zAddress );

    if ( !listening->isOpen() )
    {
        return 0;
    }
    return 1;
}

int ZTelnetServer::stop()
{
    if ( !listening )
    {
        return 1;
    }

    if ( listening->isOpen() )
    {
        for ( int i=0; i<zTelnetServer_MAX_SESSIONS; ++i )
        {
            if ( session[i].isActive() )
            {
                session[i].stop();

            }
        }
    }
    delete listening;
    listening = NULL;
    return 1;
}

void ZTelnetServer::setCmdHandler ( ZTelnetServerCmdHandler *_handler )
{
    handler = _handler;
    for ( int i = 0; i < zTelnetServer_MAX_SESSIONS; ++i )
    {
        if ( session[i].isActive() )
        {
            session[i].setCmdHandler ( _handler );
        }
    }
}

// Main Processing Routiene
//--------------------------------------------------------------------

int ZTelnetServer::poll()
{

    int i;

    // Handle to Threads
    HANDLE ahThread[zTelnetServer_MAX_SESSIONS];

    if ( listening->isOpen() )
    {
        // Check for a new connection

        int sockFD = listening->accept ( 0 );
        if ( sockFD > 0 )
        {

            Zocket  *z = new Zocket ( listening );

            // Find an inactive session and initialize it with this zocket.
            // If there is no inactive session, close the zocket.

            for ( i = 0; i < zTelnetServer_MAX_SESSIONS; ++i )
            {
                if ( !session[i].isActive() )
                {

                    // Fill Structure for Passing to new Thread

                    class main_system *ms = new main_system;

                    //PASSING *pass = new PASSING;

                    ms->pass->handler = ( ZTelnetServerCmdHandler * ) handler; // Command Handler
                    ms->pass->session = &session[i];                           // Session Handler
                    ms->pass->z = z;                                           // Socket  Handler

                    ahThread[i] = ( HANDLE ) _beginthread ( StartSessionThread, 0, ms );

                    /*
                    This is Where each new Telnet Session is started on Connection,
                    Here will start each new Telnet Session Thread, which will also
                    Each have it's own polling seperate to keep thing seperate and fast!

                    // This is Where Server Session is Started!!!  Need to Transfer to Menu / Startup System
                    if( session[i].start(z, handler) ) {
                        // Call the handler function with a NULL line, indicating that the connection was just established.
                        if( handler != NULL ) {
                            if( !(*handler)(NULL, &session[i]) ) {
                                session[i].stop();
                            }
                        }
                    } */
                    break;
                }
            }

            if ( i >= zTelnetServer_MAX_SESSIONS )
            {
                delete z;
            }
        }
    }

    /*
    // Don't poll each session ..
    // Once a session is started, it will poll it self in it's own thread!
    // So each Thread can have it's own system!

    // Poll all of the active sessions.
    for( i = 0; i < zTelnetServer_MAX_SESSIONS; ++i ) {
        if( session[i].isActive() ) {
            session[i].poll();
        }
    }
    */

    return 1;
}

int ZTelnetServer::fillOutSet ( void *readSet, void *writeSet, void *exceptSet )
{
    // Fill out the set for each catagory from the listening zocket and each active zocket.
    int largest = 0;
    if ( listening->isOpen() )
    {
        if ( readSet != NULL )
        {
            zocketFdSet ( listening->getSockFD(), readSet );
            largest = max ( largest, listening->getSockFD() );
        }

        if ( writeSet != NULL )
        {
            zocketFdSet ( listening->getSockFD(), writeSet );
            largest = max ( largest, listening->getSockFD() );
        }

        if ( exceptSet != NULL )
        {
            zocketFdSet ( listening->getSockFD(), exceptSet );
            largest = max ( largest, listening->getSockFD() );
        }

        for ( int i = 0; i < zTelnetServer_MAX_SESSIONS; ++i )
        {
            if ( session[i].isActive() )
            {
                if ( readSet != NULL )
                {
                    zocketFdSet ( session[i].getSockFD(), readSet );
                    largest = max ( largest, session[i].getSockFD() );
                }

                if ( writeSet != NULL )
                {
                    zocketFdSet ( session[i].getSockFD(), writeSet );
                    largest = max ( largest, session[i].getSockFD() );
                }

                if ( exceptSet != NULL )
                {
                    zocketFdSet ( session[i].getSockFD(), exceptSet );
                    largest = max ( largest, session[i].getSockFD() );
                }
            }
        }
    }
    return largest;
}

int main ( int argc, char **argv )
{

#ifdef _WIN32
    SetConsoleTitle ( VERSION );
#endif


    // Setup Global Path in Structs.h
    char parg[255];
    strcpy ( parg,argv[0] );
    int num = 0;

    // Get FULL PATH TO EXE, and Chop off Filename for PATH
    for ( int i = 0; ; i++ )
    {
        if ( parg[i] == '\0' ) break;
        if ( parg[i] == '\\' ) num = i;
    }
    if ( num != 0 )
    {
        for ( int i = 0; i < num+1; i++ )
        {
            BBSPATH[i] = parg[i];
        }
        SetCurrentDirectory ( BBSPATH );
    }
    else
    {
        strcpy ( BBSPATH, "" );
    }
    strcpy ( DATAPATH,"data\\" );
    strcpy ( MENUPATH,"menu\\" );
    strcpy ( ANSIPATH,"ansi\\" );
    strcpy ( MESGPATH,"msgs\\" );

    // Do Error Checking if CONFIG.CFG File exists, if not creates it
    if ( configdataexists() == false ) createconfig(); // Creates Config with Default Settings

    // Open and Read Config file
    parseconfig();

    // Compile Language File on Startup
    language *lang = new language;
    printf ( "\nCompiling Language File" );
    lang->lang_compile();
    printf ( "\nLanguage File Completed!\n" );
    delete lang;

    ZTelnetServer rCmdServer ( 2000, handler );
    printf ( "\n%s \nInitalized on port 2000! ", VERSION );

    // The server runs a non-blocking interface that requires
    // an occasional call to poll.  You can either call this poll
    // once per frame in a main-loop like this:
    //
    // while( 1 ) {
    //     rCmdServer.poll();
    // }
    //
    // Or you can fill out a selection set and go to sleep, like this
    // which is the OS friendly way to do it.


    while ( 1 )
    {

        fd_set readSet;
        fd_set exceptSet;

        FD_ZERO ( &readSet );
        FD_ZERO ( &exceptSet );

        int largest = rCmdServer.fillOutSet ( &readSet, NULL, &exceptSet );
        int _select = select ( largest+1, &readSet, NULL, &exceptSet, NULL );
        // This select will block until one of the file handles has
        // data or exception pending.

        rCmdServer.poll();
    }
    return 0;
}
