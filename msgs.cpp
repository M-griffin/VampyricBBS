
#include <stdio.h>
#include <string>

using namespace std;

#include "struct.h"
#include "msgapi.h"
#include "msgs.h"



int msg_func::read_mbaselist(mb_list_rec *mr, int recno) {

	int x = 0;

	std::string path = DATAPATH;
    path += "forums.dat";

	FILE *fptr = fopen(path.c_str(),"rb+");
	if(fptr == NULL) {
	    fptr = fopen(path.c_str(), "wb");
    	if(fptr == NULL) {
    		printf("Error forum_read!");
        	return x;
    	}
	}

	if(fseek(fptr,(long)recno*sizeof(mb_list_rec),SEEK_SET)==0)
		x=fread(mr,sizeof(mb_list_rec),1,fptr);

	fclose(fptr);
	return(x);
}

int msg_func::save_mbasetemp(mb_list_rec *mr, int recno) {

	FILE *fptr;
	int x = 0;

	std::string path = DATAPATH;
    path += "forums.tmp";

	fptr=fopen(path.c_str(),"rb+");
	if(fptr==NULL) {
		fptr=fopen(path.c_str(),"wb");
		if(fptr==NULL) {
			printf("Error creating forums.tmp");
			return x;
			}
		}

	if(fseek(fptr,(long)recno*sizeof(mb_list_rec),SEEK_SET)==0)
		x=fwrite(mr,sizeof(mb_list_rec),1,fptr);

	fclose(fptr);
	return(x);
}


int msg_func::save_mbaselist(mb_list_rec *mr, int recno) { 

	std::string path = DATAPATH;
    path += "forums.dat";

    int x = 0;
	FILE *stream = fopen(path.c_str(),"rb+");
   	if(stream == NULL) {
	   	stream = fopen(path.c_str(), "wb");
    	if(stream == NULL) {
    		printf("Error forum_write!");
        	return x;
    	}
	}
    if(fseek(stream,(int)recno*sizeof(mb_list_rec),SEEK_SET)==0)
	    x = fwrite(mr,sizeof(mb_list_rec),1,stream);
    fclose(stream);
    return x;
}



int msg_func::msg_count() {

	int i = 0;
    mb_list_rec mb;

    while(read_mbaselist(&mb,i++));
    if(i < 1)	i = -1;
    else i--;
    return(i);
}


void msg_func::save_msgbase(mb_list_rec *mb) {

	struct _minf m;
	HAREA harea;
	HMSG hmsg;
	
	char path[81];
	sprintf(path,"%s%s", mb->mbpath, mb->mbfile);

	m.req_version = 0;
	m.def_zone = 1;
	
	if(MsgOpenApi(&m) == 0 ){
		harea = MsgOpenArea((BYTE *)path, MSGAREA_CRIFNEC, mb->Type | MSGTYPE_ECHO);
		if(harea != NULL){
			hmsg = MsgOpenMsg(harea, MOPEN_CREATE,0);
			if(hmsg != NULL) MsgCloseMsg(hmsg);
			else printf("\r\nUnable to create message area.");
		 	MsgCloseArea(harea);
        }
		else printf("\r\nUnable to open %s", path);
		MsgCloseApi();
    }
}
