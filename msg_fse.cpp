/*

    Notes...      Current Things that need Fixing
    
    
    Shift PGUP/PGDN
    Console / Save, Clear, Abort, Quote   // Also make ESC Console Key!

*/

#include <stdio.h>
#include <string>
#include <fstream>

using namespace std;

#include "msg_fse.h"
#include "struct.h"
#include "ansiout.h"

#define READ_BUFF_SIZE        512
#define MAX_WIDTH  79

#define ST_DEFAULT            0     // receiving regular characters
#define ST_ESCAPE             1     // got an escape, looking for next character
#define ST_COMMAND            2     // got command, looking for command data

#define SP                    0x20
#define ESC                   '\x1b'
#define CMD_SE                0xF0
#define CMD_GA                0xF9
#define CMD_SB                0xFA
#define CMD_WILL              0xFB
#define CMD_WONT              0xFC
#define CMD_DO                0xFD
#define CMD_DONT              0xFE
#define CMD_IAC               0xFF

msg_fse::msg_fse() {

    // Initalize FSE Class Defaults
    head = new LineRec;
    head->data = "";
    head->up_link  = head;
    head->dn_link  = head;
    current_node   = head;
    
    // Setup Defaults
    Row = 0;    // Y - Row
    Col = 0;    // X - Colume
    Len = 0;    // Line Length
    
    // Just incase ini is not read, set defaults.    
    Top = 0;    // Top Margin
    Bot = 0;    // Bottom Margin
    Num = 0;    // Number of Lines in Link List
    Tot = 0;    // Total Number of Lines in Link List
    
    Line  = ""; // Current String of Chars
    TLine = ""; // Temp Line Data
    OVERWRITE = false;
    
    recvState = ST_DEFAULT;
    inSubOpt  = 0;
    charMode  = 0;
    
    TRow      = 0;      // Temp Row, that were Line Wrapping On
    cont      = false;  // If continious wrappign is on or off

}


void msg_fse::start(PASSING *passing) {

    pass = passing;

}


// Add Line to Link List
void msg_fse::add_to_list(std::string add_data) {

    LineRec *add = new LineRec;

    add->data = add_data;
    add->up_link = current_node;
    add->dn_link = current_node->dn_link;
    current_node->dn_link->up_link = add;
    current_node->dn_link = add;
    current_node = add;
}

// Copy message from link lists to buffer
void msg_fse::save_all() {
  
    int i = 1;
    if(head->dn_link == head) {
        // Empty
        return;
    }
    else {    
        // Make Sure Link List Stays at current if there is none!
        current_node = head->dn_link;
        while ( current_node->dn_link != head ) {
            current_node->data += '\r';
            strcat(buff,current_node->data.c_str());
            // Goto Next In List
            current_node = current_node->dn_link;         
        }                        
    }
}


//Move up 1 from current to prev in list
bool msg_fse::move_up() {

    if(head->up_link == head) {    
        // Empty
        return false;
    }
    else {    
        // Make Sure Link List Stays at current if there is none!
        if (current_node->up_link == head) { 
            return false;
        }
        else current_node = current_node->up_link;
        return true;
    }
}


// move down 1 from current to next
bool msg_fse::move_down() {

    if(head->dn_link == head) {
        // Empty
        return false;
    }
    else {    
        // Make Sure Link List Stays at current if there is none!
        if (current_node->dn_link == head) return false;
        else current_node = current_node->dn_link;
        return true;
        
    }
}


// Count how many lines in list from current all the way down
void msg_fse::move_first() {

    current_node = head;
    Num = 1;
}


// Count how many lines in list from current all the way down
void msg_fse::move_last() {
   
    LineRec *lineTmp;
    int Last = line_total();
    
    if(head->dn_link == head) {
        // Empty.. Re Init
        return;
    }
    else {    
        // Make Sure Link List Stays at current if there is none!
        lineTmp = current_node;
        if (lineTmp->dn_link == head) return;                
        else {
            // Move to Last Data Consontainer
            while ( lineTmp->dn_link != head ) {
                lineTmp = lineTmp->dn_link;
            }
            current_node = lineTmp;
        }
        Num = Last;
        Tot = Last;
        return;
    }
}


// Count how many lines in list from current all the way down
int msg_fse::line_count() {

    int count = 0;    
    LineRec *lineTmp;
    
    if(head->dn_link == head) {
        // Empty
        return 0;
    }
    else {    
        // Make Sure Link List Stays at current if there is none!
        lineTmp = current_node;
        if (lineTmp->dn_link == head) return 0;                
        else {
            // Count how many Lines from Current to Bottom
            while ( lineTmp->dn_link != head ) {
                lineTmp = lineTmp->dn_link;
                ++count;
            }
        }                
        return count;
    }
}

