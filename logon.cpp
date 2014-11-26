
#include <stdio.h>
#include <string>

using namespace std;

#include "struct.h"
#include "logon.h"

#include "ansiout.h"
#include "language.h"
#include "menu_func.h"
#include "misc.h"
#include "users.h"

// Class Constructor
//----------------------------------------------------------------------------

logon::logon() {

   
}

// Start BBS Menu System
//----------------------------------------------------------------------------
void logon::start(PASSING *passing, UserRec *user) {

    pass = passing;  // Holds Session I/O
    ur   = user;     // Holds Reference to Loaded User
    memset(ur,0,sizeof(UserRec));
}

void logon::login() {

    // Starting New User Application Process
    pass->session->puts("\x1b[2J");  // Clear Screen    
}


void logon::Handle() {

    char rBuffer[200];
    char text[200]={0};

    while (1) {
        // Ask for Users Handle
  	    strcpy(rBuffer,"");
        _lang.lang_get(text,8);
        strcat(text,"|15|17                              \x1b[30D");
        pipe2ansi(pass,text);
  	    get_str(pass,rBuffer);
   	    if (strcmp(rBuffer,"") != 0) {
            // Check if already exists 
            if(!_user.idx_match(rBuffer)) {
                strcpy(ur->Handle,rBuffer);
                break;
            }
            else {
            // Incorrect Handle
                _lang.lang_get(text,18);
                pipe2ansi(pass,text);
            }
        }
    }
}

void logon::Name() {

    char rBuffer[200];
    char text[200]={0};

    // Ask for Users Real Name
  	strcpy(rBuffer,"");
    _lang.lang_get(text,9);
    strcat(text,"|15|17                              \x1b[30D");
    pipe2ansi(pass,text);
    get_str(pass,rBuffer);
   	if (strcmp(rBuffer,"") != 0) strcpy(ur->Name,rBuffer);
   	
}

void logon::Password() {

    char rBuffer[200];
    char text[200]={0};

    // Ask for Users Password
    strcpy(rBuffer,"");
    _lang.lang_get(text,10);
    strcat(text,"|15|17                              \x1b[30D");
    pipe2ansi(pass,text);
  	get_str(pass,rBuffer);
   	if (strcmp(rBuffer,"") != 0) strcpy(ur->Password,rBuffer);
   	
}

void logon::Sex() {

    char text[200]={0};
    unsigned char c;

    // Ask Sex M/F
    _lang.lang_get(text,11);
    strcat(text,"|15|17 \x1b[D");
    pipe2ansi(pass,text);       
	while (pass->session->isActive()) {
        get_chr(pass,c);
       	if (toupper(c) == 'M' || toupper(c) == 'F') { 
            sprintf(text,"|15|17%c",c);
            pipe2ansi(pass,text);
            ur->Sex = c;
            break;
        }
  	}
}

void logon::BDay() {

    char rBuffer[200];
    char text[200]={0};

    // Ask for Users Bitchday
    strcpy(rBuffer,"");
    _lang.lang_get(text,12);
    strcat(text,"|15|17                              \x1b[30D");
    pipe2ansi(pass,text);
  	get_str(pass,rBuffer);
   	if (strcmp(rBuffer,"") != 0) strcpy(ur->Password,rBuffer);
   	
}

void logon::Email() {

    char rBuffer[200];
    char text[200]={0};

    // Ask for Email Address
    strcpy(rBuffer,"");
    _lang.lang_get(text,13);
    strcat(text,"|15|17                                        \x1b[40D");
    pipe2ansi(pass,text);
  	get_str(pass,rBuffer);
   	if (strcmp(rBuffer,"") != 0) strcpy(ur->Email,rBuffer);
   	
}

void logon::EmailPriv() {

    char text[200]={0};
    unsigned char c;

    // Ask to Keep Email Private
    _lang.lang_get(text,14);
    strcat(text,"|15|17 \x1b[D");
    pipe2ansi(pass,text);   
   	get_chr(pass,c);
  	if (toupper(c) == 'Y') ur->EmailPrivate = true;
  	else ur->EmailPrivate = false;
  	sprintf(text,"|15|17%c",c);
    pipe2ansi(pass,text);
}

void logon::Note() {

    char rBuffer[200];
    char text[200]={0};

    // Desired User Note
    strcpy(rBuffer,"");
    _lang.lang_get(text,15);
    strcat(text,"|15|17                                        \x1b[40D");
    pipe2ansi(pass,text);
  	get_str(pass,rBuffer);
   	if (strcmp(rBuffer,"") != 0) strcpy(ur->UserNote,rBuffer);
   	
}

void logon::Word() {

    char rBuffer[200];
    char text[200]={0};

    // Secret Word - If forgot password
    strcpy(rBuffer,"");
    _lang.lang_get(text,16);
    strcat(text,"|15|17                                        \x1b[40D");
    pipe2ansi(pass,text);
  	get_str(pass,rBuffer);
   	if (strcmp(rBuffer,"") != 0) strcpy(ur->Word,rBuffer);
   	
}


