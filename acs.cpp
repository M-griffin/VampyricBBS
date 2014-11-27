
#include <stdio.h>
#include <ctype.h>

#include "struct.h"

#include "acs.h"
#include "misc.h"

char dotwflg[35];

acs_levels::acs_levels()
{

    memset ( &pass,0,sizeof ( PASSING ) );

}

void acs_levels::start ( PASSING *passing )
{

    pass = passing;

}

void acs_levels::copy_ACS ( ACSrec *To, ACSrec *From )
{

    To->eflags.onn=From->eflags.onn;
    To->eflags.off=From->eflags.off;
    To->fflags.onn=From->fflags.onn;
    To->fflags.off=From->fflags.off;
    To->mgroup.onn=From->mgroup.onn;
    To->mgroup.off=From->mgroup.off;
    To->fgroup.onn=From->fgroup.onn;
    To->fgroup.off=From->fgroup.off;
    To->gender=From->gender;
    To->min_sl=From->min_sl;
    To->dotw=From->dotw;
    To->min_age=From->min_age;
    To->min_logons=From->min_logons;
    To->min_nodes=From->min_nodes;
}

void acs_levels::mod_Sflag ( ACSrecS *flg )
{

    unsigned char ch;
    int num;

    while ( pass->session->isActive() )
    {

        pass->session->printf ( "\r\n Select (A-P, Q=Quit):[%s]: ", Sflag2str ( flg ) );
        get_chr ( pass, ch );
        if ( strchr ( "ABCDEFGHIJKLMNOPQ", toupper ( ch ) ) )
        {

            switch ( toupper ( ch ) )
            {
                case 'Q':
                    return;

                default :
                    num = ch-'A';
                    if ( BitTst ( flg->onn,num ) && BitTst ( flg->off,num ) )
                    {
                        flg->onn=BitClr ( flg->onn,num );
                    }
                    else
                    {
                        if ( BitTst ( flg->onn,num ) && !BitTst ( flg->off,num ) )
                        {
                            flg->off=BitSet ( flg->off,num );
                        }
                        else
                        {
                            if ( !BitTst ( flg->onn,num ) && !BitTst ( flg->off,num ) )
                            {
                                flg->onn=BitSet ( flg->onn,num );
                            }
                            else
                            {
                                if ( !BitTst ( flg->onn,num ) && BitTst ( flg->off,num ) )
                                {
                                    flg->off=BitClr ( flg->off,num );
                                }
                            }
                        }
                    }
                    break;
            }
        }
    }
}

char *acs_levels::Sflag2str ( ACSrecS *flg )
{

    int i;

    memset ( dotwflg,'\0',35 );

    for ( i=0; i<16; i++ )
    {
        if ( BitTst ( flg->onn,i ) &&  BitTst ( flg->off,i ) )    dotwflg[i]='!';
        if ( !BitTst ( flg->onn,i ) &&  BitTst ( flg->off,i ) )    dotwflg[i]=i+'a';
        if ( BitTst ( flg->onn,i ) && !BitTst ( flg->off,i ) )    dotwflg[i]=i+'A';
        if ( !BitTst ( flg->onn,i ) && !BitTst ( flg->off,i ) )    dotwflg[i]='-';
    }

    dotwflg[16]='\0';

    return ( dotwflg );
}