// Count how many lines in list from Top all the way down
int msg_fse::line_total() {

    int cur   = line_count();
    int count = 0;    
    LineRec *lineTmp;
    
    if(head->dn_link == head) {
        // Empty
        return 0;
    }
    else {    
        // Make Sure Link List Stays at current if there is none!
        lineTmp = head;
        if (lineTmp->dn_link == head) return 0;                
        else {
            // Count how many Lines from Current to Bottom
            while ( lineTmp->dn_link != head ) {
                lineTmp = lineTmp->dn_link;
                ++count;
            }
        }     
        Tot = count;
        return count;
    }
}

// delete current line and Move up (for Backspaces)
void msg_fse::delete_line_up() {
    
    LineRec *tmp;    
    int Total = line_total();

    if(head->up_link == head) {
        // List is Empty, Should never get here
        // Default is Always One, and Keep Cleared in Init
        return;
    }
    
    if (Total > 1 && Num != 1) { // Make sure there is more then 1 Line in the List
        tmp = current_node;      // And Not At top of list, don't delete lines below
        current_node = current_node->dn_link;
        current_node->up_link = tmp->up_link;
        tmp->up_link->dn_link = current_node;
        delete tmp;
      
        if (move_up()) {
            // No Lines Below this, move to top line
            Line = current_node->data;
            Col  = Line.size()+1;
        }
    }
    else {  // This is the First Entry, just Clear
        current_node->data = "";
        Line = "";
        Col = 1; 
        Row = 1;
        Num = 1;
        Tot = 1; 
    }

}

// Modified Delete Line, Deletes Current Line, then Moves Lower to Current
// when no more lower lines starts moving up..
void msg_fse::delete_line() {
    
    LineRec *tmp;    
    int Total = line_total();
    Tot = Total;
    int Cnt = line_count();

    if(head->up_link == head) {
        // List is Empty, Should never get here
        // Default is Always One, and Keep Cleared in Init
        return;
    }        
        
    if (Total > 1 && Num != 1) { // Make sure there is more then 1 Line in the List
        tmp = current_node;      // And Not At top of list, don't delete lines below
        current_node = current_node->dn_link;
        current_node->up_link = tmp->up_link;
        tmp->up_link->dn_link = current_node;
        delete tmp;
           
        if (Cnt != 0) {            
            // Move any Lower Lines to Current
            Line = current_node->data;
            //Col  = Line.size()+1;
            Col = 1;
        }
        else
            if (move_up()) {
            // No Lines Below this, move up a line now
            Line = current_node->data;
            //Col  = Line.size()+1;
            Col = 1;
            if (Row != 1) --Row;            
        }
        
    }
    // Were at the First Entry, just Clear, Then check if line below,
    // If we have a line below. copy second to first and delete second 
    else {
        // We have a line below, so move that line to the First Spot
        if (Total > 1) {
            move_down();
            Line = current_node->data;
           
            // Delete Second Line 
            tmp = current_node;
            current_node = current_node->dn_link;
            current_node->up_link = tmp->up_link;
            tmp->up_link->dn_link = current_node;
            delete tmp;
            
            // Move back to first line
            move_up();
            current_node->data = Line;
            Col = 1;
            //Col = Line.size()+1;                       
        }
        // This is the only line, just clear it
        else {                
            current_node->data = "";
            Line = "";
            Col = 1; 
            Row = 1;
            Num = 1;
            Tot = 1; 
        }
    }
}


// Testing Only
void msg_fse::display_list(void) {

    unsigned char c;

    LineRec *print;
    long int i = 0;
    
    if(head->dn_link == head) {
        // Empty
        return;
    }
    else {    
        print = head->dn_link;
        while(print != head) {
            print = print->dn_link;
            ++i;
        }
        //get_chr(pass,c);
    }    
}


// Remove all enteries in list
void msg_fse::dispose_list() {

    LineRec *tmp;     
    current_node = head->dn_link;
    while(current_node != head) {

        tmp = current_node;
        current_node = current_node->dn_link;
        delete tmp;
    }
    delete head;
}


// Insert line in middle of paragraph on screen
void msg_fse::insert_line() {

    current_node->data = Line;
    add_to_list(current_node->data);
    move_up();
    current_node->data = "";
    Line               = "";
    Col = 1;    
    // Redraw Screen Text inside of Input Box
    box_redraw();        
}

