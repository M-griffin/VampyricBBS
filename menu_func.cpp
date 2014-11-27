
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>
#include <string>

using namespace std;

#include "struct.h"

#include "menu_func.h"
#include "menu_edit.h"
#include "msg_edit.h"
#include "msg_read.h"
#include "msg_fse.h"

#include "ansiout.h"
#include "logon.h"
#include "misc.h"



// Class Constructor
//----------------------------------------------------------------------------

menu_func::menu_func()
{

    _loadnew = false;
    strcpy ( _curmenu,"" );
    strcpy ( _premenu,"" );

}

// Start BBS Menu System
//----------------------------------------------------------------------------
void menu_func::start ( PASSING *passing, UserRec *user )
{

    pass = passing;
    usr  = user;
}

void menu_func::menu_readin()
{

    printf ( "\nReadin() Menu: %s",_curmenu );
    memset ( &menur2,0,sizeof ( MenuRec ) );

    // Read in Menu
    int idx;
    idx = _mnu.menu_find ( _curmenu );
    _mnu.menu_read ( &menur2,idx );

    //Count Commands
    int nogc;
    noc = _mnu.cmds_count ( _curmenu );

    // If Global Menu True, Then allocate space for these commands as well
    if ( menur2.MFlags.UseGlobal )
    {
        nogc = _mnu.cmds_count ( "global" );
    }
    else nogc = 0;
    noc += nogc;

    // Allocate Comands
    memset ( &cmdr2,0,sizeof ( CommandRec[MAX_MENU_COMMANDS] ) );

    // Load Commands
    idx = 0;
    while ( _mnu.cmds_read ( _curmenu,&cmdr2[noc++],idx++ ) );

    // Now Load Global Commands, if not 0
    if ( menur2.MFlags.UseGlobal && nogc != 0 )
    {
        idx = 0;
        while ( _mnu.cmds_read ( "global",&cmdr2[noc++],idx++ ) );
    }

    // Reset Load New Menu Flag
    _loadnew = false;
}



void ScrollingText ( void *p )
{

    PASSING *pass = ( PASSING* ) p;

    short xpos = 0;
    short ypos = 0;
    short boxlen;
    char  sidecolor[04];
    char  midcolor[04];
    short loop;
    std::string clear;
    std::string quotestr;
    std::string q2;
    short loop2;

    char  outBuff[100];
    std::string output;


    // Test for End of Thread
    if ( pass->session->ScrollBreak )
    {
        _endthread();
        return;
    }

    // Setup The Scrolling Text
    loop = 0;
    quotestr = "";
    output = "";
    q2 = "";

    boxlen = pass->cmdr2->STLen;
    if ( boxlen-1 > strlen ( pass->cmdr2->MString ) )
    {
        for ( int i = 0; i != boxlen; i++ ) quotestr += " ";
    }

    xpos = pass->cmdr2->Xcoord;
    ypos = pass->cmdr2->Ycoord;
    strcpy ( midcolor,  pass->cmdr2->HiString );
    strcpy ( sidecolor, pass->cmdr2->LoString );
    quotestr += pass->cmdr2->MString;
    loop2 = quotestr.size();
    quotestr += quotestr.substr ( 0, boxlen-1 );


    // Loop Scrolling Text Here
    while ( pass->session->isActive() )
    {

        // Test for End of Thread
        if ( pass->session->ScrollBreak )
        {
            break;
        }

        // Now Write Scrolling Menu Command Text
        output = "";
        sprintf ( outBuff, "\x1b[%i;%iH",ypos,xpos );
        output += outBuff;
        if ( loop > loop2 ) loop = 0;
        output += sidecolor;
        q2 = quotestr.substr ( loop, 2 );
        output += q2;
        output += midcolor;
        q2 = quotestr.substr ( loop + 2, ( boxlen - 4 ) );
        output += q2;
        output += sidecolor;
        q2 = quotestr.substr ( loop + ( boxlen - 2 ), 2 );
        output += q2;
        ++loop;

        pipe2ansi ( pass, ( char* ) output.c_str() );

        // Give small delay to Slow Text Scrolling Down
        Sleep ( 100 );

    }
    _endthread();
}


