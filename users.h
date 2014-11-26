#ifndef USERS_H
#define USERS_H


#include "struct.h"

typedef struct users {
    
    // User Index File for Faster Read / Writes  
    int  idx_writet(UserIdx *usr, int idx);
    int  idx_write(UserIdx *usr, int idx);
    int  idx_read(UserIdx *usr, int idx);
    int  idx_count();
    int  idx_find(char *name);
    bool idx_match(char *name);
    void idx_new(char *name, int idx);
    
    // User File
    int  users_writet(UserRec *usr, int idx);
    int  users_write(UserRec *usr, int idx);
    int  users_read(UserRec *usr, int idx);
    
}users;




#endif