void msg_fse::box_redraw() {

    LineRec *cur;
    
    // Faster display to throw in 1 string and send all out at 1 time
    std::string _editbox = "";
    char capture[200]={0};
    
    // Check here if We are Scrolling all the lines in the Box up by 1
    if (Row == (Bot-Top+1)) {
        --Row; // Reset the Line Position
    }

    // Fist Get Box Size
    int boxsize = Bot - Top;    
    cur = current_node;    
            
    // Start at lines above current row to top of box
    int Cnt2 = Row;
    while (Cnt2 > 1) {
        if (!move_up()) break; 
        --Cnt2; 
    }
    
    
    //Now clear the box First
    for (int t = 0; t != boxsize+1; t++) {
        sprintf(capture, "\x1b[%i;%iH\x1b[K", (Top-1)+t, 1);
        _editbox += capture;
    }
       
    // Now Grab as many lines as will fit in the box
    int i = 0;
    for (i = 0; i < boxsize; i++) {
        sprintf(capture, "\x1b[%i;%iH%s", Top+i, 1, current_node->data.c_str());
        _editbox += capture;
        if (!move_down()) break;
    }
    // Reset Cursor and Record to Current Position
    current_node = cur;  
    
    // Reset to Current Cursor Position
    sprintf(capture, "\x1b[%i;%iH", Row+Top-1, Col);
    _editbox += capture;
    
    pass->session->printf("%s",_editbox.c_str());    
        
}

// Normal word wrapping
void msg_fse::word_wrap() {
    
    int N = 0;
    N = Line.find_last_of(" ");        
    
    // Cut out Last Word and move to next line
    if (N != -1) {
        // First cut out the word from the current line Buffer
        TLine = Line.substr(N+1,Line.size());
        Line.erase(N,N-Line.size());
        
        // Assign modified line to Current
        current_node->data = Line;

        // Create new Line that new word wrap text goes to
        add_to_list("");
        Line = TLine;
        current_node->data = Line;
        
        Col  = Line.size()+1;
        
        ++Row;  // Goto Next Row
        ++Num;  // Currrent Line is Next
        ++Tot;
        
        // Redraw Screen Text inside of Input Box
        box_redraw();
    }   
}


// Extended word wrap, for input mode line wrapping, pushing from the middle of a line
void msg_fse::word_wrapex() {

    // Check for End of Line Wraping
    int N;
    int TempCol;  
    
    printf("\nRow: %i, Col: %i, L-Size: %i",Row,Col, Line.size()); 
       
    // Current Line is > max, wra it to next line!
    if ((Line.size()) > MAX_WIDTH ) {
               
        // Wrap word at end of line to next line
        N = Line.find_last_of(" ");            
        // Cut out Last Word and move to next line
        printf("\nN = %i",N);
        
        // If the Cutting Mark is at the end of the line, erase it and start over
        if (N >= MAX_WIDTH || N == Line.size()) {
            Line.erase(N,1);  
            N = Line.find_last_of(" ");  
            printf("\nNew N = %i",N);    
        }
        
        
        if (N != -1) {           
            
            // First cut out the word from the current line Buffer
            TLine = Line.substr(N+1, Line.size());
            Line.erase(N+1, TLine.size());
            
            // Assign modified line to Current
            current_node->data = Line;
            if (Col > Line.size()+1) Col = Line.size()+1;
                                
            // For continious line wrapping
            if (cont == false) {
            
                // Were on The Current Line right here!!
                printf("\nStarting Fresh.. .");            
                cont = true;
            
                // Create new Line that new word wrap text goes to
                add_to_list("");
                ++Tot;
                
                // Assign Current / New Line the Cut Text
                current_node->data = TLine;            
            
                // Move back to Original
                move_up();                
                Line = current_node->data;     
                
                box_redraw();
                return;

            }
            // If Not Starting Fresh, Append... 
            else {
                printf("\nAppending.. .");                                                    
            
                // First move to line were appening to...
                move_down(); 
                
                // Check if Line were appending is full.. if so start a new one!
                if ( ( current_node->data.size() + TLine.size() ) >= MAX_WIDTH ) {
                    printf("\nFresh Append.. .");
                    move_up();
                    add_to_list("");
                    ++Tot;
                    current_node->data = TLine; 
                    move_up();
                    box_redraw();
                    return;
                
                }
                                
                // This line has room, append to it
                current_node->data = TLine + " " + current_node->data;
                
                // Move back to Original
                move_up();                
                Line = current_node->data;                        
                                                            
                box_redraw();
                return;                
            }
        } 
        else {
            printf("\n Ex-WW - No Spaces Found");
            
            // check if Line Below this one, If not Create New        
            // Chop off from cursor to end of line, and move it to the next line!
            TempCol = Line.size();
            TLine   = Line.substr(Col-1, TempCol); 
            Line.erase(Col-1,(Col-1)-TempCol);
            
            // Reset Current Line Data to New Modified Line
            current_node->data = Line;
            // Create And move text from line to new Line
            add_to_list("");
            ++Tot;
            
            current_node->data = TLine;
            // Move back to Previous line, and end of line
            move_up();
            
            Line = current_node->data;
            Col  = Line.size()+1;
            
            // Redraw Screen Text inside of Input Box
            box_redraw();
            return;
        }
               
    }
    else {
        // Erase Current Line and Redraw If no words are wrapped.. 
        pass->session->printf("\x1b[%i;%iH\x1b[K%s\x1b[%i;%iH", Row+Top-1, 1, Line.c_str(),Row+Top-1, Col); 
        return;
    }  
}

