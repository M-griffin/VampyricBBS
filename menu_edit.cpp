
#include <stdio.h>
#include <string>

using namespace std;

#include "struct.h"

#include "ansiout.h"
#include "misc.h"

#include "menu_edit.h"
#include "menu_func.h"



// Class Constructor
//----------------------------------------------------------------------------

menu_editor::menu_editor() {

}

// Start BBS Menu System
//----------------------------------------------------------------------------
void menu_editor::start(PASSING *passing) {

    //memcpy(&pass,&passing,sizeof(PASSING));
    pass = passing;
}


// Reads In and Sorts All Menu files on the fly
//----------------------------------------------------------------------------
void menu_editor::menu_list() {

	MenuRec menur;	
	memset(&menur,0,sizeof(MenuRec));
 
    char dBuffer[1024]={0};	
	
    pass->session->puts("\x1b[0m\x1b[2J");
    pipe2ansi(pass, "|08.|07----------------------------------------------------------------|08.\r\n");
    pipe2ansi(pass, "|07| |15Menu Files Listing|07.. .                                         |07|\r\n");
    pipe2ansi(pass, "|08`|07----------------------------------------------------------------|08'\r\n");
    pipe2ansi(pass, "  |15##   |09Menuname                           |11SL          |03Lightbar\r\n");
    pipe2ansi(pass, "|08.|07----------------------------------------------------------------|08.\r\n");
    
	int i   = 0;
	int row = 6;
	std::string TF;
	unsigned char c;
	int result;
	

	
	i = _mnu.menu_count();
	if (i != 0) {
		i = 0;
		while(_mnu.menu_read(&menur,i++)) {	
			if (menur.Lightbar == true) TF = "yes";
			else TF = "no";
    	  	sprintf(dBuffer,"|07| |15%i.  \x1b[%i;8H|09%s \x1b[%i;43H|11%s \x1b[%i;55H|03%s \x1b[%i;66H|07|\r\n",i,row,menur.MenuName,row,menur.Acs,row,TF.c_str(),row);
    	  	pipe2ansi(pass,dBuffer);
        	++row;
        	
        	// Do Screen Pause
        	if (row > 22) {
        	    pipe2ansi(pass,"\r\n|15press any key to continue|07.. .");
        	   	while (1) {        	   	
	   	    	    // Check conenction and Poll for messages
       		        if( pass->session->isActive() ) {
           		        get_chr(pass,c);
		                // Display Header
                        pass->session->printf("\x1b[0m\x1b[2J");
                        pipe2ansi(pass, "|08.|07----------------------------------------------------------------|08.\r\n");
                        pipe2ansi(pass, "|07| |15Menu Files Listing|07.. .                                         |07|\r\n");
                        pipe2ansi(pass, "|08`|07----------------------------------------------------------------|08'\r\n");
                        pipe2ansi(pass, "  |15##   |09Menuname                           |11SL          |03Lightbar\r\n");
                        pipe2ansi(pass, "|08.|07----------------------------------------------------------------|08.\r\n");
		                row = 6;
                        break; 
      		        }
      		        else return;
                }       	
        	}        	
	    }
 	}
 	else {
 		pipe2ansi(pass,"|07| |15No Menu Files Found!\x1b[6;66H|07|\r\n");
	}
	pipe2ansi(pass,"|08`|07----------------------------------------------------------------|08'\r\n");
	pass->session->printf("\r\n");
	sprintf(dBuffer," |08[|15c|08]|07reate, |08[|15d|08]|07elete, |08[|15e|08]|07dit, |08[|15q|08]|07uit : |15|17 %c", 0x08);
	pipe2ansi(pass, dBuffer);
	return;
}