void acs_levels::mod_Lflag ( ACSrecL *flg )
{


    unsigned char ch;
    int num;

    while ( pass->session->isActive() )
    {

        pass->session->printf ( "\r\n Select (A-Z, <CR>=Quit):[%s]: ",Lflag2str ( flg ) );
        get_chr ( pass, ch );
        if ( strchr ( "@ABCDEFGHIJKLMNOPQRSTUVWXYZ\n\r", toupper ( ch ) ) )
        {

            switch ( toupper ( ch ) )
            {

                case '\r':
                case '\n':
                    return;
                case '@':
                    if ( flg->onn )     flg->onn=0L;
                    else            flg->onn=0xFFFFFFFF;
                    break;
                default:
                    num=ch-'A';
                    if ( BitTst ( flg->onn,num ) && BitTst ( flg->off,num ) )
                    {
                        flg->onn=BitClr ( flg->onn,num );
                    }
                    else
                    {
                        if ( BitTst ( flg->onn,num ) && !BitTst ( flg->off,num ) )
                        {
                            flg->off=BitSet ( flg->off,num );
                        }
                        else
                        {
                            if ( !BitTst ( flg->onn,num ) && !BitTst ( flg->off,num ) )
                            {
                                flg->onn=BitSet ( flg->onn,num );
                            }
                            else
                            {
                                if ( !BitTst ( flg->onn,num ) && BitTst ( flg->off,num ) )
                                {
                                    flg->off=BitClr ( flg->off,num );
                                }
                            }
                        }
                    }
                    break;
            }
        }
    }
}

char *acs_levels::Lflag2str ( ACSrecL *flg )
{

    int i;

    memset ( dotwflg,'\0',35 );

    for ( i=0; i<26; i++ )
    {
        if ( BitTst ( flg->onn,i ) &&  BitTst ( flg->off,i ) )    dotwflg[i]='!';
        if ( !BitTst ( flg->onn,i ) &&  BitTst ( flg->off,i ) )    dotwflg[i]=i+'a';
        if ( BitTst ( flg->onn,i ) && !BitTst ( flg->off,i ) )    dotwflg[i]=i+'A';
        if ( !BitTst ( flg->onn,i ) && !BitTst ( flg->off,i ) )    dotwflg[i]='-';
    }

    dotwflg[32]='\0';

    return ( dotwflg );
}


void acs_levels::mod_dotw ( unsigned int *DOTW )
{

    unsigned char ch;
    int onn,off;

    while ( pass->session->isActive() )
    {

        pass->session->printf ( "\r\n Select (A-G, Q=Quit):[%s]: ",flag2dotw ( *DOTW ) );
        get_chr ( pass, ch );
        if ( strchr ( "ABCDEFGQ", toupper ( ch ) ) )
        {

            switch ( toupper ( ch ) )
            {

                case 'Q':
                    return;

                default :
                    onn=ch-'A';
                    off=onn+7;
                    if ( BitTst ( *DOTW,onn ) && BitTst ( *DOTW,off ) )
                    {
                        *DOTW=BitClr ( *DOTW,onn );
                    }
                    else
                    {
                        if ( BitTst ( *DOTW, onn ) && !BitTst ( *DOTW,off ) )
                        {
                            *DOTW=BitSet ( *DOTW,off );
                        }
                        else
                        {
                            if ( !BitTst ( *DOTW,onn ) && !BitTst ( *DOTW,off ) )
                            {
                                *DOTW=BitSet ( *DOTW,onn );
                            }
                            else
                            {
                                if ( !BitTst ( *DOTW,onn ) && BitTst ( *DOTW,off ) )
                                {
                                    *DOTW=BitClr ( *DOTW,off );
                                }
                            }
                        }
                    }
                    break;
            }
        }
    }
}

char *acs_levels::flag2dotw ( unsigned int DOTW )
{

    char dotwchars[17];
    int i,onn,off;

    strcpy ( dotwchars,"SMTWTFSsmtwtfs" );
    memset ( dotwflg,'\0',8 );

    for ( i=0; i<7; i++ )
    {
        onn=i;
        off=onn+7;
        if ( BitTst ( DOTW,onn ) &&  BitTst ( DOTW,off ) )    dotwflg[onn]='!';
        if ( !BitTst ( DOTW,onn ) &&  BitTst ( DOTW,off ) )    dotwflg[onn]=i+'a';
        if ( BitTst ( DOTW,onn ) && !BitTst ( DOTW,off ) )    dotwflg[onn]=i+'A';
        if ( !BitTst ( DOTW,onn ) && !BitTst ( DOTW,off ) )    dotwflg[onn]='-';
    }

    dotwflg[8]='\0';

    return ( dotwflg );
}