/*

// Extended word wrap, for input mode line wrapping, pushing from the middle of a line
void msg_fse::word_wrapex() {

    // Check for End of Line Wraping
    int N;
    int TempCol;   
    
    // first check if cursor is out of bounds...
    if (Col > MAX_WIDTH) {
        printf(" Extended Word Wrapped");
        word_wrap();
        return;               
    }    
    
    // Current Line is > max, wra it to next line!
    if ((Line.size()+1) > MAX_WIDTH ) {
    
        printf("\n> Max_Width LineS: %i", Line.size());
           
        // Wrap word at end of line to next line
        N = Line.find_last_of(" ");            
        // Cut out Last Word and move to next line
        if (N != -1) {           
            
            // First cut out the word from the current line Buffer
            TLine = Line.substr(N+1, Line.size());
            Line.erase(N, TLine.size());
            
            // Assign modified line to Current
            current_node->data = Line;
                                
            // For continious line wrapping
            if (cont == false) {
            
                // Were on The Current Line right here!!
                printf("\nStarting Fresh.. .");            
                cont = true;
            
                // Create new Line that new word wrap text goes to
                add_to_list("");
                ++Tot;
                
                // Assign Current / New Line the Cut Text
                current_node->data = TLine;            
            
                // Move back to Original
                move_up();                
                Line = current_node->data;     
                
                box_redraw();
                return;

            }
            // If Not Starting Fresh, Append... 
            else {
                printf("\nAppending.. .");                                                    
            
                // First move to line were appening to...
                move_down();                
                
                if (current_node->data.size() + TLine.size() > MAX_WIDTH) {
                    printf("\n> MAX_WIDTH.. .");
                    
                    move_up();
                    // Create new Line that new word wrap text goes to
                    add_to_list("");
                    ++Tot;
                
                    // Assign Current / New Line the Cut Text
                    current_node->data = TLine;            
            
                    // Move back to Original
                    move_up();
                    Line = current_node->data;     
                
                    box_redraw();
                    return;
                }
                
                // This line has room, append to it
                current_node->data = TLine + " " + current_node->data;
                
                // Move back to Original
                move_up();                
                Line = current_node->data;                        
                                                            
                box_redraw();
                return;                
            }
        }
        else {
            printf("\n WW - No Spaces Found");
            
            // check if Line Below this one, If not Create New        
            // Chop off from cursor to end of line, and move it to the next line!
            TempCol = Line.size();
            TLine   = Line.substr(Col-1, TempCol); 
            Line.erase(Col-1,(Col-1)-TempCol);
            
            // Reset Current Line Data to New Modified Line
            current_node->data = Line;
            // Create And move text from line to new Line
            Line = TLine;
            add_to_list("");
            ++Tot;
            
            current_node->data = Line;
            // Move back to Previous line, and end of line
            move_up();
            
            Line = current_node->data;
            Col  = Line.size()+1;
            
            // Redraw Screen Text inside of Input Box
            box_redraw();
            return;
        }
        
    }
    else {
        // Erase Current Line and Redraw If no words are wrapped.. 
        pass->session->printf("\x1b[%i;%iH\x1b[K%s\x1b[%i;%iH", Row+Top-1, 1, Line.c_str(),Row+Top-1, Col); 
        return;
    }  
} */


