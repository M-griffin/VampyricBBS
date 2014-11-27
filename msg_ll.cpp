

#include <string>
#include "struct.h"
#include "msg_ll.h"
#include "ansiout.h"
#include "misc.h"

msg_ll::msg_ll()
{


    // Initalize Link List Class Defaults
    head = new LineRec;
    head->data = "";
    head->up_link  = head;
    head->dn_link  = head;
    current_node   = head;

    Row   = 1;
    Top   = 1;
    Bot   = 1;
    Tot   = 1;
    Num   = 1;
    Lines = 0;
    Page  = 1;
    TotPages = 0;

}

void msg_ll::start ( PASSING *passing )
{

    pass = passing;

}


// Testing Only
void msg_ll::display_list ( void )
{

    unsigned char c;

    LineRec *print;
    long int i = 0;

    if ( head->dn_link == head )
    {
        // Empty
        return;
    }
    else
    {
        print = head->dn_link;
        while ( print != head )
        {
            printf ( "\n%s",print->data.c_str() );
            print = print->dn_link;
            ++i;
        }
        get_chr ( pass,c );
    }
}

// FS READER Link List functions

// delete current line and Move up (for Backspaces)
void msg_ll::delete_line_up()
{

    LineRec *tmp;
    int Total = line_total();

    if ( head->up_link == head )
    {
        // List is Empty, Should never get here
        // Default is Always One, and Keep Cleared in Init
        return;
    }

    if ( Total > 1 && Num != 1 ) // Make sure there is more then 1 Line in the List
    {
        tmp = current_node;      // And Not At top of list, don't delete lines below
        current_node = current_node->dn_link;
        current_node->up_link = tmp->up_link;
        tmp->up_link->dn_link = current_node;
        delete tmp;

        if ( move_up() )
        {
            // No Lines Below this, move to top line
            //Line = current_node->data;
            //Col  = 1;
        }
    }
    else    // This is the First Entry, just Clear
    {
        current_node->data = "";
        //Line = "";
        //Col = 1;
        Row = 1;
        Num = 1;
        Tot = 1;
    }

}


// Add Line to Link List
void msg_ll::add_to_list ( std::string add_data )
{

    LineRec *add = new LineRec;

    add->data = add_data;
    add->up_link = current_node;
    add->dn_link = current_node->dn_link;
    current_node->dn_link->up_link = add;
    current_node->dn_link = add;
    current_node = add;
}


//Move up 1 from current to prev in list
bool msg_ll::move_up()
{

    if ( head->up_link == head )
    {
        // Empty
        return false;
    }
    else
    {
        // Make Sure Link List Stays at current if there is none!
        if ( current_node->up_link == head )
        {
            return false;
        }
        else current_node = current_node->up_link;
        return true;
    }
}


// move down 1 from current to next
bool msg_ll::move_down()
{

    if ( head->dn_link == head )
    {
        // Empty
        return false;
    }
    else
    {
        // Make Sure Link List Stays at current if there is none!
        if ( current_node->dn_link == head ) return false;
        else current_node = current_node->dn_link;
        return true;

    }
}

// Count how many lines in list from current all the way down
int msg_ll::line_count()
{

    int count = 0;
    LineRec *lineTmp;

    if ( head->dn_link == head )
    {
        // Empty
        return 0;
    }
    else
    {
        // Make Sure Link List Stays at current if there is none!
        lineTmp = current_node;
        if ( lineTmp->dn_link == head ) return 0;
        else
        {
            // Count how many Lines from Current to Bottom
            while ( lineTmp->dn_link != head )
            {
                lineTmp = lineTmp->dn_link;
                ++count;
            }
        }
        return count;
    }
}

// Count how many lines in list from Top all the way down
int msg_ll::line_total()
{

    int cur   = line_count();
    int count = 0;
    LineRec *lineTmp;

    if ( head->dn_link == head )
    {
        // Empty
        return 0;
    }
    else
    {
        // Make Sure Link List Stays at current if there is none!
        lineTmp = head;
        if ( lineTmp->dn_link == head ) return 0;
        else
        {
            // Count how many Lines from Current to Bottom
            while ( lineTmp->dn_link != head )
            {
                lineTmp = lineTmp->dn_link;
                ++count;
            }
        }
        Tot = count;
        return count;
    }
}