void acs_levels::ACS_setup ( ACSrec *acs, char *fstr )
{

    char ttxt[21],*gentxt[3]= {"Don\'t Care","Male","Female"};
    unsigned char ch;

    while ( pass->session->isActive() )
    {

        pass->session->puts ( "\x1b[0m\x1b[2J" );

        pass->session->printf ( "\r\nSecurity Setup for %s",fstr );
        pass->session->puts ( "\r\n\r\n" );
        pass->session->printf ( "(S) Min SecLev   : %d", acs->min_sl );
        pass->session->puts ( "\r\n" );
        pass->session->printf ( "(D) Days/Week    : %s", flag2dotw ( acs->dotw ) );
        pass->session->puts ( "\r\n" );
        pass->session->printf ( "(G) Gender       : %s", gentxt[acs->gender] );
        pass->session->puts ( "\r\n" );
        pass->session->printf ( "(E) E Flags      : %s", Sflag2str ( &acs->eflags ) );
        pass->session->puts ( "\r\n" );
        pass->session->printf ( "(N) Minimum Node : %d", acs->min_nodes );
        pass->session->puts ( "\r\n" );
        pass->session->printf ( "(F) F Flags      : %s", Sflag2str ( &acs->fflags ) );
        pass->session->puts ( "\r\n" );
        pass->session->printf ( "(L) Min. Logins  : %d", acs->min_logons );
        pass->session->puts ( "\r\n" );
        pass->session->printf ( "(A) Min. Age     : %d", acs->min_age );
        pass->session->puts ( "\r\n" );
        pass->session->printf ( "(M) Msg Groups   : %s", Lflag2str ( &acs->mgroup ) );
        pass->session->puts ( "\r\n" );
        pass->session->printf ( "(I) File Groups  : %s", Lflag2str ( &acs->fgroup ) );
        pass->session->puts ( "\r\n\r\n" );
        pass->session->printf ( "Select, Q=Quit): ", sizeof ( ACSrec ) );

        get_chr ( pass, ch );
        if ( strchr ( "SDGENFLAMIQ", toupper ( ch ) ) )
        {

            switch ( toupper ( ch ) )
            {

                case 'Q':
                    return;
                case 'A':
                    pass->session->puts ( "Minimum Age: \x1b[1;44m  \x1b[2D" );
                    get_str ( pass,ttxt );
                    if ( strcmp ( ttxt," " ) == 0 ) break;
                    acs->min_age = atoi ( ttxt );
                    break;

                case 'D':
                    mod_dotw ( &acs->dotw );
                    break;
                case 'E':
                    mod_Sflag ( &acs->eflags );
                    break;
                case 'F':
                    mod_Sflag ( &acs->fflags );
                    break;
                case 'G':
                    acs->gender++;
                    if ( acs->gender>2 )    acs->gender=0;
                    break;

                case 'I':
                    mod_Lflag ( &acs->fgroup );
                    break;
                case 'L':
                    pass->session->puts ( "Minimum Logons: \x1b[1;44m  \x1b[2D" );
                    get_str ( pass,ttxt );
                    if ( strcmp ( ttxt," " ) == 0 ) break;
                    acs->min_logons = atoi ( ttxt );
                    break;

                case 'M':
                    mod_Lflag ( &acs->mgroup );
                    break;
                case 'N':
                    pass->session->puts ( "Minimum Node Number: \x1b[1;44m  \x1b[2D" );
                    get_str ( pass,ttxt );
                    if ( strcmp ( ttxt," " ) == 0 ) break;
                    acs->min_nodes = atoi ( ttxt );
                    break;

                case 'S':
                    pass->session->puts ( "Minimum Security Level: \x1b[1;44m  \x1b[2D" );
                    get_str ( pass,ttxt );
                    if ( strcmp ( ttxt," " ) == 0 ) break;
                    acs->min_sl = atoi ( ttxt );
                    break;

            }
        }
    }
}
