
#include <stdio.h>
#include <string>
#include <fstream>

using namespace std;

#include "struct.h"
#include "config.h"


/****************************************************************************/
/* Checks if CONFIG.CFG file exists, if not return false                                        */
/****************************************************************************/
bool configdataexists()
{

    std::string path = BBSPATH;
    path += "config.ini";

    FILE *stream;
    stream = fopen ( path.c_str(),"rb+" );
    if ( stream == NULL )
    {
        return false;
    }
    fclose ( stream );
    return true;
}

/****************************************************************************/
/* Create Defauly Config File if None Exists                                                            */
/****************************************************************************/
void createconfig()
{

    std::string name = BBSPATH;
    name += "config.ini";

    ofstream outStream2;
    outStream2.open ( name.c_str(), ofstream::out | ofstream::trunc );
    if ( !outStream2.is_open() )
    {
        printf ( "\nError Creating: %s \n", name.c_str() );
        exit ( 1 );
        return;
    }

    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | " << VERSION << "           |" << endl;
    outStream2 << "# | Main System Configuration File                                 |" << endl;
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
    outStream2 << "# | Set this port for incomming telnet connections                 |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "set TELNET_PORT \"23\"" << endl;
    outStream2 << endl;
    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | Set the Max # Of Connections / Nodes for your BBS              |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "set TELNET_MAX \"4\"" << endl;
    outStream2 << endl;
    outStream2.close();
    return;
}

/****************************************************************************/
/* Parses Config File Data                                                                                                      */
/****************************************************************************/
void checkcfg ( std::string cfgdata )
{

    std::string temp = cfgdata;
    int id1 = -1;

    // Disgards any Config lines with the # Character
    if ( temp[0] == '#' ) return;

    // Sets if LOGGING is on / off
    id1 = -1;
    id1 = temp.find ( "Set LOG", 0 );
    if ( id1 != -1 )
    {
        std::string temp1;
        int st1 = -1;
        int st2 = -1;
        signed int  ct = -1;

        st1 = temp.find ( '"', 0 );
        st2 = temp.find ( '"', st1+1 );
        ++st1;
        temp1 = temp.substr ( st1,st2 );
        ct = st2 - st1;
        if ( temp1.length() > ct )
            temp1.erase ( ct,temp1.length() );
        id1 = atoi ( temp1.c_str() );

        //if ( id1 == 1) LOGGING = true;
        //else LOGGING = false;

    }

    // Sets Download Path
    id1 = -1;
    id1 = temp.find ( "Set DLPATH", 0 );
    if ( id1 != -1 )
    {
        std::string temp1;
        int st1 = -1;
        int st2 = -1;
        signed int  ct = -1;

        st1 = temp.find ( '"', 0 );
        st2 = temp.find ( '"', st1+1 );
        ++st1;
        temp1 = temp.substr ( st1,st2 );
        ct = st2 - st1;
        if ( temp1.length() > ct )
            temp1.erase ( ct,temp1.length() );
        if ( ( temp1 != "NONE" ) && ( temp1 != "none" ) )
        {
            if ( temp1 != "" )
            {
                // strcpy(DLPATH,temp1.c_str());
            }
        }
    }
}

/****************************************************************************/
/* Read Config File                                                                                                                     */
/****************************************************************************/
void parseconfig()
{

    std::string name = BBSPATH;
    name += "config.ini";

    ifstream inStream;
    inStream.open ( name.c_str() );
    if ( !inStream.is_open() )
    {
        printf ( "Coun't Open Config File For Reading %s\n", name.c_str() );
        exit ( 1 );
    }

    std::string cfgdata;
    for ( ;; )
    {
        getline ( inStream,cfgdata );
        checkcfg ( cfgdata );
        if ( inStream.eof() ) break;
    }
    inStream.close();
    return;
}