// Reads In and Sorts All Menu files on the fly
//----------------------------------------------------------------------------
void menu_editor::cmds_list(char *menuname) {

	CommandRec cmdr;	
	memset(&cmdr,0,sizeof(CommandRec));	
	
    // Display Header
    pass->session->printf("\x1b[0m\x1b[2J");
    pass->session->printf(".----------------------------------------------------------------.\r\n");
    pass->session->printf("| Command Listing.. .  For Menu: %s \x1b[2;66H|\r\n", menuname);
    pass->session->printf("`----------------------------------------------------------------'\r\n");
    pass->session->printf("  ##   Description  \r\n");
    pass->session->printf(".----------------------------------------------------------------.\r\n");

	int i   = 0;
	int row = 6;
	std::string TF;
	unsigned char c;
	int result;
	
	
	i = _mnu.cmds_count(menuname);
	
	if (i != 0) {
		i = 0;
		while(_mnu.cmds_read(menuname,&cmdr,i++)) {	
    	  	pass->session->printf("| %i.  \x1b[%i;8H%s \x1b[%i;66H|\r\n",i,row,cmdr.LDesc,row);
        	++row;
        	
        	// Do Screen Pause
        	if (row > 22) {
        	    pass->session->printf("\r\npress any key to continue.. .");
        	   	while (1) {
	   	    	    // Check conenction and Poll for messages
       		        if( pass->session->isActive() ) {
           		        get_chr(pass,c);       			            
		                pass->session->printf("\x1b[0m\x1b[2J");
                        pass->session->printf(".----------------------------------------------------------------.\r\n");
                        pass->session->printf("| Command Listing.. .  For Menu: %s \x1b[2;66H|\r\n", menuname);
                        pass->session->printf("`----------------------------------------------------------------'\r\n");
                        pass->session->printf("  ##   Description  \r\n");
                        pass->session->printf(".----------------------------------------------------------------.\r\n");
		                row = 6;
                        break;                          			           			    
      		        }
      		        else return;
                }       	
        	}        	
	    }
 	}
 	else {
 		pass->session->printf("| No Commands Found In This Menu! \x1b[6;66H|\r\n");
	} 		
    pass->session->printf("`----------------------------------------------------------------'\r\n");
    pass->session->printf("\r\n");
    pass->session->puts(" [c]opy, [d]elete, [e]dit, [i]nsert, [m]ove, [q]uit : \x1b[1;44m \x1b[D");    
	return;
}


