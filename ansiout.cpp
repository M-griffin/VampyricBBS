
#include <stdio.h>
#include <string>

using namespace std;

#include "struct.h"
#include "ansiout.h"


void ansi_fg(char *szEsc, int fg) {

    switch (fg) {    
        case 0:
			strcat(szEsc, "x[0;30m");
			break;
        case 1:
			strcat(szEsc, "x[0;34m");
			break;
		case 2:
			strcat(szEsc, "x[0;32m");
			break;
		case 3:
			strcat(szEsc, "x[0;36m");
			break;
		case 4:
			strcat(szEsc, "x[0;31m");
			break;
		case 5:
			strcat(szEsc, "x[0;35m");
			break;
		case 6:
			strcat(szEsc, "x[0;33m");
			break;
		case 7:
			strcat(szEsc, "x[0;37m");
			break;
		case 8:
			strcat(szEsc, "x[1;30m");
			break;
		case 9:
			strcat(szEsc, "x[1;34m");
			break;
		case 10:
			strcat(szEsc, "x[1;32m");
			break;
		case 11:
			strcat(szEsc, "x[1;36m");
			break;
		case 12:
			strcat(szEsc, "x[1;31m");
			break;
		case 13:
			strcat(szEsc, "x[1;35m");
			break;
		case 14:
			strcat(szEsc, "x[1;33m");
			break;
		case 15:
			strcat(szEsc, "x[1;37m");
			break;		
    
        default :
            break;
    }
	szEsc[0] = 27;		
}

void ansi_bg(char *szEsc, int bg) {

  	switch (bg) {  	
        case 16:
			strcat(szEsc, "x[40m");
			break;
		case 17:
			strcat(szEsc, "x[44m");
			break;
		case 18:
			strcat(szEsc, "x[42m");
			break;
		case 19:
			strcat(szEsc, "x[46m");
			break;
		case 20:
			strcat(szEsc, "x[41m");
			break;
		case 21:
			strcat(szEsc, "x[45m");
			break;
		case 22:
			strcat(szEsc, "x[43m");
			break;
        case 23:
			strcat(szEsc, "x[47m");
			break;
		default : break;
	}   
	szEsc[0] = 27;	
}

/****************************************************************************/
/* Parses String for Pipe Codes and Replaces with Ansi Color Sequences
/****************************************************************************/
void pipe2ansi(PASSING *pass, char* szString) {

    
    int id1 = 0;         // Pipe Position
    char szTmp[3];       // Holds 1, 2nd digit of Pipe
    char szReplace[10];  // Holds Converted Pipe 2 Ansi
    char szPos1[3];      // Hold XY Pos for Ansi Postion Codes
    char szPos2[3];      // Hold XY Pos for Ansi Postion Codes
    
    int test;
    
    std::string AnsiString = szString;

    // Search for First Pipe    
    id1 = 0;
    id1 = AnsiString.find("|",id1);
    while (id1 != -1) {                 
        
        if (id1 != -1) {
            memset(&szTmp,0,sizeof(szTmp));            
            memset(&szReplace,0,sizeof(szReplace));
            szTmp[0] = AnsiString[id1+1];  // Get First # after Pipe
            szTmp[1] = AnsiString[id1+2];  // Get Second Number After Pipe                        
            
            if (isdigit(szTmp[0]) && isdigit(szTmp[1])) {
            
                switch (szTmp[0]) {
                
                    case '0' :
                        switch (szTmp[1]) {
                            case '0' : ansi_fg(szReplace, 0); break;
                            case '1' : ansi_fg(szReplace, 1); break;
                            case '2' : ansi_fg(szReplace, 2); break;
                            case '3' : ansi_fg(szReplace, 3); break;
                            case '4' : ansi_fg(szReplace, 4); break;
                            case '5' : ansi_fg(szReplace, 5); break;
                            case '6' : ansi_fg(szReplace, 6); break;
                            case '7' : ansi_fg(szReplace, 7); break;
                            case '8' : ansi_fg(szReplace, 8); break;
                            case '9' : ansi_fg(szReplace, 9); break;
                            default : break;
                        }
                        break;
                    
                    case '1' :
                        switch (szTmp[1]) {
                            case '0' : ansi_fg(szReplace, 10); break;
                            case '1' : ansi_fg(szReplace, 11); break;
                            case '2' : ansi_fg(szReplace, 12); break;
                            case '3' : ansi_fg(szReplace, 13); break;
                            case '4' : ansi_fg(szReplace, 14); break;
                            case '5' : ansi_fg(szReplace, 15); break;
                            case '6' : ansi_bg(szReplace, 16); break;
                            case '7' : ansi_bg(szReplace, 17); break;
                            case '8' : ansi_bg(szReplace, 18); break;
                            case '9' : ansi_bg(szReplace, 19); break;
                            default : break;
                        }
                        break;
                    
                    case '2' :
                        switch (szTmp[1]) {
                            case '0' : ansi_bg(szReplace, 20); break;
                            case '1' : ansi_bg(szReplace, 21); break;
                            case '2' : ansi_bg(szReplace, 22); break;
                            case '3' : ansi_bg(szReplace, 23); break;
                            default : break;
                        }
                    break;
                
                    default : break;               
                }
                
                // Replace pipe code with Ansi Sequence
                if (strcmp(szReplace,"") != 0)
                    AnsiString.replace(id1,3,szReplace);            
                
            }
            // Else not a Pipe Color / Parse for Screen Modification
            else {
                // Carriage Return / New Line
                if (strcmp(szTmp,"CR") == 0){
                    ansi_bg(szReplace, 16);  // Clear Background Attribute first
                    strcat(szReplace,"\r\n");
                    AnsiString.replace(id1,3,szReplace);
                }
                // Clear Screen
                if (strcmp(szTmp,"CS") == 0){
                    ansi_bg(szReplace, 16);  // Clear Background Attribute first
                    strcat(szReplace,"\x1b[2J");
                    AnsiString.replace(id1,3,szReplace);
                }
                // Sexy XY Cursor Position
                if (strcmp(szTmp,"XY") == 0){
                    memset(&szPos1,0,sizeof(szPos1));
                    memset(&szPos2,0,sizeof(szPos2));
                    
                    // X Pos
                    szPos1[0] = AnsiString[id1+3];
                    szPos1[1] = AnsiString[id1+4];
                    // Y Pos
                    szPos2[0] = AnsiString[id1+5];
                    szPos2[1] = AnsiString[id1+6];
                
                    ansi_bg(szReplace, 16);  // Clear Background Attribute first
                    sprintf(szReplace,"%s\x1b[%i;%iH",szReplace, atoi(szPos2),atoi(szPos1));
                    AnsiString.replace(id1,7,szReplace);
                }
            }                      
        }            
        else break; 
        
        // Scan for next Pipe Code    
        id1 = AnsiString.find("|",id1+1);      
    }
    
    while ( pass->session->isActive() ) {
        if (pass->session->write( (char *)AnsiString.c_str(), AnsiString.size()) > 1 ) break;
        Sleep(10);
    }
}

