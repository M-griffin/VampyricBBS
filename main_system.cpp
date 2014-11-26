
#include <stdio.h>
#include <string>

#include "struct.h"
#include "main_system.h"

#include "ansiout.h"
#include "misc.h"

#include "menu_func.h"
//#include "users.h"


/*
    This is the Main Global Entry Point for the BBS
    All System Information per Session will be kept 
    and Passed along from this point .. .
    
    User Info, BBS Configuration Info.. etc..
    This is Repensent at the global where everything
    is init'd and falls back to.
*/


main_system::main_system() {

    pass = new PASSING;
    user = new UserRec;
    uidx = new UserIdx;
}

main_system::~main_system() {

    delete pass;
    delete user;
    delete uidx;
}


void main_system::start(PASSING *passing) {

    pass = passing;
}

void main_system::proc() {

    // Display Welcome Screen
    ansiPrintf(pass, "welcome");
    
    // Screen Pause
    pipe2ansi(pass, "\r\n|11 %|03% |15p|07au|08se.. . |03%|11% |07");
    unsigned char c;
    int result;
    
    // Pause
    get_chr(pass,c);


    char startupmenu[10] = {"matrix"};
    strcpy(mnuf._curmenu,startupmenu);
            
    mnuf._loadnew = true;
    printf("\nloadnew: %i",mnuf._loadnew);
        
    // Initalize Menu System
    mnuf.start(pass,user);        
   
    while(pass->session->isActive()) {            
        
        char in[10];
        // Startup Menu System Here 
        
        printf("\nloadnew: %i",mnuf._loadnew);               
        if (mnuf._loadnew == true) {
            printf("\nReading in Menu: %s",mnuf._curmenu);
            mnuf.menu_readin();
        }            

        mnuf.menu_proc(in);        
        Sleep(10);
    }
}


void main_system::system_execute() {
   
    while (pass->session->isActive()) {        
        proc();
        Sleep(10);
    }
}

