#ifndef ACS_H
#define ACS_H

#include "struct.h"

class acs_levels {

    protected:
    PASSING *pass;
    
    public:
    acs_levels();
    void start(PASSING *passing);

    void copy_ACS(ACSrec *To, ACSrec *From);
    void mod_Sflag(ACSrecS *flg);
    char *Sflag2str(ACSrecS *flg);
    void mod_Lflag(ACSrecL *flg);
    char *Lflag2str(ACSrecL *flg);
    void mod_dotw(unsigned int *DOTW);
    void ACS_setup(ACSrec *acs, char *fstr);
    char *flag2dotw(unsigned int DOTW);
    
};

#endif

