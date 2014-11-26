
#include <stdio.h>
#include <string>

using namespace std;

#include "struct.h"
#include "menus.h"



int menus::menu_writet(MenuRec *menur, int idx) { 

	std::string path = DATAPATH;
    path += "menu.tmp";

    int x = 0;
	FILE *stream = fopen(path.c_str(),"rb+");
   	if(stream == NULL) {
	   	stream = fopen(path.c_str(), "wb");
    	if(stream == NULL) {
    		printf("Error menu_write temp!");
        	return x;
    	}
	}
    if(fseek(stream,(int)idx*sizeof(MenuRec),SEEK_SET)==0)
	    x = fwrite(menur,sizeof(MenuRec),1,stream);
    fclose(stream);
    return x;
}

int menus::menu_write(MenuRec *menur, int idx) { 

	std::string path = DATAPATH;
    path += "menu.dat";

    int x = 0;
	FILE *stream = fopen(path.c_str(),"rb+");
   	if(stream == NULL) {
	   	stream = fopen(path.c_str(), "wb");
    	if(stream == NULL) {
    		printf("Error menu_write!");
        	return x;
    	}
	}
    if(fseek(stream,(int)idx*sizeof(MenuRec),SEEK_SET)==0)
	    x = fwrite(menur,sizeof(MenuRec),1,stream);
    fclose(stream);
    return x;
}

int menus::menu_read(MenuRec *menur, int idx) {

    std::string path = DATAPATH;
    path += "menu.dat";
	
    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
    	stream=fopen(path.c_str(), "wb");
    	if(stream == NULL) {
       		printf("Error menu_read!");
      		return x;
    	}
	}
    fclose(stream);

	stream = fopen(path.c_str(), "rb");
    if(fseek(stream,(int)idx*sizeof(MenuRec),SEEK_SET)==0)
    	x = fread(menur,sizeof(MenuRec),1,stream);
    fclose(stream);
    return x;
}

int menus::menu_count() {

	int i = 0;
    MenuRec menur;

    while(menu_read(&menur,i++));
    if(i < 1)	i = -1;
    else i--;
    return(i);
}

int menus::menu_find(char *tfile) {    

	string temp1, temp2;
	MenuRec menur;
	temp1 = (tfile);
	
	int idx = 0;
	while(menu_read(&menur,idx)){
    	temp2 = (menur.MenuName);
        if(temp1 == temp2) return(idx);
        idx++;
    }
	return(-1);
}

bool menus::menu_match(char *name) {

    if ((menu_find(name)) == -1) return false;
    else return true;
}

int menus::cmds_write(char *menu, CommandRec *cmdr, int idx) { 

 	std::string path = MENUPATH;
    path += menu;
    path += ".mnu";

    int x = 0;
	FILE *stream = fopen(path.c_str(),"rb+");
   	if(stream == NULL) {
	   	stream = fopen(path.c_str(), "wb");
    	if(stream == NULL) {
    		printf("Error cmds_write %s.mnu.. .", menu);
        	return x;
    	}
    }
    if(fseek(stream,(int)idx*sizeof(CommandRec),SEEK_SET)==0)
	    x = fwrite(cmdr,sizeof(CommandRec),1,stream);
    fclose(stream);
    return x;
}

int menus::cmds_writet(char *menu, CommandRec *cmdr, int idx) { 

 	std::string path = MENUPATH;
    path += menu;
    path += ".tmp";

    int x = 0;
	FILE *stream = fopen(path.c_str(),"rb+");
   	if(stream == NULL) {
	   	stream = fopen(path.c_str(), "wb");
    	if(stream == NULL) {
    		printf("Error cmds_write %s.tmp.. .", menu);
        	return x;
    	}
    }
    if(fseek(stream,(int)idx*sizeof(CommandRec),SEEK_SET)==0)
	    x = fwrite(cmdr,sizeof(CommandRec),1,stream);
    fclose(stream);
    return x;
}

int menus::cmds_read(char *menu, CommandRec *cmdr, int idx) {

	std::string path = MENUPATH;
    path += menu;
    path += ".mnu";

    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
    	stream=fopen(path.c_str(), "wb");
    	if(stream == NULL) {
       		printf("Error cmds_read %s.mnu.. .", menu);
      		return x;
    	}
	}
    fclose(stream);

	stream = fopen(path.c_str(), "rb");
    if(fseek(stream,(int)idx*sizeof(CommandRec),SEEK_SET)==0)
    	x = fread(cmdr,sizeof(CommandRec),1,stream);
    fclose(stream);
    return x;
}

// Copy a menu command
//----------------------------------------------------------------------------
void menus::cmds_copy(char *menuname, int num, int many) {
    
	CommandRec cmdr;
	memset(&cmdr,0,sizeof(CommandRec));
	
	int idx = 0;
	int i   = 0;
    
    --num;    
    cmds_read(menuname, &cmdr, num);
    idx = cmds_count(menuname);    
    
    while(cmds_write(menuname, &cmdr, idx++)) {
        ++i;
        if (i == many) break;
	}		
}