// Get Ansi X/Y , Also For knowing when the Ansi Has finished displaying
//--------------------------------------------------------------------
//void ansi_getxy(PASSING *pass, int* x, int* y) {
void ansi_getxy(PASSING *pass) {

    unsigned char c;
	int result;

    //*x=0;
    //*y=0;

    // Request cusor position
    pass->session->ansigetxy = true;
	
    // Detection
    while (pass->session->isActive()) {
       printf("running loop");
        
       if (pass->session->puts("\x1b[6n") == 0 ) break;
       Sleep(5);
    }

    

    while(pass->session->isActive()) {

  		result = pass->session->pollc(c);
        if (!result) break;
        Sleep(10);
         		
        /*  - Temp don't need position just yet!  		
		if((ch=incom())!=NOINP) {
		
			if(ch==ESC && rsp==0) {
            	rsp++;
				start=time(NULL);
			}
            else if(ch=='[' && rsp==1) {
            	rsp++;
				start=time(NULL);
			}
            else if(isdigit(ch) && rsp==2) {
               	(*y)*=10;
                (*y)+=(ch&0xf);
				start=time(NULL);
            }
            else if(ch==';' && rsp>=2) {
            	rsp++;
				start=time(NULL);
			}
            else if(isdigit(ch) && rsp==3) {
            	(*x)*=10;
                (*x)+=(ch&0xf);
				start=time(NULL);
            }
            else if(ch=='R' && rsp)
            	break;
			else
				ungetkey(ch);
        }
    	if(time(NULL)-start>TIMEOUT_ANSI_GETXY) {
        	lprintf("!Node %d: TIMEOUT in ansi_getxy", cfg.node_num);
            break;
        }
        */
        
    }
    printf("end of running loop");
}

// Display Ansi file over Zsocket
//--------------------------------------------------------------------
void ansiPrintf(PASSING *pass, char *filename) {

    char szDateFormat[128];	// System Date
    char szTimeFormat[128];	// System Time

    int c;
    long int size;
    char *AnsiBuf;
    
    std::string path = ANSIPATH;
    path += filename;
    path += ".ans";

    // Get Filesize to Allocate Buffer
    FILE *inStream;
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL) {
        //printf("\ncan't open %s", path.c_str());
        return;
    }
    fseek(inStream, 0L, SEEK_END);    /* Position to end of file */
    size = ftell(inStream);           /* Get file length */
    fclose(inStream);

    AnsiBuf = new char [size+1];
    memset(AnsiBuf,0,sizeof(size+1));

    // Open and Read Ansi file
    if ((inStream = fopen(path.c_str(), "r+")) ==  NULL) {
        //printf("can't open %s", path.c_str());
        return;
    }

    int i = 0;
    do {
        c = getc(inStream);
        if (c == '\n') { AnsiBuf[i] = '\r'; ++i; }
        AnsiBuf[i] = c;
        i++;
    }
    while (c != EOF);
    fclose(inStream);
      
    while ( pass->session->isActive() ) {    
        if (pass->session->write( AnsiBuf, size) > 1 ) break;
        Sleep(10);
    }
    
    delete [] AnsiBuf;
    return;
}


