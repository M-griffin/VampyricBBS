
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <fstream>

using namespace std;

#include "language.h"
#include "struct.h"

using namespace std;


void language::lang_remove()
{

    std::string path = DATAPATH;
    path += "language.dat";
    remove ( path.c_str() );

}

int language::lang_write ( LangRec *lang, int idx )
{

    std::string path = DATAPATH;
    path += "language.dat";

    int x = 0;
    FILE *stream = fopen ( path.c_str(),"rb+" );
    if ( stream == NULL )
    {
        stream = fopen ( path.c_str(), "wb" );
        if ( stream == NULL )
        {
            printf ( "Error language_write!" );
            return x;
        }
    }
    if ( fseek ( stream, ( int ) idx*sizeof ( LangRec ),SEEK_SET ) ==0 )
        x = fwrite ( lang,sizeof ( LangRec ),1,stream );
    fclose ( stream );
    return x;
}

int language::lang_read ( LangRec *lang, int idx )
{

    std::string path = DATAPATH;
    path += "language.dat";

    int x = 0;
    FILE *stream = fopen ( path.c_str(),"rb+" );
    if ( stream == NULL )
    {
        stream=fopen ( path.c_str(), "wb" );
        if ( stream == NULL )
        {
            printf ( "Error language_read!" );
            return x;
        }
    }
    fclose ( stream );

    stream = fopen ( path.c_str(), "rb" );
    if ( fseek ( stream, ( int ) idx*sizeof ( LangRec ),SEEK_SET ) ==0 )
        x = fread ( lang,sizeof ( LangRec ),1,stream );
    fclose ( stream );
    return x;
}

void language::lang_get ( char *lang, int idx )
{

    LangRec l1;
    --idx;
    lang_read ( &l1,idx );
    strcpy ( lang,l1.Lang );

    //should do MCI Code Parsing and if file, display it right away

}


void language::lang_check ( std::string lang )
{

    std::string temp  = lang;
    std::string temp2 = "";
    int id1 = -1;

    // Disgards any Config lines starting with the # or ; Character
    if ( temp[0] == '#' || temp[0] == ';' ) return;

    unsigned char c;
    char Num[5]= {0};
    int LangNum;
    int cnt = 0;

    // Parse first 3/4 Digits for Language string Number
    for ( int i = 0; i !=4; i++ )
    {
        if ( isdigit ( temp[cnt] ) )
        {
            Num[cnt] = temp[cnt];
            ++cnt;
        }
    }

    LangNum = atoi ( Num );
    --LangNum;

    LangRec l1;
    if ( temp.size() > 4 )
    {
        temp2 = temp.substr ( 4, temp.size() );
        strcpy ( l1.Lang,temp2.c_str() );
        lang_write ( &l1,LangNum );
    }

}

void language::lang_compile()
{

    std::string name = BBSPATH;
    name += "language.txt";

    ifstream inStream;
    inStream.open ( name.c_str() );
    if ( !inStream.is_open() )
    {
        printf ( "Couldn't Read language.txt" );
        return;
    }
    lang_remove();

    std::string cfgdata;
    for ( ;; )
    {
        getline ( inStream,cfgdata );
        lang_check ( cfgdata );
        if ( inStream.eof() ) break;
    }
    inStream.close();
    return;
}
