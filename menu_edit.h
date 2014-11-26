#ifndef MENU_EDIT_H
#define MENU_EDIT_H

#include "struct.h"
#include "menus.h"

class menu_editor {

    private:   
    
    PASSING *pass;         
    menus   _mnu;      

    public:          

    menu_editor();        
    void start(PASSING *passing);    
       
    void cmds_list(char *menuname);
    void cmds_edit(char *menuname, int num);
    void cmds_editor(char *menuname);
    
    void menu_list();
    void menu_edit(char *menuname);

};

void menu_edit(PASSING *pass);

#endif