// Add char to current line, and test what mode were in
void msg_fse::add_char(unsigned char c) {

    std::string CharTemp;
    CharTemp = c;
    TLine    = "";
    
    bool _insert = false;
    int N = 0;
    
    // 1st Check if char Position is in the Middle of the Line
    // Replace / Insert The New Char in String  
    if ((int)Line.size() != (Col-1)) {
            
        if (OVERWRITE) {  // Overwrite char Current Char in Col Position
            TLine   = Line.erase(Col-1, 1);
            Line    = TLine.insert(Col-1, CharTemp);
        }
        else {            // just insert char at Col Position   
            // Need to add check for End of Line chars, and word wrap continiously down
            TLine   = Line.insert(Col-1, CharTemp);
            Line    = TLine;
            _insert = true;
        }
    }
    else {
        cont = false;
        // Normal Text.. add to end of line
        if ((Line.size()) < MAX_WIDTH) { // Make Sure Line isn't Full!
            Line += c;
        }
        else {
            // Make a new Line and Move this char to it!
            add_to_list("");             // Create Next Line
            Col  = 2;
            Line = c;           
            ++Row;  // Goto Next Row
            ++Num;  // Currrent Line is Next  
            ++Tot;                      
            
            current_node->data = Line;
            // Redraw Screen Text inside of Input Box
            box_redraw();
            return;
        }
    }       
    ++Col;
    
    // If Insert (meaning in the middle of a line) Word Wrap Extended
    if (_insert) { 
        word_wrapex();
        return;
    }
        
    // Then check if normal typing Word Wrap
    if (Col > MAX_WIDTH) {      // If Col is over Max
        cont = false;
        // Word wrap last word if less then 20 chars at end of line
        N = Line.find_last_of(" ");        
        if (N > (MAX_WIDTH-20)) {   // Check if Wrapping word / Chars
            word_wrap(); // move the word to the next line
        }
        else {           // Just move to next line and continue text 
        
            current_node->data = Line;   // Add Current Line to Current Node
            add_to_list("");             // Create Next Line
            Col  = 1;
            Line = "";                
            ++Row;  // Goto Next Row
            ++Num;  // Currrent Line is Next
            ++Tot;                       
            
            // Redraw Screen Text inside of Input Box
            box_redraw();
        }    
    }        
}


void msg_fse::up_arrow() {       
        
    int N = line_count();  
    current_node->data = Line;            
        
    if (move_up()) { 
        Line = current_node->data;
        if (Col > 1) Col  = Line.size()+1;
        else Col = 1;
    }
    
    if (Row > 1) --Row;        
    if (Num > 1) --Num;

    if (Row != 1) {
        pass->session->printf("\x1b[%i;%iH", Row+Top-1, Col);
    }
    else box_redraw();
}


void msg_fse::dn_arrow() {

    int N = line_count();   
    if (N == 0) return;    
    current_node->data = Line;   
                 
    if (Row != (Bot-Top)) {
        ++Row;
    }
    ++Num;
    
    if (move_down()) { 
        Line = current_node->data;
        if (Col > 1) Col  = Line.size()+1;
        else Col = 1;
    }
    Line = current_node->data;
    
    if (Row != (Bot-Top)) {
        pass->session->printf("\x1b[%i;%iH", Row+Top-1, Col);
    }
    else box_redraw();
}


void msg_fse::rt_arrow() {

    if (Col < MAX_WIDTH) { // Not at End of Line                              
        // Don't Let Arror Go past the lines length        
        if (Col > Line.size()) return;
        else ++Col;        
        pass->session->puts("\x1b[C");        
    }
    else if (Col > MAX_WIDTH) { // at End of Line ?
        --Col;
    }

}
void msg_fse::lt_arrow() {

    if (Col != 1) { // Not at Begining of Line                
        pass->session->puts("\x1b[D");
        --Col;         
    }
}

void msg_fse::home_cursor() {

    Col = 1;              
    pass->session->printf("\x1b[%i;%iH", Row+Top-1, Col);
}

void msg_fse::end_cursor() {

    Col = Line.size()+1;
    pass->session->printf("\x1b[%i;%iH", Row+Top-1, Col);
}