// Later Change this to return Command # on Enter or Hotkey
void menu_func::menu_bars ( char *inPut )
{

    // Menu Lightbar Variables
    short xx      = 1;        // Holds X Coord
    short yy      = 1;        // Holds Y Coord

    short iNoc    = 0;        // Holds Lightbar # of choices
    short sNoc    = 0;        // Holds Scroll Text # of choices
    short choice  = 0;        // Holds Currect Lightbar #
    short execnum[200]= {0};  // Holds commands which are Lightbars
    short execnum2[10]= {0};  // Holds commands which are Scroll Text

    unsigned char c,cc;       // Hold Input / Lightbar Key

    short result  = 0;        // Result of Key Poll
    bool  EscHit = false;     // Is Input key Escaped char, or Normal Key
    char  outBuff[100];       // Holds Formatted Lightbar Data

    std::string output;       // Buffer for writing all lightbars at the same time
    short sRand = 0;          // Randmise Text Scrollies

    // Display Menu Ansi for Lightber Menu, Except Message and File Lightbar Prompts
    ansiPrintf ( pass, menur2.Directive );
    execnum2[0] = '\0';

    HANDLE ahThread;

    while ( pass->session->isActive() )
    {

        // Count Cmds with lightbar flag only
        for ( int i = 0; i != noc; i++ )
        {
            // Get Ligthbar Commands
            if ( cmdr2[i].LBarCmd )
            {
                execnum[iNoc] = i;
                ++iNoc;
            }
            // Get Text Scrolling Commands
            else if ( cmdr2[i].SText )
            {
                execnum2[sNoc] = i;
                ++sNoc;
            }
        }

        // If no Menu Commands Return!
        if ( iNoc == 0 ) return;


        // Make sure there is more then 1 to Randomize
        if ( sNoc > 0 )
        {
            // Randomize Scroll Text
            srand ( time ( NULL ) );
            sRand = rand() %sNoc;
        }

        // Start Text Scrolling Thread here...
        if ( execnum2[0] != '\0' )
        {
            pass->cmdr2 = &cmdr2[execnum2[sRand]];
            pass->session->ScrollBreak = false;
            ahThread = ( HANDLE ) _beginthread ( ScrollingText, 0, pass );
            printf ( "\nStarting Scrolling Text Thread" );
        }


        // Setup of first Command, Highlited
        output = "";
        xx = cmdr2[execnum[0]].Xcoord;
        yy = cmdr2[execnum[0]].Ycoord;
        sprintf ( outBuff,"\x1b[%i;%iH%s",yy,xx,cmdr2[execnum[0]].HiString );
        output += outBuff;

        // Setup of Remaining Lightbars in Low Form
        for ( int rep = 1; rep != iNoc; rep++ )
        {
            xx = cmdr2[execnum[rep]].Xcoord;
            yy = cmdr2[execnum[rep]].Ycoord;
            sprintf ( outBuff,"\x1b[%i;%iH%s",yy,xx,cmdr2[execnum[rep]].LoString );
            output += outBuff;
        }

        // Write out all the Lightbars
        output += "\x1b[25;80H";
        pipe2ansi ( pass, ( char* ) output.c_str() );

        // Handle Lightbar Movement, if Enter, return with cmdkey, data string
        while ( pass->session->isActive() )
        {
            result = false; // Reset to False
            while ( pass->session->isActive() )
            {

                // Get Lightbar Input
                result = pass->session->pollc ( c );

                // Check here if Arrow or Escaped Input was Received
                if ( ( int ) c == 27 )
                {
                    c = pass->session->EscapeKey[2];
                    EscHit = true;
                }
                else EscHit = false;

                // Input Received Break Loop and Redisplay Lightbars
                if ( result ) break;
                Sleep ( 10 );

            }

            output = "";
            if ( EscHit ) // Input Key is Escaped Meaning Arrow Keys
            {

                // Skip Moving Lightbars with Up/Dn Keys if were in Message Prompt
                if ( ( strcmp ( _curmenu,"msgp" ) != 0 ) && ( strcmp ( _curmenu,"msgp2" ) != 0 ) )
                {
                    if ( c == 'A' ) c = 'D';
                    else if ( c == 'B' ) c = 'C';
                }

                if ( c == 'D' )   // Up & Left
                {
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf ( outBuff,"\x1b[%i;%iH%s",yy,xx,cmdr2[execnum[choice]].LoString );
                    output += outBuff;

                    if ( choice == 0 ) choice = iNoc-1;
                    else --choice;

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf ( outBuff,"\x1b[%i;%iH%s\x1b[25;80H",yy,xx,cmdr2[execnum[choice]].HiString );
                    output += outBuff;
                    // Send Lightbar output
                    pipe2ansi ( pass, ( char* ) output.c_str() );
                }
                else if ( c == 'C' ) // Down & Right
                {
                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf ( outBuff,"\x1b[%i;%iH%s",yy,xx,cmdr2[execnum[choice]].LoString );
                    output += outBuff;

                    if ( choice == iNoc-1 ) choice = 0;
                    else ++choice;

                    xx = cmdr2[execnum[choice]].Xcoord;
                    yy = cmdr2[execnum[choice]].Ycoord;
                    sprintf ( outBuff,"\x1b[%i;%iH%s\x1b[25;80H",yy,xx,cmdr2[execnum[choice]].HiString );
                    output += outBuff;
                    // Send Lightbar output
                    pipe2ansi ( pass, ( char* ) output.c_str() );
                }
                else
                {

                    // Pass through Key and Return it
                    inPut[0] = c;
                    return;

                }

            }
            // Normal Key Input
            else
            {
                // If Enter, Return Cmd # of Lightbar Executed
                if ( c == '\r' )
                {
                    pass->session->ScrollBreak = true;
                    // Here Loop through and execute stacked Commands
                    for ( int ckey = 0; ckey != noc; ckey++ )
                    {
                        if ( strcmp ( cmdr2[ckey].CKeys, cmdr2[execnum[choice]].CKeys ) == 0 )
                        {
                            menu_docmd ( cmdr2[ckey].CmdKeys,cmdr2[ckey].MString );
                            strcpy ( inPut, ( const char* ) cmdr2[ckey].CKeys );
                        }
                    }
                    return;
                }
                // Later Check here for Normal Menu Commands Hotkey!


            }
        }
    }
}


