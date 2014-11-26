#ifndef MSG_EDIT_H
#define MSG_EDIT_H

#include "struct.h"
#include "msgs.h"


class msg_edit {

    private: 
    PASSING   *pass;             
    msg_func  _mfunc; 
    
    public:  
       
    msg_edit();        
    void start(PASSING *passing);
    char *faddr2char(char *s,fidoaddr *fa);
    void mbeditmenu();  
    void kill_mbase(int mbnum);
    void delete_mbase();
    void poke_mbase(int pokenum);
    void init_mbase(mb_list_rec *mb);
    int  list_message_bases(int page);
    void insert_mbase();
    void edit_aka(mb_list_rec *mr);
    void ACS(ACSrec *acs, char *fstr);
    void mod_mbase(int mbnum);
    void modify_mbase();
    void swap_mbase(int iFrom, int iTo);
    void move_mbase();   

};


#endif