// Reads In and Sorts All Menu files on the fly
//----------------------------------------------------------------------------
void menu_editor::cmds_edit(char *menuname, int num) {

    --num;

	CommandRec cmdr;
	memset(&cmdr,0,sizeof(CommandRec));
	
	
	int count = _mnu.cmds_count(menuname);
	_mnu.cmds_read(menuname, &cmdr, num);
	
	unsigned char c;
    int result = 0;
    char rBuffer[1024];
    std::string TF;
	
	while (pass->session->isActive()) {
	
		// Display Header
    	pass->session->puts("\x1b[0m\x1b[2J");
	    pass->session->puts(".----------------------------------------------------------------.\r\n");
   		pass->session->puts("| Command Editor.. .                                             |\r\n");
    	pass->session->puts("`----------------------------------------------------------------'\r\n");
        pass->session->printf("  Menu: %s.mnu - Command #: %i", menuname, num+1);
        pass->session->puts("\r\n");
    	pass->session->puts(".-------------------------.");	
        pass->session->puts("\r\n");
    	pass->session->printf("| [A]. Description        | %s", cmdr.LDesc); 
    	pass->session->puts("\r\n");
		pass->session->printf("| [B]. Display Text       | %s", cmdr.SDesc);
		pass->session->puts("\r\n");
    	pass->session->printf("| [C]. Menu Key           | %s", cmdr.CKeys);
    	pass->session->puts("\r\n");
    	pass->session->printf("| [D]. Access Required    | %s", cmdr.Acs);    	
    	pass->session->puts("\r\n");
        pass->session->printf("| [E]. Command            | %s", cmdr.CmdKeys);
        pass->session->puts("\r\n");
    	pass->session->printf("| [F]. Command Data       | %s", cmdr.MString);
    	pass->session->puts("\r\n");
       	pass->session->puts("|                         |");
     	pass->session->puts("\r\n");
    	pass->session->printf("| [G]. Lightbar Highlight | %s", cmdr.HiString);
        pass->session->puts("\r\n");			    	
		pass->session->printf("| [H]. Lightbar Low       | %s", cmdr.LoString);
		pass->session->puts("\r\n");
		pass->session->printf("| [I]. Lightbar X Coord   | %i", cmdr.Xcoord);
		pass->session->puts("\r\n");
		pass->session->printf("| [J]. Lightbar Y Coord   | %i", cmdr.Ycoord);
		pass->session->puts("\r\n");
		if (cmdr.LBarCmd) TF = "on";
		else TF = "off";
		pass->session->printf("| [K]. Flag as Lightbar   | %s", TF.c_str());
		pass->session->puts("\r\n");
        if (cmdr.SText) TF = "on";
		else TF = "off";       
        pass->session->printf("| [L]. Flag as Scrolling  | %s", TF.c_str());
        pass->session->puts("\r\n");
		pass->session->printf("| [M]. Scroll Text Length | %i", cmdr.STLen);
		pass->session->puts("\r\n");
     	pass->session->puts("`-------------------------'");
     	pass->session->puts("\r\n");
    	pass->session->printf("  [Q]. Quit, [-]. Previous, [+]. Next : \x1b[1;44m \x1b[D");
		   
		get_chr(pass,c);		   		
   		switch(toupper(c)) {
   		
   		   case '+' :
           case ']' :
                if (num < count-1) {
                    _mnu.cmds_write(menuname,&cmdr,num);
                    ++num;
                    _mnu.cmds_read(menuname,&cmdr,num);
                }
   		        break;   	
   		        
            case '-' :
            case '[' : 
   		        if (num != 0 ) {
                    _mnu.cmds_write(menuname,&cmdr,num);
                    --num;
                    _mnu.cmds_read(menuname,&cmdr,num);
                }
   		        break;	   		
   		
   		    case 'A' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m                                                            \x1b[60D");
       		    get_str(pass,rBuffer);
	            if (strcmp(rBuffer,"") != 0) strcpy(cmdr.LDesc,rBuffer);
   		        break;   		    
             
	        case 'B' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m                                        \x1b[40D");
      		    get_str(pass,rBuffer);
                if (strcmp(rBuffer,"") != 0) strcpy(cmdr.SDesc,rBuffer);	
   		        break;   		        

	        case 'C' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m          \x1b[10D");
                get_str(pass,rBuffer);
   		        if (strcmp(rBuffer,"") != 0) strcpy(cmdr.CKeys,rBuffer);
   		        break;
   		        
	        case 'D' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m          \x1b[10D");
                get_str(pass,rBuffer);
   		        if (strcmp(rBuffer,"") != 0) strcpy(cmdr.Acs,rBuffer);
   		        break;
   		        
	        case 'E' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m  \x1b[2D");
           	    get_str(pass,rBuffer);
        	    if (strcmp(rBuffer,"") != 0) strcpy(cmdr.CmdKeys,rBuffer);
   		        break;
   		        
	        case 'F' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m                                                                      \x1b[70D");
        	    get_str(pass,rBuffer);
   		        if (strcmp(rBuffer,"") != 0) strcpy(cmdr.MString,rBuffer);
   		        break;
	        
	        case 'G' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m                                        \x1b[40D");
                get_str(pass,rBuffer);
        	    if (strcmp(rBuffer,"") != 0) strcpy(cmdr.HiString,rBuffer);
   		        break;
   		        
	        case 'H' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m                                        \x1b[40D");
        	    get_str(pass,rBuffer);
   		        if (strcmp(rBuffer,"") != 0) strcpy(cmdr.LoString,rBuffer);
   		        break;
   		        
	        case 'I' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m  \x1b[2D");
                get_str(pass,rBuffer);
   		        if (strcmp(rBuffer,"") != 0) { 
   		            result = atoi(rBuffer);
                    cmdr.Xcoord = result;
                }
   		        break;
   		        
	        case 'J' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m  \x1b[2D");
           	    get_str(pass,rBuffer);
        	    if (strcmp(rBuffer,"") != 0) { 
   		            result = atoi(rBuffer);
                    cmdr.Ycoord = result;
                }	            
   		        break;
   		        
	        case 'K' :
	           if (cmdr.LBarCmd) cmdr.LBarCmd = false;
               else { cmdr.LBarCmd = true; cmdr.SText = false; }
               break;
             

            case 'L' :
	           if (cmdr.SText) cmdr.SText = false;
               else { cmdr.SText = true; cmdr.LBarCmd = false; }
               break;
               
            case 'M' :
	           pass->session->puts("\x1b[0m\r\n\r\n  -> \x1b[1;44m  \x1b[2D");
           	   get_str(pass,rBuffer);
        	   if (strcmp(rBuffer,"") != 0) { 
   		           result = atoi(rBuffer);
                   cmdr.STLen = result;              
	           }
   		       break;

   			case 'Q' : 
                _mnu.cmds_write(menuname,&cmdr,num);
                return;
                               
            default : break;
		}  
	}   
}

// Reads In and Sorts All Menu files on the fly
//----------------------------------------------------------------------------
void menu_editor::cmds_editor(char *menuname) {
     
    // Dispaly Menu and Retrive Selection
    unsigned char c;
    int result  = 0;
    int result2 = 0;
    char rBuffer[1024];
    
    menus mnu;
       
    // Command Editor Main Loop
    while (pass->session->isActive()) {
    
        cmds_list(menuname);

	    get_chr(pass, c);
        switch (toupper(c)) {

            case 'I' : 
                pass->session->puts("\r\n\r\n Insert How many new Commands?: \x1b[1;44m   \x1b[3D");
          		get_str(pass, rBuffer);
       	    	if (strcmp(rBuffer,"") != 0) {         
                    result = atoi(rBuffer);                                        
      		        mnu.cmds_create(menuname, result);
    		    }
                break;

            case 'D' :
            	pass->session->puts("\r\n\r\n Delete which #: \x1b[1;44m  \x1b[2D");
                get_str(pass, rBuffer);            	
                if (strcmp(rBuffer,"") != 0) {         
                    result = atoi(rBuffer);                                        
          		    mnu.cmds_del(menuname,result);
			    }
                break;

            case 'E' :
                pass->session->puts("\r\n\r\n Edit Which Command #: \x1b[1;44m  \x1b[2D");
                get_str(pass, rBuffer);
        		if (strcmp(rBuffer,"") != 0) {         
                    result = atoi(rBuffer);                                        
          		    cmds_edit(menuname,result);
			    }
                break;
            
            case 'M' :
                pass->session->puts("\r\n\r\n Move Which Command #: \x1b[1;44m  \x1b[2D");
                get_str(pass, rBuffer);
        		if (strcmp(rBuffer,"") != 0) {         
                    result = atoi(rBuffer);                                        
			    }
			    pass->session->puts("\r\n Move Before Which Command #: \x1b[1;44m  \x1b[2D");
                get_str(pass, rBuffer);
        		if (strcmp(rBuffer,"") != 0) {         
                    result2 = atoi(rBuffer);                                        
			    }
			    mnu.cmds_move(menuname, result, result2);
			    break;
		
            case 'C' :
                pass->session->puts("\r\n\r\n Copy Which Command #: \x1b[1;44m  \x1b[2D");
                get_str(pass, rBuffer);
        		if (strcmp(rBuffer,"") != 0) {         
                    result = atoi(rBuffer);                                        
			    }
			    pass->session->puts("\r\n Make How many Copies #: \x1b[1;44m  \x1b[2D");
             	pass->session->printf("\x1b[2D");
			    get_str(pass, rBuffer);
        		if (strcmp(rBuffer,"") != 0) {         
                    result2 = atoi(rBuffer);                                        
			    }
			    mnu.cmds_copy(menuname, result, result2);
			    break;

            case 'Q' :
                return;
        }
	}
}

// Reads In and Sorts All Menu files on the fly
//----------------------------------------------------------------------------
void menu_editor::menu_edit(char *menuname) {

	MenuRec menur;	
	memset(&menur,0,sizeof(MenuRec));
	
	
	int idx = _mnu.menu_find(menuname);
	_mnu.menu_read(&menur,idx);
	
	unsigned char c;
    int result = 0;
    char rBuffer[1024];
    std::string TF;
    
    // Menu Function Class for Testing Menus
    menu_func *mnuf = new menu_func;
    
    UserRec users;
    mnuf->start(pass,&users);
	
	while (pass->session->isActive()) {
	
		// Display Header
    	pass->session->puts("\x1b[0m\x1b[2J");
	    pass->session->puts(".----------------------------------------------------------------.\r\n");
   		pass->session->puts("| Menu File Editor.. .                                           |\r\n");
    	pass->session->puts("`----------------------------------------------------------------'\r\n");
        pass->session->printf("  Menu: %s.mnu", menur.MenuName);
        pass->session->puts("\r\n");
    	pass->session->puts(".-------------------------.");	
        pass->session->puts("\r\n");
    	pass->session->printf("| [A]. Menu Prompt String | %s", menur.MenuPrompt); 
    	pass->session->puts("\r\n");
		pass->session->printf("| [B]. Menu Ansi File     | %s", menur.Directive);
		pass->session->puts("\r\n");
    	pass->session->printf("| [C]. Access Required    | %s", menur.Acs);
    	pass->session->puts("\r\n");
    	pass->session->printf("| [D]. Password           | %s", menur.Password);    	
    	pass->session->puts("\r\n");
        pass->session->puts("|                         |");
        pass->session->puts("\r\n");
       	if (menur.ForceInput == 0) TF = "Users Defaults";
		else if (menur.ForceInput == 1) TF = "Force Hotkey ON";
		else TF = "Force Hotkey OFF";
        pass->session->printf("| [E]. Forced Input Type  | %s", TF.c_str());
        pass->session->puts("\r\n");
        if (menur.ForceHelpLevel == 0) TF = "Users Defaults";
		else if (menur.ForceHelpLevel == 1) TF = "Force Ansi Displayed";
		else TF = "Force Expert Mode";
    	pass->session->printf("| [F]. Menu Ansi Display  | %s", TF.c_str());
    	pass->session->puts("\r\n");
    	if (menur.Lightbar == true) TF = "yes";
		else TF = "no";
    	pass->session->printf("| [G]. Lightbar Menu      | %s", TF.c_str());
        pass->session->puts("\r\n");			    	
    	pass->session->puts("|                         |"); 
    	pass->session->puts("\r\n");
        if (menur.MFlags.ClrScrBefore == true) TF = "yes";
		else TF = "no";          
		pass->session->printf("| [H]. Clear Before Menu  | %s", TF.c_str());
		pass->session->puts("\r\n");
		if (menur.MFlags.NoMenuPrompt == true) TF = "yes";
		else TF = "no";          
		pass->session->printf("| [I]. No menu prompt     | %s", TF.c_str());
		pass->session->puts("\r\n");
		if (menur.MFlags.ForcePause == true) TF = "yes";
		else TF = "no";          
		pass->session->printf("| [J]. Pause before menu  | %s", TF.c_str());
		pass->session->puts("\r\n");
		if (menur.MFlags.UseGlobal == true) TF = "yes";
		else TF = "no"; 
		pass->session->printf("| [K]. Use global menu    | %s", TF.c_str());
		pass->session->puts("\r\n");
		if (menur.MFlags.ClrScrAfter == true) TF = "yes";
		else TF = "no";
    	pass->session->printf("| [L]. Clear After Input  | %s", TF.c_str());
    	pass->session->puts("\r\n");
     	pass->session->puts("|                         |");
     	pass->session->puts("\r\n");
     	pass->session->puts("| [X]. Edit Commands      |");
     	pass->session->puts("\r\n");
     	pass->session->puts("`-------------------------'");
     	pass->session->puts("\r\n");
    	pass->session->printf("  [Q]. Quit, [T]. Test Menu : \x1b[1;44m \x1b[D");
	
		get_chr(pass,c);        		
   		switch(toupper(c)) {
   		
   		    case 'A' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m                                                            \x1b[60D");
       		    get_str(pass,rBuffer);
	            if (strcmp(rBuffer,"") != 0) strcpy(menur.MenuPrompt,rBuffer);
   		        break;
   		        
	        case 'B' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m                    \x1b[20D");
       		    get_str(pass,rBuffer);
	            if (strcmp(rBuffer,"") != 0) strcpy(menur.Directive,rBuffer);
   		        break;
   		        
	        case 'C' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m          \x1b[10D");
       		    get_str(pass,rBuffer);
   	            if (strcmp(rBuffer,"") != 0) strcpy(menur.Acs,rBuffer);
		        break;
   		        
	        case 'D' : 
   		        pass->session->puts("\r\n\r\n  -> \x1b[1;44m          \x1b[10D");        	    
        		get_str(pass,rBuffer);
 	            if (strcmp(rBuffer,"") != 0) strcpy(menur.Password,rBuffer);
                break;   		        
   		        
	        case 'E' :
	            if (menur.ForceInput == 2) menur.ForceInput = 0;
	            else ++menur.ForceInput;   		        
   		        break;
   		        
	        case 'F' :
	            if (menur.ForceHelpLevel == 2) menur.ForceHelpLevel = 0;
	            else ++menur.ForceHelpLevel;   		        
   		        break;
   		        
            case 'G' :
	            if (menur.Lightbar) menur.Lightbar = false;
	            else menur.Lightbar = true;   		        
   		        break;
   		        
	        case 'H' :
	            if (menur.MFlags.ClrScrBefore) menur.MFlags.ClrScrBefore = false;
	            else menur.MFlags.ClrScrBefore = true;   		        
   		        break;
   		        
	        case 'I' :
	            if (menur.MFlags.NoMenuPrompt) menur.MFlags.NoMenuPrompt = false;
	            else menur.MFlags.NoMenuPrompt = true;   		        
   		        break;
   		        
	        case 'J' :
	            if (menur.MFlags.ForcePause) menur.MFlags.ForcePause = false;
	            else menur.MFlags.ForcePause = true;   		        
   		        break;
   		        
	        case 'K' :
	            if (menur.MFlags.UseGlobal) menur.MFlags.UseGlobal = false;
	            else menur.MFlags.UseGlobal = true;   		        
   		        break;
   		        
	        case 'L' :
	            if (menur.MFlags.ClrScrAfter) menur.MFlags.ClrScrAfter = false;
	            else menur.MFlags.ClrScrAfter = true;   		        
   		        break;
   		        
	        case 'X' :
	            cmds_editor(menuname);
	            break;
          
            case 'T' :
                // Set Menu                
                strcpy(mnuf->_curmenu,menuname);
                // Read In Menu
        	    mnuf->menu_readin();
        	    char in[10];
        	    mnuf->menu_proc(in);
        	    break;
   		
   			case 'Q' : 
                _mnu.menu_write(&menur,idx);
                delete mnuf;
                return;
                
            default  : 
                break;
		}  
	}
	delete mnuf;
}


void menu_edit(PASSING *pass) {

    /*
    menu_editor *mnued  = new menu_editor;
    menus _mnu;
    
    // Initalize Menu Editing System
    mnued->start(pass);
    
    // Dispaly Menu and Retrive Selection
    unsigned char c;
    int result = 0;
    char rBuffer[100]={0};
    
    while (pass->session->isActive()) {
    
        mnued->menu_list();  // List Menu      
    	get_chr(pass,c);
    
        switch (toupper(c)) {    
            case 'C' :
                pass->session->puts("\r\n\r\n New Menu Name: \x1b[1;44m                    \x1b[20D");        	
        		get_str(pass,rBuffer);
        		if (strcmp(rBuffer,"") != 0) {          
          			if (!_mnu.menu_create(rBuffer)) {
	    	        	pass->session->printf("\r\n Error Creating New Menu: %s", rBuffer);
    			    }
			    }
			    break;
			    
            case 'D' :
            	pass->session->puts("\r\n\r\n Menu Name to Delete: \x1b[1;44m                    \x1b[20D");        
        		get_str(pass,rBuffer);
        		_mnu.menu_del(rBuffer);
        		break;

            case 'E' :
                pass->session->puts("\r\n\r\n Menu Name: \x1b[1;44m                    \x1b[20D");
        		get_str(pass,rBuffer);
       			if (!_mnu.menu_match(rBuffer)) {  // Make sure menu is a match
	        		pass->session->printf("\r\n %s doesn't exist!", rBuffer);
   	    			Sleep(1000);        		
    			}
    			else mnued->menu_edit(rBuffer);
    			break;


            case 'Q' :
                delete mnued;
                return;
                
            default  :
                break;
             
        }
	}
    delete mnued;		
    */
    
    menu_editor _mnued;
    menus       _mnu;
    
    // Initalize Menu Editing System
    _mnued.start(pass);
    
    // Dispaly Menu and Retrive Selection
    unsigned char c;
    int result = 0;
    char rBuffer[100]={0};
    
    while (pass->session->isActive()) {
    
        _mnued.menu_list();  // List Menu      
    	get_chr(pass,c);
    
        switch (toupper(c)) {    
            case 'C' :
                pass->session->puts("\r\n\r\n New Menu Name: \x1b[1;44m                    \x1b[20D");        	
        		get_str(pass,rBuffer);
        		if (strcmp(rBuffer,"") != 0) {          
          			if (!_mnu.menu_create(rBuffer)) {
	    	        	pass->session->printf("\r\n Error Creating New Menu: %s", rBuffer);
    			    }
			    }
			    break;
			    
            case 'D' :
            	pass->session->puts("\r\n\r\n Menu Name to Delete: \x1b[1;44m                    \x1b[20D");        
        		get_str(pass,rBuffer);
        		_mnu.menu_del(rBuffer);
        		break;

            case 'E' :
                pass->session->puts("\r\n\r\n Menu Name: \x1b[1;44m                    \x1b[20D");
        		get_str(pass,rBuffer);
       			if (!_mnu.menu_match(rBuffer)) {  // Make sure menu is a match
	        		pass->session->printf("\r\n %s doesn't exist!", rBuffer);
   	    			Sleep(1000);        		
    			}
    			else _mnued.menu_edit(rBuffer);
    			break;


            case 'Q' :
                return;
                
            default  :
                break;
             
        }
	}
}

