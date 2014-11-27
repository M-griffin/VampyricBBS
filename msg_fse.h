#ifndef MSG_FSE_H
#define MSG_FSE_H

#include <string>
#include "struct.h"

class msg_fse
{

private:
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

    // Editor Variables
    int Row,       // Y - Row    [Lines]
        Col,       // X - Colume [Char in Line]
        Len,       // Line Length

        Top,       // Top Margin
        Bot,       // Bottom Margin

        Tot,       // Total # of Lines in Link List
        Num;       // Current Line Number in Link List

    // Holds String Data for Current Line in use
    std::string Line;   // Line Data
    std::string TLine;  // Temp Line Data

    bool OVERWRITE; // Do Replace or Insert

    int recvState;
    int inSubOpt;
    int charMode;

    int  TRow;      // Temp Row, that were Line Wrapping On
    bool cont;      // If continious wrappign is on or off

    char *buff;     // Message Tmp Buffer


public:
    msg_fse();
    void start ( PASSING *passing );

    // Link List Functions
    void add_to_list ( std::string );
    void save_all();
    bool move_up();
    bool move_down();
    void move_first();
    void move_last();
    int  line_count();
    int  line_total();
    void display_list();
    void delete_line_up();
    void delete_line();
    void dispose_list();
    void print_options();

    // FSE Functions
    void insert_line();
    void box_redraw();
    void word_wrapex();
    void word_wrap();
    void add_char ( unsigned char c );
    void up_arrow();
    void dn_arrow();
    void rt_arrow();
    void lt_arrow();
    void home_cursor();
    void end_cursor();
    void poll_chr ( char *buffer );

private:
    // fse.ini
    bool fse_exists();
    void fse_create();
    void fse_check ( std::string cfgdata );
    void fse_parse();



};


#endif
