#ifndef MAIN_SYSTEM_H
#define MAIN_SYSTEM_H

#include "struct.h"
#include "users.h"
#include "menu_func.h"

/*
    This is the Main Global Entry Point for the BBS
    All System Information per Session will be kept
    and Passed along from this point .. .

    User Info, BBS Configuration Info.. etc..
    This is Repensent at the global where everything
    is init'd and falls back to.
*/


class main_system
{

private:
    UserRec     *user;    // Global User Data
    UserIdx     *uidx;    // Global Users Index File

    class
            menu_func    mnuf;    // Menu System

public:
    PASSING     *pass;    // Global Socket Session Socket Info

    main_system();
    ~main_system();
    void start ( PASSING *passing );
    void proc();
    void main_system::system_execute();

    // From here the bbs configuration is read, then
    // The Logon / Menu System is kicked in..


};


#endif
