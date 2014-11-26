#include <stdio.h>
#include <string>

using namespace std;

#include "users.h"
#include "struct.h"


int users::idx_writet(UserIdx *usr, int idx) { 

	std::string path = DATAPATH;
    path += "uidx.tmp";

    int x = 0;
	FILE *stream = fopen(path.c_str(),"rb+");
   	if(stream == NULL) {
	   	stream = fopen(path.c_str(), "wb");
    	if(stream == NULL) {
    		printf("Error uidx_write temp!");
        	return x;
    	}
	}
    if(fseek(stream,(int)idx*sizeof(UserIdx),SEEK_SET)==0)
	    x = fwrite(usr,sizeof(UserIdx),1,stream);
    fclose(stream);
    return x;
}

int users::idx_write(UserIdx *usr, int idx) {

	std::string path = DATAPATH;
    path += "uidx.dat";

	int x = 0;
	FILE *stream=fopen(path.c_str(),"rb+");
	if(stream == NULL){
		stream=fopen(path.c_str(), "wb");
		if(stream == NULL) {
			printf("Error uidx_write!");
			return x;
		}
	}
	if(fseek(stream,(int)idx*sizeof(UserIdx),SEEK_SET)==0)
		x = fwrite(usr,sizeof(UserIdx),1,stream);
	fclose(stream);
	return x;
}

int users::idx_read(UserIdx *usr, int idx) {

	std::string path = DATAPATH;
    path += "uidx.dat";

	int x;
	FILE *stream=fopen(path.c_str(),"rb+");
	if(stream == NULL) {
		stream=fopen(path.c_str(), "wb");
		if(stream == NULL) {
			printf("Error uidx_read!");
			return x;
		}
	}
	fclose(stream);

	stream = fopen(path.c_str(), "rb");
	if(fseek(stream,(long)idx*sizeof(UserIdx),SEEK_SET)==0)
		x = fread(usr,sizeof(UserIdx),1,stream);
	fclose(stream);
	return x;
}

int users::idx_count() {
	
	UserIdx usr;

    int i = 0;
	while(idx_read(&usr,i++));
	if(i < 1)	i=-1;
	else		i--;
	return(i);
}

int users::idx_find(char *name) {

	UserIdx usr;
	int idx = 0;
	std::string temp1, temp2;

	temp1 = (name);
	while(idx_read(&usr,idx)) {
		temp2 = (usr.Handle);
		if(temp1 == temp2) return(idx);
        idx++;
    }
	return(-1);
}

bool users::idx_match(char *name) {

	int index =- 1;
	index = idx_find(name);
	if (index == -1) return false;
	else return true;
}

void users::idx_new(char *name, int idx) {

	UserIdx usr;	
	
	memset(&usr,0,sizeof(UserRec));
	strcpy(usr.Handle,name);
	usr.Number = idx;
	idx_write(&usr,idx);

}

int users::users_writet(UserRec *usr, int idx) { 

	std::string path = DATAPATH;
    path += "users.tmp";

    int x = 0;
	FILE *stream = fopen(path.c_str(),"rb+");
   	if(stream == NULL) {
	   	stream = fopen(path.c_str(), "wb");
    	if(stream == NULL) {
    		printf("Error users_write temp!");
        	return x;
    	}
	}
    if(fseek(stream,(int)idx*sizeof(UserRec),SEEK_SET)==0)
	    x = fwrite(usr,sizeof(UserRec),1,stream);
    fclose(stream);
    return x;
}

int users::users_write(UserRec *usr, int idx) { 

	std::string path = DATAPATH;
    path += "users.dat";

    int x = 0;
	FILE *stream = fopen(path.c_str(),"rb+");
   	if(stream == NULL) {
	   	stream = fopen(path.c_str(), "wb");
    	if(stream == NULL) {
    		printf("Error users_write!");
        	return x;
    	}
	}
    if(fseek(stream,(int)idx*sizeof(UserRec),SEEK_SET)==0)
	    x = fwrite(usr,sizeof(UserRec),1,stream);
    fclose(stream);
    return x;
}

int users::users_read(UserRec *usr, int idx) {

    std::string path = DATAPATH;
    path += "users.dat";
	
    int x = 0;
    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL) {
    	stream=fopen(path.c_str(), "wb");
    	if(stream == NULL) {
       		printf("Error users_read!");
      		return x;
    	}
	}
    fclose(stream);

	stream = fopen(path.c_str(), "rb");
    if(fseek(stream,(int)idx*sizeof(UserRec),SEEK_SET)==0)
    	x = fread(usr,sizeof(UserRec),1,stream);
    fclose(stream);
    return x;
}



