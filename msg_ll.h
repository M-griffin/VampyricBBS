#ifndef MSG_LL_H
#define MSG_LL_H

#include <string>
#include "struct.h"


typedef struct msg_ll
{

    PASSING *pass;

    // Link List for Holding Each Line
    typedef struct LineRec
    {

        std::string  data;
        LineRec     *up_link;
        LineRec     *dn_link;

    } LineRec;

    // Main List for Holding All Data
    LineRec *head;
    LineRec *current_node;

    int Row;
    int Top;
    int Bot;
    int Tot;
    int Num;
    int Lines;
    int Page;
    int TotPages;

    msg_ll::msg_ll();
    void start ( PASSING *passing );
    void display_list ( void );
    void delete_line_up();
    void add_to_list ( std::string add_data );
    bool move_up();
    bool move_down();
    int  line_count();
    int  line_total();
    void dispose();
    void dispose_list();
    void box_start();
    void box_pgdn();
    void box_pgup();

} msg_ll;


#endif