// Remove all enteries in list
void msg_ll::dispose()
{

    int Total = line_total();

    // Delete All the Lines
    for ( ; Total != 0; Total-- )
    {
        Num = Total;
        delete_line_up();
    }
}

// Remove all enteries in list
void msg_ll::dispose_list()
{

    LineRec *tmp;
    current_node = head->dn_link;

    while ( current_node != head )
    {
        tmp = current_node;
        current_node = current_node->dn_link;
        delete tmp;
    }
    delete head;
}

void msg_ll::box_start()
{

    // Faster display to throw in 1 string and send all out at 1 time
    std::string _editbox = "";
    char capture[200]= {0};
    Page  = 1;

    // Get Message Box Size
    int boxsize = Bot - Top; // Fist Get Box Size

    // Check Total Lines in Message
    Tot = line_total();

    // Check Total Number of Pages
    TotPages = Tot / boxsize;
    ++TotPages;
    if ( Tot <= boxsize+1 ) TotPages = 1;
    printf ( "\nTotPages: %i",TotPages );

    // Set Current Node to First In List
    current_node = head->dn_link;

    // Now Grab as many lines as will fit in the box
    for ( int i = 1; i < boxsize+1; i++ )
    {
        sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str() );
        _editbox += capture;
        if ( !move_down() ) break;
    }

    // Reset to Current Cursor Position
    sprintf ( capture, "\x1b[%i;%iH", Row+Top-1, 1 );
    _editbox += capture;

    pipe2ansi ( pass, ( char * ) _editbox.c_str() );

}

void msg_ll::box_pgdn()
{

    // Faster display to throw in 1 string and send all out at 1 time
    std::string _editbox = "";
    char capture[200]= {0};

    // Make sure we can go down another page..
    int boxsize = Bot - Top;   // Get boxsize

    // Check Total Number of Pages
    // If On Last page Return
    if ( Page == TotPages ) return;
    ++Page;

    //Now clear the box First
    for ( int t = 0; t < boxsize+1; t++ )
    {
        sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top-1 ) +t, 1 );
        _editbox += capture;
    }

    // Set Current Node to First In List
    current_node = head->dn_link;

    // Now Move Down How Many Pages
    for ( int i = 1; i < ( boxsize* ( Page-1 ) ); i++ )
    {
        if ( !move_down() ) break;
    }

    // Now Grab as many lines as will fit in the box
    for ( int i = 1; i < boxsize+1; i++ )
    {
        sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str() );
        _editbox += capture;
        if ( !move_down() ) break;
    }

    // Display Message
    pipe2ansi ( pass, ( char * ) _editbox.c_str() );

}

void msg_ll::box_pgup()
{

    // Faster display to throw in 1 string and send all out at 1 time
    std::string _editbox = "";
    char capture[200]= {0};

    // Make sure we can go down another page..
    int boxsize = Bot - Top;   // Get boxsize

    // Check Total Number of Pages
    // If On Last page Return
    if ( Page == 1 ) return;
    --Page;

    //Now clear the box First
    for ( int t = 0; t < boxsize+1; t++ )
    {
        sprintf ( capture, "\x1b[%i;%iH\x1b[K", ( Top-1 ) +t, 1 );
        _editbox += capture;
    }

    // Set Current Node to First In List
    current_node = head->dn_link;

    if ( Page > 1 )
    {
        // Now Move Down How Many Pages
        for ( int i = 1; i < ( boxsize* ( Page-1 ) ); i++ )
        {
            if ( !move_down() ) break;
        }
    }

    // Now Grab as many lines as will fit in the box
    for ( int i = 1; i < boxsize+1; i++ )
    {
        sprintf ( capture, "\x1b[%i;%iH%s", Top+i-1, 1, current_node->data.c_str() );
        _editbox += capture;
        if ( !move_down() ) break;
    }

    // Display Message
    pipe2ansi ( pass, ( char * ) _editbox.c_str() );

}
