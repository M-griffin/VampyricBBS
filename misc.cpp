
#include <stdio.h>
#include "struct.h"

void get_str ( PASSING *pass, char *szBuffer )
{

    int result;
    strcpy ( szBuffer,"" );

    while ( pass->session->isActive() )
    {
        result = pass->session->polls ( szBuffer );
        if ( result ) break;
        Sleep ( 10 );
    }
    pass->session->puts ( "\x1b[0m" );
}


void get_chr ( PASSING *pass, unsigned char &c )
{

    int result;
    c = '\0';

    while ( pass->session->isActive() )
    {
        result = pass->session->pollc ( c );
        if ( result ) break;
        Sleep ( 10 );
    }
    pass->session->puts ( "\x1b[0m" );
}

char *faddr2char ( char *s,fidoaddr *fa )
{
    if ( fa->point )
        sprintf ( s,"%u:%u/%u.%u",
                  fa->zone,fa->net,fa->node,fa->point );
    else
        sprintf ( s,"%u:%u/%u",
                  fa->zone,fa->net,fa->node );
    return ( s );
}
