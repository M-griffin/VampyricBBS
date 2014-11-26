#ifndef MENU_FUNC_H
#define MENU_FUNC_H

#include "struct.h"
#include "menus.h"

class menu_func {

    private:    
    // This holds passed telnet session data
    // So Menu System can Read/Write Sockets    
    PASSING    *pass;    // Holds Socket/Session I/O        
    menus      _mnu;
                
    // Holds Loaded Menu Data
    // For loading and Processing Menu Commands on the Fly
    MenuRec    menur2;
    CommandRec cmdr2[MAX_MENU_COMMANDS];
    
    int  noc;          // Number of Commands in Loaded Menu
    
    public:
    
    UserRec  *usr;    
    
    menu_func();
    void start(PASSING *passing, UserRec *user);
    
    // Below here are Menu Processing Functions
    bool _loadnew;
    char _curmenu[20];  // Current Menu
    char _premenu[20];  // Current Menu    
    
    // Read in / Load Menu and Commands
    void menu_readin();    
    void menu_bars(char *inPut);
    void menu_proc(char *mString);
    
    // Initalizes the User Logon / Application Process
    void logon_system(unsigned char c);   // Init Logon Sequence / Application
    void msgedit_system();                // Init Message Base Editor
    void msgread_system(unsigned char c); // Init Message Reader System
    void msgpost_system(unsigned char c);
    // Menu Command Processing
    void menu_docmd(char *CmdKey, char *mString);

};


#endif