void logon::application() {

    // Starting New User Application Process
    ansiPrintf(pass, "newuser");
    
    char text[200]={0};
    int result;
    unsigned char c;    
   
    // Ask If want to Register a new account
    _lang.lang_get(text,7);
    strcat(text,"|15|17 \x1b[D");
    pipe2ansi(pass,text);         	
  	while (pass->session->isActive()) {
  	    get_chr(pass,c);
  	    if (toupper(c) == 'Y') break;
  	    else if (toupper(c) == 'N') return;
    }
    sprintf(text,"|15|17%c",c);
    pipe2ansi(pass,text);
  	
   	Handle();    // Handle
    Name();      // Name
    Password();  // Password
    Sex();       // Sex
    BDay();      // Birthday
    Email();     // Email
    EmailPriv(); // Keep Email Private
    Note();      // Note
    Word();      // Secret Word
   	
   	// Save or Change User Info
   	verify_info();
        
}


void logon::ParseVerify(char *filename) {

    char szDateFormat[128];	// System Date
    char szTimeFormat[128];	// System Time

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
    fseek(inStream, 0L, SEEK_END);    // Position to end of file
    size = ftell(inStream);           // Get file length
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
			
			if (strcmp(MCI,"UH") == 0)      { temp += ur->Handle;   } // Users Handle
			else if (strcmp(MCI,"UN") == 0) { temp += ur->Name;     } // Users Name
			else if (strcmp(MCI,"UP") == 0) { temp += ur->Password; } // Users Password			
			else if (strcmp(MCI,"US") == 0) { temp += ur->Sex;      } // Users Sex
			else if (strcmp(MCI,"BD") == 0) { temp += ur->BDay;     } // Birthday
			else if (strcmp(MCI,"NO") == 0) { temp += ur->UserNote; } // User Note
			else if (strcmp(MCI,"WD") == 0) { temp += ur->Word;     } // Secret Word
			else if (strcmp(MCI,"EM") == 0) { temp += ur->Email;    } // Users Email
			else if (strcmp(MCI,"EP") == 0) { // Email Private?
     	        if (ur->EmailPrivate) 
                    temp += "yes";
                else
                    temp += "no"; 
            } 
			else { temp += c; temp += MCI; }
			
			temp += getc(inStream);
		}
		
        else if (c == '\n') temp += '\r';
        else temp += c;

    }
    while (c != EOF);
    fclose(inStream);
    
    temp += "\r\n"; // NewLine to Fix Next Ansi Sequence
    while ( pass->session->isActive() ) {    
        if (pass->session->puts((char *)temp.c_str()) > 1 ) break;
        Sleep(10);
    }
    return;
}

void logon::save_user() {
    
    int idx = _user.idx_count();
    _user.users_write(ur,idx);

}

void logon::verify_info() {

    // Initalize Menu Function Class
    menu_func _mf;                    
    _mf.start(pass,ur); 
    
    char mString[10] = {0};
    char text[200]   = {0};
    bool done        = false;
    unsigned char ch;
    
    // Setup the Menu For Lightbars
    strcpy(_mf._curmenu,"verify");

    while(pass->session->isActive() && !done) {
    
        // Setup Screen Display Ansi Header
        pass->session->puts("\x1b[0m\x1b[2J");            
        ParseVerify("verify");                

        // Readin the Menu Prompt
        _mf.menu_readin();

        // Process Verify Prompt with lightbar menu             
        _mf.menu_proc(mString);
               
        // Process Return Input from Lightbars, Next, Prev, Quit ...        
        ch = mString[0];   
        
        switch (toupper(ch)) {        
            case 'Q': // Save & Exit                       
                save_user();
                done = true;
                break;
                
            case 'E': // Edit / Change a Setting 
                // Display Mdify which string
                _lang.lang_get(text,22);
                strcat(text, "|15|17 \x1b[D");
                pipe2ansi(pass,text);
                
                get_chr(pass,ch);
                // Draw out key inputted
                sprintf(text,"|15|17%c",ch);
                pipe2ansi(pass,text);
                
                if (isdigit(ch)) {
                    switch (ch) {
                        case '1': 
                            Handle();
                            break;
                            
                        case '2':
                            Name();
                            break;
                            
                        case '3':
                            Password();
                            break;
                            
                        case '4':
                            Sex();
                            break;
                            
                        case '5':
                            BDay();
                            break;
                            
                        case '6':
                            Note();
                            break;
                            
                        case '7':
                            Word();
                            break;
                            
                        case '8':
                            Email();
                            break;
                            
                        case '9':
                            EmailPriv();
                            break;
                            
                        default :
                            break;                    
                    }
                }                                                                                        
                break;
                
            case 'A': // Abort
                return;
                
            default : 
                break;
    
        }
    }   
}