void menu_func::menu_proc ( char *mString )
{


    // Read through and do and Commands with FIRSTCMD/EVERYTIME
    strcpy ( mString,"" );

    // Process Commands as lightbars
    if ( menur2.Lightbar )
    {
        if ( noc != 0 ) menu_bars ( mString );
    }

    // Do normal Menu Processing Here
}

void menu_func::logon_system ( unsigned char c )
{

    /*
    class logon *lgn = new logon;

    // Pass Socket and User Rec to Logon
    lgn->start(pass,usr);

    switch (c) {
        case 'L' :
            lgn->login();
            break;

        case 'A' :
            lgn->application();
            break;

        default  :
            break;
    }

    delete lgn;*/

    logon _lgn;

    // Pass Socket and User Rec to Logon
    _lgn.start ( pass,usr );

    switch ( c )
    {
        case 'L' :
            _lgn.login();
            break;

        case 'A' :
            _lgn.application();
            break;

        default  :
            break;
    }

}

void menu_func::msgedit_system()
{

    /*
    msg_edit *_medit = new msg_edit;
    // Startup Msg Base Editor
    _medit->start(pass);
    _medit->mbeditmenu();
    delete _medit; */

    msg_edit _medit;
    // Startup Msg Base Editor
    _medit.start ( pass );
    _medit.mbeditmenu();
    //delete _medit;

}