void msg_fse::poll_chr(char *buffer) {

    // This is a Broke Off Polling System for the Full Screen Editor,
    // This will provide faster buffered input then calling the normal
    // Socket's polling with it's extras..

    // Attempt to read some data from the zocket, without blocking.
    char readBuffer[READ_BUFF_SIZE];
    
    // Socket Polling Variables   
    int len;
    int count;
    int inCursor = 0;
    int index = 0;
    char tmpBuf[20]; // Hold Escape Sequences
        
    // Get Top and Bottom Coords for FSE Input Box 
    if (!fse_exists()) fse_create();
    fse_parse();
    
    // Assign Temp Buffer
    buff = buffer;
    
    // Init Full Screen Editor + Display Header
    ansiPrintf(pass, "fsehead");
    // Set to Normal Text Color Later Read BBS Config for Text Colors
    pass->session->puts("\x1b[0m");
    
    // Setup inital first line here!
    add_to_list("");        // Start off with inital Blank Line!
    move_up();
    
    // Char Function Variables    
    bool COK   = false;     // Message Editing Finishied
    OVERWRITE  = false;     // Default to Insert Mode
    Row = 1;                // Default Row
    Col = 1;                // Default Colume
    Num = 1;                // Current Line Number in List
    Tot = 1;                // Total Lines      
    unsigned char c;        // Holds Char Input
    
    int TCol = 0;           // Temp Colume        
    
    // Setup Line and Cursor Position top of Input Box
    pass->session->printf("\x1b[%i;%iH", Top, Col);
        
    while (pass->session->isActive()) {
    
        memset(&readBuffer,0,READ_BUFF_SIZE);
        inCursor = 0;
        index    = 0;        
        len      = 0;        
        
        // This is the Input Buffer Loop, sleep for 10 MS while no Data Avial
        do {            
            len = pass->session->z->read( readBuffer, READ_BUFF_SIZE, 0 );
            if( len == -1 ) {
                pass->session->stop();
                return;
            }
            Sleep(15);
        }
        while (len == 0);
        count = len;

        // Here is the Char Breakdown loop
        while( count-- ) {
        
            c = readBuffer[index];
            COK = false;

            // Handle either as a command or a raw characters
            switch( recvState ) {
                case ST_DEFAULT:
                    if( c == CMD_IAC ) {
                        // Escaped character, change state
                        recvState = ST_ESCAPE;
                    }
                    else {
                        if( inSubOpt ) {
                            // Add the character to the subOpt buffer
                        }
                        else {
                            if( charMode ) {
                            }
                            else {
                                // Catch all Input Escape Key Sequences from user
                                if (c == ESC) {                            
                                    // Capture Arror Key ESC Codes                                 
                                    // If ESC KEy found Capute to Global ESC Bufer so 
                                    // Function can pull it in and test it..                                         
                                    strcpy(pass->session->EscapeKey,"");
                                    pass->session->EscapeKey[0] = readBuffer[index];    // char 27
                                    pass->session->EscapeKey[1] = readBuffer[++index];  // char [
                                    pass->session->EscapeKey[2] = readBuffer[++index];  // char Key                                                                
                                    
                                    COK = true;
                                    break;
                                }   
                                else if( c == '\n' || c == '\0' ) {
                                    // Swallow newlines in the input,
                                    // so that Microsoft's POS telnet client will look right.
                                    // For some reason, I was getting '\0' characters
                                    // being sent down the line. Swallow them too.
                                    break;
                                }
                                else {
                                    // Rest are Normal Chars
                                    COK = true;
                                    break;
                                }
                            }
                        }
                    }
                    break;
                
                // Parse out Telnet IAC = Starting of Commands
                case ST_ESCAPE:
                    if( c == CMD_IAC ) {
                        // This was not a command, but simply an escaped
                        // CMD_IAC character. Store a CMD_IAC in the in buffer and
                        // return to the default state.
                        if( inSubOpt ) {
                            // Add the character to the subOpt buffer
                        }
                        else {
                            //inBuffer[inCursor++] = c;
                        }
                        recvState = ST_DEFAULT;
                    }
                    else {
                        // This is a command sequence, store it and change the state.
                        //cmdId = c;
                        recvState = ST_COMMAND;
                    }
                    break;

                // Parse out Command Received from Telnet Client
                case ST_COMMAND: {
                    // The last part of a command sequence
                    switch( c ) {
                        case CMD_WILL:
                        case CMD_WONT:
                        case CMD_DO:
                        case CMD_DONT:
                            // A real telnet server would send the correct response here.
                            // We, however, choose to send nothing for now.
                            break;
    
                        case CMD_SB:
                            // Set sub opt mode
                            inSubOpt = 1;
                            break;

                        case CMD_SE:
                            // Swallow the next character, and reset sub opt mode
                            inSubOpt = 0;
                            break;

                        default:
                            // Fall through
                            break;
                    }
                    recvState = ST_DEFAULT;
                }
                break;
            }
            // Process the next character in readin buffer
            ++index;
            
            // Process Each received char Input Here
            if (COK) {
            
                printf("\nRow: %i, Col: %i, Tot: %i",Row,Col, line_total());
                
                // Do Delete
                if ((int)c == 127) {               
                    if (Col != Line.size()+1) {
                        Line.erase(Col-1,1);
                        current_node->data = Line;
                        // Delete Current Line then redraw it
                        pass->session->printf("\x1b[%i;%iH\x1b[K%s\x1b[%i;%iH", Row+Top-1, 1,Line.c_str(), Row+Top-1, Col);
                    }                         
                }
                // Do Destructive Backspace
                if (c == 0x08) {
                
                    // First do Normal Destructive Backspace..  Normal from end of Line!
                    if (Col != 1 ) {                        
                        TLine = Line.erase(Col-2, 1);
                        Line  = TLine;
                        current_node->data = Line;
                        // Delete Current Line only, and then Redraw it!
                        --Col;         
                        pass->session->printf("\x1b[%i;%iH\x1b[K%s\x1b[%i;%iH", Row+Top-1, 1,Line.c_str(), Row+Top-1, Col);
                    }                    
                    else {
                        // Were in Insert Mode, Made sure were not on the First Row!
                        if (Num != 1) { // if 1st Row, Do Nothing!!!                        
                            // Check if in Inset Mode, if So move text to upper line
                            if (Line.size()+1 > Col) {   // Were in Insert Mode
                                // Get Data From Both lines to compare sizes
                                current_node->data = Line;
                                move_up();
                                // Line Above..
                                TLine = current_node->data;
                                TCol  = TLine.size()+1;
                                move_down();
                                // Current Line
                                Line  = current_node->data;

                                // First Check if Upper TLine has room on it
                                // If TLine has no room, then don't do anything with Line!
                                if ((TLine.size()) < MAX_WIDTH) {
                                    
                                    // Now check if Full "Line" Can fit in TLine
                                    if (TLine.size()+Line.size() < MAX_WIDTH) { 
                                        // Move Previoues like and cat it to current line
                                        TLine += Line;
                                        move_up();
                                        current_node->data = TLine;
                                        move_down();
                                        delete_line_up();
                                        if ( Tot > 1) --Tot;
                                        Col = TCol;
                                        box_redraw();
                                    }
                                    else {
                                        // Full Line Can't Fit, Try to put as much as possiable
                                        Len = (MAX_WIDTH - TLine.size());
                                        TLine += Line.substr(0, Len);
                                        Line.erase(0, Len);                                        
                                        move_up();
                                        current_node->data = TLine;
                                        move_down();
                                        current_node->data = Line;
                                        Col = TCol;
                                        box_redraw();                                                                    
                                    }
                                }                                                           
                            }
                            else 
                                delete_line_up();
                                if (Row > 1) --Row;
                                if (Num > 1) --Num;
                                if (Tot > 1) --Tot;
                                box_redraw();
                        }
                    }                                        
                }
                // Escaped Keys.. Arrow Keys etc..        
                else if ((int)c == 27 ) {
                    switch (pass->session->EscapeKey[2]) {
                        case 'A' : // Up Arrow                            
                            up_arrow();
                            strcpy(pass->session->EscapeKey,"");
                            break;
                            
                        case 'B' : // Down Arrorw
                            dn_arrow();
                            strcpy(pass->session->EscapeKey,"");
                            break;
                            
                        case 'C' : // Right Arrow
                            rt_arrow();
                            strcpy(pass->session->EscapeKey,"");
                            break;
                            
                        case 'D' : // Left Arror                
                            lt_arrow();
                            strcpy(pass->session->EscapeKey,"");
                            break;
                            
                        case 'H' : // Home Cursor
                            home_cursor();
                            strcpy(pass->session->EscapeKey,"");
                            break;
                            
                        case '4' : // End Cursor
                            end_cursor();
                            strcpy(pass->session->EscapeKey,"");
                            break;
                        
                        case '5' : // Shift PageUP
                            strcpy(pass->session->EscapeKey,"");
                            break;
                        
                        case '6' : // Shift PageDN
                            strcpy(pass->session->EscapeKey,"");
                            break;
                        
                        case '\0': // ESC
                            current_node->data = Line;
                            save_all();
                            dispose_list();
                            strcpy(pass->session->EscapeKey,"");
                            // Reset Top / Botom Margins
                            pass->session->printf("\x1b[1;25r");
                            return;
                        
                        default  :
                            strcpy(pass->session->EscapeKey,"");
                            break;
                    }                  
                }               
                // Handle ENTER \ New Line
                else if (c == '\r') {
                
                    //Note, add if cursor is in middle of line.. cut and move to next line!
                    // 1st Check if char Position is in the Middle of the Line                
                    if ((int)Line.size() != (Col-1)) {                    
                        TLine = Line.substr(Col-1, Line.size());
                        Line.erase(Col-1, (Col-1)-Line.size());                        
                        current_node->data = Line;
                        // Now Create New Line, and move from cursor position to end to new line
                        add_to_list("");             // Create Next Line                        
                        current_node->data = TLine;
                        Line = current_node->data;
                        Col  = 1; // Line.size()+1;
                        
                        ++Tot;    // Add To Total # of Lines
                        ++Row;    // Goto Next Row
                        ++Num;    // Currrent Line is Next                        
                        box_redraw();
                    }
                    else {                    
                        current_node->data = Line;   // Add Current Line to Current Node
                        add_to_list("");             // Create Next Line
                        Col  = 1;
                        Line = "";                
                        ++Tot;  // Add To Total # of Lines
                        ++Row;  // Goto Next Row
                        ++Num;  // Currrent Line is Next                                    
                        // Redraw Screen Text inside of Input Box
                        box_redraw(); 
                    }
                }
                // Check for Normal Letters, Numbers, Etc..
                else if ((int)c > 31 && (int)c < 126) {
                    pass->session->printf("%c", c);
                    add_char(c); // Add Char to line
                }
                // Delete Current Line
                else if ((int)c == 25) {  // CTRL Y
                    delete_line();
                    box_redraw();
                }                
                // Insert New  Line
                else if ((int)c == 14) { // CTRL N
                    insert_line();            
                
                }            
                // Toggle Insert / Overwrite
                else if ((int)c == 22) { // CTRL V
                
                
                }
                
                // Toggle Insert / Overwrite
                else if ((int)c == 3) { // CTRL C
                    for (int i = line_total(); i != 0; i--) {
                        delete_line_up();
                    }
                    box_redraw();
                
                }
            
            }
        }                
    }
}



