#ifndef MREAD_INI_H
#define MREAD_INI_H

#include <string>

typedef struct msgread_ini {

    int Top;
    int Bot;

    // Message Read .ini File
    bool msg_exists();
    void msg_create();
    void msg_check(std::string cfgdata);
    void msg_parse();

}msgread_ini;

#endif
