#ifndef MENUS_H
#define MENUS_H

#include "struct.h"

using namespace std;

// Menu Editor System & Menu Readin
//----------------------------------------------------------------------------

typedef struct menus
{

    // Menu and Cmds Read and write Functions
    int  menu_writet ( MenuRec *menur, int idx );
    int  menu_write ( MenuRec *menur, int idx );
    int  menu_read ( MenuRec *menur, int idx );
    int  menu_count();
    int  menu_find ( char *tfile );

    int  cmds_writet ( char *menu, CommandRec *cmdr, int idx );
    int  cmds_write ( char *menu, CommandRec *cmdr, int idx );
    int  cmds_read ( char *menu, CommandRec *cmdr, int idx );
    void cmds_copy ( char *menuname, int num, int many );
    void cmds_move ( char *menuname, int num, int pos );
    void cmds_del ( char *menuname, int num );
    void cmds_create ( char *menuname, int num );
    int  cmds_count ( char *menu );

    void menu_del ( char *menuname );
    bool menu_create ( char *menuname );
    bool menu_match ( char *name );


} menus;



#endif