bool msg_fse::fse_exists() {

    std::string path = BBSPATH;
    path += "fse.ini";

    FILE *stream;
    stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) { return false; }
    fclose(stream);
    return true;
}

void msg_fse::fse_create() {

    std::string name = BBSPATH;
    name += "fse.ini";

    ofstream outStream2;
    outStream2.open( name.c_str(), ofstream::out | ofstream::trunc );
    if (!outStream2.is_open()) {
        printf( "\nError Creating: %s \n", name.c_str());
        return;
    }

    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | " << VERSION << "           |" << endl;
    outStream2 << "# | Full Screen Editor Input Box Coords                            |" << endl;
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
    outStream2 << "# | Set the TOP Line # for the Top Margin                          |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "set TOP \"5\"" << endl;
    outStream2 << endl;
    outStream2 << "#" << endl;
    outStream2 << "# .----------------------------------------------------------------." << endl;
    outStream2 << "# | Set the BOTTOM Line # for the Bottom Margin                    |" << endl;
    outStream2 << "# `----------------------------------------------------------------'" << endl;
    outStream2 << "#" << endl;
    outStream2 << "set BOT \"22\"" << endl;
    outStream2 << endl;
    outStream2.close();
    return;
}

void msg_fse::fse_check(std::string cfgdata) {

    std::string temp = cfgdata;
    int id1 = -1;

    // Disgards any Config lines with the # Character
    if (temp[0] == '#') return;

    // Sets if LOGGING is on / off
    id1 = -1;
    id1 = temp.find("set TOP", 0);
    if (id1 != -1) {
        std::string temp1;
        int st1 = -1;
        int st2 = -1;
        signed int  ct = -1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        id1 = atoi(temp1.c_str());
        Top = id1;
    }

    // Sets Download Path
    id1 = -1;
    id1 = temp.find("set BOT", 0);
    if (id1 != -1) {
       std::string temp1;
        int st1 = -1;
        int st2 = -1;
        signed int  ct = -1;

        st1 = temp.find('"', 0);
        st2 = temp.find('"', st1+1);
        ++st1;
        temp1 = temp.substr(st1,st2);
        ct = st2 - st1;
        if (temp1.length() > ct)
            temp1.erase(ct,temp1.length());
        id1 = atoi(temp1.c_str());
        Bot = id1;
    }
}

void msg_fse::fse_parse() {

    std::string name = BBSPATH;
    name += "fse.ini";

    ifstream inStream;
    inStream.open( name.c_str() );
    if (!inStream.is_open()) {
        printf("Couldn't Open Config: %s\n", name.c_str());
        return;
    }

    std::string cfgdata;
    for (;;) {
        getline(inStream,cfgdata);
        fse_check(cfgdata);
        if(inStream.eof()) break;
    }
    inStream.close();
    return;
}