// Move a menu command
//----------------------------------------------------------------------------
void menus::cmds_move(char *menuname, int num, int pos) {
    
	CommandRec cmdr;
	CommandRec cmdr2;
	
	memset(&cmdr,0,sizeof(CommandRec));
	memset(&cmdr2,0,sizeof(CommandRec));
	
	int idx = 0;
	int i   = 0;
    
    --num;
    --pos;
    
    // Read Command to Move
    cmds_read(menuname, &cmdr2, num);
    
    while(cmds_read(menuname, &cmdr, idx)) {
		
		if (idx != num) { // Skip Reading File to be Moved
            if (idx == pos) {
                cmds_writet(menuname, &cmdr2, i++);  // If New Position, Write Temp
                cmds_writet(menuname, &cmdr,  i++);  // Now move this command after new
            }
            else cmds_writet(menuname, &cmdr, i++);  // Else refill with normal
        }
		idx++;
	}
	
	// Remove menufile.mnu and copy over menufile.tmp
	std::string path = MENUPATH;
    path += menuname;
    path += ".mnu";
	remove(path.c_str());
	
    // rename tmp to dat		
    std::string path2 = MENUPATH;
    path2 += menuname; 
    path2 += ".tmp";    
      	
	rename(path2.c_str(),path.c_str());
	remove(path2.c_str());	
	
}

// Delete a menu command
//----------------------------------------------------------------------------
void menus::cmds_del(char *menuname, int num) {
    
	CommandRec cmdr;
	memset(&cmdr,0,sizeof(CommandRec));
	
	int idx = 0;
	int i   = 0;
    
    --num;
    while(cmds_read(menuname, &cmdr, idx)) {
		if (idx != num) cmds_writet(menuname, &cmdr, i++);
		idx++;
	}
	
	// Remove menufile.mnu and copy over menufile.tmp
	std::string path = MENUPATH;
    path += menuname;
    path += ".mnu";
	remove(path.c_str());
	
    // rename tmp to dat		
    std::string path2 = MENUPATH;
    path2 += menuname; 
    path2 += ".tmp";    
      	
	rename(path2.c_str(),path.c_str());
	remove(path2.c_str());	
	
}

void menus::cmds_create(char *menuname, int num) {

    CommandRec cmdr;
    memset(&cmdr,0,sizeof(CommandRec));
    	
	int idx;
	int i = 1;
	
	while (1) {	
	    memset(&cmdr,0,sizeof(CommandRec));	
        strcpy(cmdr.LDesc,    "New Menu Command");
        strcpy(cmdr.SDesc,    "[Q]uit to Main");
        strcpy(cmdr.CKeys,    "Q");
        strcpy(cmdr.Acs,      "s50");
        strcpy(cmdr.CmdKeys,  "-^");
        strcpy(cmdr.MString,  "main");
        strcpy(cmdr.HiString, "");
        strcpy(cmdr.LoString, "");
        cmdr.Xcoord           = 0; 
        cmdr.Ycoord           = 0;
        cmdr.LBarCmd          = false;
        cmdr.SText            = false;
        cmdr.STLen            = 0;

	    idx = cmds_count(menuname);
	    cmds_write(menuname,&cmdr,idx);	
	    if ( i == num ) break;
	    ++i;
    }
}

int menus::cmds_count(char *menu) {

	int i = 0;
    CommandRec cmdr;

    while(cmds_read(menu,&cmdr,i++));
    if(i < 1) i = -1;
    else i--;
    return(i);
}

// Delete's a Menu
//----------------------------------------------------------------------------
void menus::menu_del(char *menuname) {

	MenuRec menur;
	memset(&menur,0,sizeof(MenuRec));
	
	int idx = 0;
	int i   = 0;
    
    int rm = menu_find(menuname);	
	while(menu_read(&menur,idx)) {
		if (idx != rm) menu_writet(&menur,i++);
		idx++;
	}
	
	// Remove menu.dat and copy over menu.tmp
	std::string path = DATAPATH;
    path += "menu.dat";
	remove(path.c_str());
	
    // rename tmp to dat		
    std::string path2 = DATAPATH;
    path2 += "menu.tmp";   	
	rename(path2.c_str(),path.c_str());
	remove(path2.c_str());	
	
	// Remove Commands that go with this menu
    path = MENUPATH;
    path += menuname;
    path += ".mnu";
    remove(path.c_str()); 
}

// Creates a New Menu
//----------------------------------------------------------------------------
bool menus::menu_create(char *menuname) {

	MenuRec menur;			 	
	memset(&menur,0,sizeof(MenuRec));	
	
	int idx = menu_count();	

	// Main Menu Options
    strcpy(menur.MenuName,    menuname);
    strcpy(menur.Directive,   "MENUNAME");
    strcpy(menur.MenuPrompt,  "comamnd: ");
    strcpy(menur.Acs,         "s50");
    strcpy(menur.Password,    "");
    menur.ForceInput          = 0;
    menur.ForceHelpLevel      = 0;
    menur.Lightbar            = false;
    
	// Menu Flags
    menur.MFlags.ClrScrAfter  = false;
    menur.MFlags.ClrScrBefore = false; 
    menur.MFlags.ForcePause   = false;
    menur.MFlags.NoMenuPrompt = false;
    menur.MFlags.UseGlobal    = false;

	// Create a default command for this menu	
	menu_write(&menur,idx);
 	
	CommandRec cmdr;
	memset(&cmdr,0,sizeof(CommandRec));
	
    strcpy(cmdr.LDesc,    "New Menu Command");
    strcpy(cmdr.SDesc,    "[Q]uit to Main");
    strcpy(cmdr.CKeys,    "Q");
    strcpy(cmdr.Acs,      "s50");
    strcpy(cmdr.CmdKeys,  "-^");
    strcpy(cmdr.MString,  "main");
    strcpy(cmdr.HiString, "");
    strcpy(cmdr.LoString, "");
    cmdr.Xcoord           = 0; 
    cmdr.Ycoord           = 0;
    cmdr.LBarCmd          = false;
    cmdr.SText            = false;
    cmdr.STLen            = 0;

	idx = cmds_count(menuname);
	cmds_write(menuname,&cmdr,idx);	
	
}


