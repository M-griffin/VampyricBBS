#ifndef __CONFIG_H    /*  An extra safeguard to prevent this header from  */
#define __CONFIG_H    /*  being included twice in the same source file    */

#include <string>

using namespace std;

/*--------------------------------------------------------------------------------*/
// Checks if CONFIG.CFG file exists
bool configdataexists();

/*--------------------------------------------------------------------------------*/
// Creat Defauly Config File if None Exists
void createconfig();

/*--------------------------------------------------------------------------------*/
// Parses Config File Data
void checkcfg(std::string cfgdata);

/*--------------------------------------------------------------------------------*/
// Read Config File
void parseconfig();

#endif