void menu_func::msgread_system ( unsigned char c )
{


    printf ( "\nMessage Reader Init" );

    // Handle to Message Reader Class
    class msg_read  *_mread = new msg_read;

    // Handle to it's own Menu Function class for Lightbar Prompt
    // class menu_func *_mf    = new menu_func;

    // Temp User Hard Code for Testing
    usr = new UserRec;
    usr->lastmbarea = 0;
    usr->lastmsg    = 1;
    usr->Number     = 0;
    usr->linelen    = 80;
    usr->pagelen    = 25;
    strcpy ( usr->Handle, "mercyful fate" );

    //strcpy(_mf->_curmenu,"msgp");   // Setup Message Prompt Menu
    // Initalize Menu Function Class
    //_mf->start(pass,usr);
    //_mf->menu_readin();

    char mString[10] = {0};
    unsigned char ch;
    bool done  = false;
    int result = 0;

    // Startup Message Read Class
    _mread->start ( pass,usr );
    _mread->start_reading();

    /*

    // Run through Mesage Look untill exit from reader prompt
    while (pass->session->isActive() && !done) {



        // Read in Mesage
        //if (!_mread->ReadMessages(usr->lastmbarea)) break;
        // Process Messeage Promt with lightbar menu
        //_mf->menu_proc(mString);


        // Process Return Input from Lightbars, Next, Prev, Quit ...
        ch = mString[0];
        switch (toupper(ch)) {

            case 'N': // Next Message
                usr->lastmsg++;
                break;
            case 'P': // Previous Message
                usr->lastmsg--;
                break;
            case 'Q': // Quit Message Reading
                done = true;
                break;
            default :
                break;

        }

    } */


    delete usr;
    //delete _mf;
    delete _mread;


    /*
    printf("\nMessage Reader Init");

    // Handle to Message Reader Class
    msg_read _mread;

    // Temp User Hard Code for Testing
    UserRec _usr;
    _usr.lastmbarea = 0;
    _usr.lastmsg    = 1;
    _usr.Number     = 0;
    _usr.linelen    = 80;
    _usr.pagelen    = 25;

    char mString[10] = {0};
    unsigned char ch;
    bool done  = false;
    int result = 0;

    // Startup Message Read Class
    _mread.start(pass,&_usr);
    _mread.start_reading();*/

}


void menu_func::msgpost_system ( unsigned char c )
{

    printf ( "\nMessage FSE Init" );
    char buff[2000000];
    /*
    // Handle to Message Reader Class
    class msg_fse  *_mpost = new msg_fse;

    _mpost->start(pass);
    _mpost->main_fse();

    delete _mpost; */

    msg_fse _mpost;
    _mpost.start ( pass );
    //_mpost.main_fse();
    _mpost.poll_chr ( buff );

    printf ( "\n%s",buff );

}

void menu_func::menu_docmd ( char *CmdKey, char *mString )
{

    unsigned char c1 = CmdKey[0];
    unsigned char c2 = CmdKey[1];

    _loadnew = false;

    switch ( c1 )
    {

            // Message Reader Return right away
        case '!' :
            break;

            // Matrix Menu Commands
        case '*' :
            switch ( c2 )
            {
                case 'L' : // Login
                    logon_system ( c2 );
                    break;

                case 'A' : // Apply
                    logon_system ( c2 );
                    break;

                case 'G' : // Logoff
                    pass->session->stop();
                    break;

                default  : // None Found!
                    break;
            }
            break;

            // Message System Commands
        case 'M' :
            switch ( c2 )
            {
                case 'R' : // Message Reader
                    msgread_system ( c2 );
                    _loadnew = true;
                    break;

                case 'P' : // Mesasge FSE Post
                    msgpost_system ( c2 );
                    break;

                default  : // None Found!
                    break;
            }
            break;

            // Sysop Commands
        case '%' :
            switch ( c2 )
            {
                case '#' : // Menu Editor
                    menu_edit ( pass );
                    _loadnew = true;
                    break;

                case 'M' : // Message Forum Editor
                    msgedit_system();
                    break;

                default  : // None Found!
                    break;
            }
            break;

        case '-' :
            switch ( c2 )
            {
                case '^' : // Change Menu
                    strcpy ( _curmenu,mString );
                    _loadnew = true;
                    break;

                default  : // None Found!
                    break;
            }
            break;

            // None Found!
        default  :
            break;
    }
}
