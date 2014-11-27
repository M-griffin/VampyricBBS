#ifndef MSGS_H
#define MSGS_H

#include "struct.h"

typedef struct msg_func
{

    int  read_mbaselist ( mb_list_rec *mr, int recno );
    int  save_mbasetemp ( mb_list_rec *mr, int recno );
    int  save_mbaselist ( mb_list_rec *mr, int recno );
    int  msg_count();

    void save_msgbase ( mb_list_rec *mb );

} msg_func;

#endif
