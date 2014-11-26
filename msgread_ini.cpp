
#include <stdio.h>
#include <string>
#include <fstream>

using namespace std;

#include "struct.h"
#include "msgread_ini.h"

bool msgread_ini::msg_exists() {

    Top = 0;
    Bot = 0;

    std::string path = BBSPATH;
    path += "msg.ini";

    FILE *stream;
    stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) { return false; }
    fclose(stream);
    return true;
}

void msgread_ini::msg_create() {

    std::string name = BBSPATH;
    name += "msg.ini";

    ofstream outStream2;
    outStream2.open( name.c_str(), ofstream::out | ofstream::trunc );
    if (!outStream2.is_open()) {
        printf( "\nError Creating: %s \n", name.c_str());
        return;
    }

    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | " << VERSION << "           |" << endl;
    outStream2 << "# | Message Reader Text Box Coords                                 |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | Note : This file is regenerated with defaults if missing.      |" << endl;
    outStream2 << "# | Note : Any Lines starting with # will be ignored!              |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << endl;
    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | Set the TOP Line # for the Top Margin                          |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "set TOP \"5\"" << endl;
    outStream2 << endl;
    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | Set the BOTTOM Line # for the Bottom Margin                    |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "set BOT \"22\"" << endl;
    outStream2 << endl;
    outStream2.close();
    return;
}

void msgread_ini::msg_check(std::string cfgdata) {

    std::string temp = cfgdata;
    int id1 = -1;

    // Disgards any Config lines with the # Character
    if (temp[0] == '#') return;

    // Sets if LOGGING is on / off
    id1 = -1;
    id1 = temp.find("set TOP", 0);
    if (id1 != -1) {
        std::string temp1;
        int st1 = -1;
        int st2 = -1;
        signed int  ct = -1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        id1 = atoi(temp1.c_str());
        Top = id1;
    }

    // Sets Download Path
    id1 = -1;
    id1 = temp.find("set BOT", 0);
    if (id1 != -1) {
       std::string temp1;
        int st1 = -1;
        int st2 = -1;
        signed int  ct = -1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        id1 = atoi(temp1.c_str());
        Bot = id1;
    }
}

void msgread_ini::msg_parse() {

    std::string name = BBSPATH;
    name += "msg.ini";

    ifstream inStream;
    inStream.open( name.c_str() );
    if (!inStream.is_open()) {
        printf("Couldn't Open Config File: %s\n", name.c_str());
        return;
    }

    std::string cfgdata;
    for (;;) {
        getline(inStream,cfgdata);
        msg_check(cfgdata);
        if(inStream.eof()) break;
    }
    inStream.close();
    return;
}
