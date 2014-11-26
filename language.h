#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <string>

using namespace std;

#include "struct.h"

typedef struct language {
    
    // Language File, Read and Compile new
    void lang_remove();
    int  lang_write(LangRec *lang, int idx);
    int  lang_read(LangRec *lang, int idx);
    void lang_get(char *lang, int idx);
    void lang_check(std::string lang);
    void lang_compile();
    
};




#endif
