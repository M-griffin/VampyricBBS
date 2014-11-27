#ifndef LOGON_H
#define LOGON_H

#include "struct.h"
#include "language.h"
#include "users.h"

class logon
{

protected:
    PASSING      *pass; // Handle to Session I/O

private:
    UserRec      *ur;   // Holdes Loaded User Rec
    users        _user; // Handle to Users Database
    language     _lang; // Handle to Language File

public:

    logon();
    void start ( PASSING *passing, UserRec *user );
    void login();

    void Handle();
    void Name();
    void Password();
    void BDay();
    void Sex();
    void Email();
    void EmailPriv();
    void Note();
    void Word();
    void application();

    void ParseVerify ( char *filename );
    void save_user();
    void verify_info();


};

#endif
