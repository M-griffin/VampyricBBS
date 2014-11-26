
#include <string>
#include <ctype.h>
#include "misc.h"
#include "struct.h"
#include "msgapi.h"
#include "msg_edit.h"
#include "acs.h"
#include "msgs.h"

typedef enum {LOCAL, NETMAIL, ECHOMAIL, EMAIL, NEWS} MSGTYPE;
typedef enum {PUBLIC, PRIVATE} POSTTYPE;

char mbtypes[5][21]={"Local","Netmail","Echomail","Email","NewsGroup"};
char posttyp[2][11]={"Public","Private"};

#define MSG_QUOTE	0
#define MSG_TEXT	1
#define MSG_TEAR	2
#define MSG_ORIGIN	3

// Class Constructor
//----------------------------------------------------------------------------

msg_edit::msg_edit() {

    memset(&pass, 0, sizeof(PASSING));    
}

// Start BBS Menu System
//----------------------------------------------------------------------------
void msg_edit::start(PASSING *passing) {

    pass = passing;
}


char *msg_edit::faddr2char(char *s,fidoaddr *fa) {

	if(fa->point)
		sprintf(s,"%u:%u/%u.%u",
			fa->zone,fa->net,fa->node,fa->point);
	else
		sprintf(s,"%u:%u/%u",
			fa->zone,fa->net,fa->node);
	return(s);	
}

void msg_edit::mbeditmenu() {

	int x, result, page = 0;
	unsigned char ch;
	
	while(pass->session->isActive()) {
	
	    pass->session->puts("\x1b[0m\x1b[2J");
	    pass->session->puts(".----------------------------------------------------------------.\r\n");
   		pass->session->puts("| Message Forum Editor                                           |\r\n");
    	pass->session->puts("`----------------------------------------------------------------'\r\n");
	    x=list_message_bases(page);	    
        pass->session->puts("\r\n [d]elete, [e]dit, [i]nsert, [m]ove, [q]uit : \x1b[1;44m \x1b[D");
		
		get_chr(pass,ch); 
				
		switch(toupper(ch)){
			case 'D': delete_mbase();	break;
			case 'I': insert_mbase();	break;
			case 'E': modify_mbase();	break;
			case 'M': move_mbase();	    break;
			//case 'B':	import_backbone();	break;
			case 'Q': return;
			case '[':	
				page--;
				if(page < 0)
					page = 0;
				break;
			case ']':
				if(x)
					page++;
				break;
            default :
                break;
        }
    }
}

int msg_edit::list_message_bases(int page) {

    mb_list_rec mbl;
    
	char disp[61] ={0},
         tType[12]={0};
	
	char outbuff[1024]={0};

    int i = _mfunc.msg_count();
   	if (i != 0) {
        i = 0;
	    while(_mfunc.read_mbaselist(&mbl,i++)) {

    		strcpy(disp ,mbl.mbdisplay);
	       	if(mbl.Type == MSGTYPE_JAM)	 strcpy(tType,"Jam");
    		if(mbl.Type == MSGTYPE_SQUISH) strcpy(tType,"Squish");
			
            sprintf(outbuff, 
                "%3d %-22s %-10s %7.7s %7.7s %7.7s\r\n",
		        (i),
    		    disp,
	    	    mbl.mbfile,  
		        mbtypes[mbl.Kind],
                tType,
     	        posttyp[mbl.Pubpriv]);

    		pass->session->puts(outbuff);
		}
    }
    else {
        pass->session->printf(" No Forums Found!\r\n");
    }    	
}



void msg_edit::init_mbase(mb_list_rec *mb) {
   
    
	strcpy(mb->mbdisplay,"New Message Base");  // Description
	strcpy(mb->mbpath,MESGPATH);               // Path to Message Files
	strcpy(mb->mbfile,"newbase");	           // Filename
	mb->Type       = MSGTYPE_JAM;              // Default to JAM
	mb->Kind       = 0;                        // Local
	mb->Pubpriv    = 0;                        // Public
	
	/*
	copy_ACS(&mb->SysopACS,   &config.DefMsg.SysopACS);
	copy_ACS(&mb->ReadACS,    &config.DefMsg.ReadACS);
	copy_ACS(&mb->WriteACS,   &config.DefMsg.WriteACS);
	copy_ACS(&mb->NetworkACS, &config.DefMsg.NetworkACS);	
	
	strcpy(mb->OriginLine,config.DefMsg.OriginLine);
	
	mb->colors[MSG_QUOTE]  =config.DefMsg.colors[MSG_QUOTE];
	mb->colors[MSG_TEXT]   =config.DefMsg.colors[MSG_TEXT];
	mb->colors[MSG_TEAR]   =config.DefMsg.colors[MSG_TEAR];
	mb->colors[MSG_ORIGIN] =config.DefMsg.colors[MSG_ORIGIN];
	
	mb->Kind       = config.DefMsg.Kind;
	mb->Type       = config.DefMsg.Type;
	mb->Pubpriv    = config.DefMsg.Pubpriv;
	mb->Allowalias = config.DefMsg.Allowalias;
	mb->scantype   = config.DefMsg.scantype;
	mb->fidoaddr   = config.DefMsg.fidoaddr;
	mb->MaxMsgs    = config.DefMsg.MaxMsgs;
	mb->MaxAge     = config.DefMsg.MaxAge;
	*/
}


void msg_edit::kill_mbase(int mbnum) {

	mb_list_rec mbin;
	
	int i1,count=0;
	char ppath[81]={0};
	char ppath2[81]={0};

	i1=0;
	while(_mfunc.read_mbaselist(&mbin,i1)){
		if(i1 != mbnum){
			mbin.idx = count;
			count++;
			if(!_mfunc.save_mbasetemp(&mbin, mbin.idx)) {
				printf("Error saving to temp file.");
			}
        }
		i1++;	
    }

    // Remove Normal file
	sprintf(ppath,"%sforums.dat",DATAPATH); 
	remove(ppath);
	
    // Rename Tmp to Original    
	sprintf(ppath2,"%sforums.tmp",DATAPATH); 	
	rename(ppath2, ppath);
	
	// Remove Tmp
	remove(ppath2);
}

void msg_edit::delete_mbase() {

	char ttxt[21]={0};
	int num, total_mbases;

	total_mbases = _mfunc.msg_count();
    pass->session->printf("\r\nDelete which? [1-%d]: \x1b[1;44m  \x1b[2D", total_mbases);
    get_str(pass,ttxt);    
   	if (strcmp(ttxt," ") == 0) return;	
	num = atoi(ttxt);
	--num;

	if(num >= 0 && num <= total_mbases){
		kill_mbase(num);
	}
}


void msg_edit::poke_mbase(int pokenum) {

	mb_list_rec mbin;
    int total_mbases = _mfunc.msg_count();

	if(total_mbases>0){
		for(int i1 = total_mbases; i1 > pokenum; i1--) {
			_mfunc.read_mbaselist(&mbin,i1-1);
			mbin.idx++;
			_mfunc.save_mbaselist(&mbin,mbin.idx);
		}
	}
	else
	   pokenum=0;
	   
	init_mbase(&mbin);
	mbin.idx=pokenum;
	_mfunc.save_mbaselist(&mbin,mbin.idx);
	total_mbases++;
}

void msg_edit::insert_mbase() {

    mb_list_rec mbin;
    memset(&mbin,0,sizeof(mb_list_rec));
    
	char ttxt[21]={0};
	int num, total_mbases, result;
	unsigned char c;
	
	total_mbases = _mfunc.msg_count();
    pass->session->printf("\r\n Insert before which? [1-%d]: \x1b[1;44m  \x1b[2D", total_mbases+1);

    get_str(pass,ttxt);    
   	if (strcmp(ttxt," ") == 0) return;	
	num = atoi(ttxt);
	--num;
	
	if(num >= 0 && num <= total_mbases){
	
		if(num == 0 && total_mbases == 0) {
			init_mbase(&mbin);
			mbin.idx = total_mbases;
			_mfunc.save_mbaselist(&mbin, mbin.idx);
			total_mbases++;
		}
		else {
			poke_mbase(num);
		}
	}
}



void msg_edit::swap_mbase(int iFrom, int iTo) {

	mb_list_rec mbto, mbfrom;

	_mfunc.read_mbaselist(&mbfrom, iFrom);

	if(iFrom < iTo) {
		for(int i1 = iFrom+1; i1 <= iTo; i1++) {
			_mfunc.read_mbaselist(&mbto,i1);
			mbto.idx--;
			_mfunc.save_mbaselist(&mbto, mbto.idx);
		}
    }
	else {
		for(int i1 = iFrom; i1 > iTo; i1--) {
			_mfunc.read_mbaselist(&mbto, i1-1);
			mbto.idx++;
			_mfunc.save_mbaselist(&mbto, mbto.idx);
		}
    }

	mbfrom.idx = iTo;
	_mfunc.save_mbaselist(&mbfrom, mbfrom.idx);
}


void msg_edit::move_mbase() {

	char ttxt[21]={0}, ttxt1[21]={0};
	int num, num1, total_mbases, result;
	
	total_mbases = _mfunc.msg_count();

    pass->session->printf("\r\n Move which? [1-%d]: \x1b[1;44m  \x1b[2D", total_mbases);	
	get_str(pass,ttxt);    
   	if (strcmp(ttxt," ") == 0) return;	
	num = atoi(ttxt);
	--num;
	
	if(num1 >= 0 && num1 < total_mbases) {
	
	    pass->session->printf("\r\n Position Before? [1-%d]: \x1b[1;44m  \x1b[2D", total_mbases);	
	    get_str(pass,ttxt);
        if (strcmp(ttxt," ") == 0) return;
    	num1 = atoi(ttxt);
	    --num1;
	
		if(num >= 0 && num < total_mbases)
			swap_mbase(num1, num);
    }
}


void msg_edit::modify_mbase() {

	char ttxt[21]={0};
	int num, result;
	
	int total_mbases = _mfunc.msg_count();
    pass->session->printf("\r\n Edit which? [1-%d]: \x1b[1;44m  \x1b[2D", total_mbases);    
    get_str(pass,ttxt);
   	if (strcmp(ttxt," ") == 0) return;	
	
	num = atoi(ttxt);
	--num;
	
	if(num>=0 && num<=total_mbases){
		mod_mbase(num);
	}
}

void msg_edit::edit_aka(mb_list_rec *mr) {

	char ttxt[16];
	char *p;

	pass->session->puts("\r\n\r\n Enter Fido Address: \x1b[1;44m                              \x1b[30D");
    get_str(pass,ttxt);
    if (strcmp(ttxt," ") == 0) return;
    //strcpy(mbl.mbdisplay,ttxt);
    
	//get_aka(ttxt);  Maybe Impliment Later

	p=strtok(ttxt,":/.");
	if(p)	mr->aka.zone=atoi(p);
	p=strtok(NULL,":/.");
	if(p)	mr->aka.net=atoi(p);
	p=strtok(NULL,":/.");
	if(p)	mr->aka.node=atoi(p);
	p=strtok(NULL,":/.");
	if(p)	mr->aka.point=atoi(p);

	//mci("~SM`09Domain`0F: `1F");
	//ttxt[0]='\0';
	//get_str(config.aka[num].domain,21);
}


void msg_edit::ACS(ACSrec *acs, char *fstr) {

    class acs_levels *level = new acs_levels;
    
    level->start(pass);
    level->ACS_setup(acs, fstr);
    delete level;
}


void msg_edit::mod_mbase(int mbnum) {

	mb_list_rec mbl;
	char faka[41]={0};	
	char sigtxt[3][21]={"None","Short Sig.","Long Sig."};
	std::string TF;
	char rBuffer[200]={0};

	_mfunc.read_mbaselist(&mbl, mbnum);
	if(mbl.sigtype < 0 || mbl.sigtype > 2) mbl.sigtype = 0;
	
	int total_mbases = _mfunc.msg_count();
	int result;
	unsigned char ch;
	
	while(pass->session->isActive()) {
	
		faddr2char(faka,&mbl.aka); 
		strcpy(rBuffer,"");
		
		pass->session->puts("\x1b[0m\x1b[2J");
	    pass->session->puts(".----------------------------------------------------------------.\r\n");
   		pass->session->puts("| Message Forum Editor                                           |\r\n");
    	pass->session->puts("`----------------------------------------------------------------'\r\n");
        pass->session->printf("  Forum: %d of %d", mbnum+1,total_mbases);
		
		pass->session->puts("\r\n");
    	pass->session->puts(".-------------------------.");	
        pass->session->puts("\r\n");
    	pass->session->printf("| [A]. Description        | %s", mbl.mbdisplay);
    	pass->session->puts("\r\n");
    	pass->session->printf("| [B]. Filename           | %s", mbl.mbfile);
    	if (mbl.Type == MSGTYPE_JAM) TF = "JAM";
		else TF = "SQUISH";
		pass->session->puts("\r\n");
    	pass->session->printf("| [C]. Message Type       | %s", TF.c_str());
	    pass->session->puts("\r\n");
    	pass->session->printf("| [D]. Forum Kind         | %s", mbtypes[mbl.Kind]);
   	    pass->session->puts("\r\n");
    	pass->session->printf("| [E]. Posting Type       | %s", posttyp[mbl.Pubpriv]);
		pass->session->puts("\r\n");
    	pass->session->printf("| [F]. Info Filename      | %s", mbl.infofile);
        if (mbl.Allowalias) TF = "yes";
		else TF = "no";
		pass->session->puts("\r\n");
    	pass->session->printf("| [G]. Allow Aliases      | %s", TF.c_str());
    	pass->session->puts("\r\n");
        pass->session->printf("| [H]. Post ACS           | ");
    	pass->session->puts("\r\n");
    	pass->session->printf("| [I]. Read ACS           | ");    	
    	pass->session->puts("\r\n");
    	pass->session->printf("| [J]. Sysop ACS          | ");
    	pass->session->puts("\r\n");
    	pass->session->printf("| [K]. Signature          | %s", sigtxt[mbl.sigtype]);
        pass->session->puts("\r\n");
    	pass->session->printf("| [L]. Mail address       | %s", faka);

        /*
    	mci("%sQuoted (%d)`0F, ", 
			ansic(mbl.colors[MSG_QUOTE]),
			mbl.colors[MSG_QUOTE]);
		mci("%sText (%d)`0F,",
			ansic(mbl.colors[MSG_TEXT]),
			mbl.colors[MSG_TEXT]);
		mci("%sTear (%d)`0F,",
			ansic(mbl.colors[MSG_TEAR]),
			mbl.colors[MSG_TEAR]);
		mci("%sOrigin (%d)~SM",
			ansic(mbl.colors[MSG_ORIGIN]),
			mbl.colors[MSG_ORIGIN]);
        */

		pass->session->puts("\r\n");
    	pass->session->printf("| [M]. Origin Line        | %s", mbl.OriginLine);
    	pass->session->puts("\r\n");
     	pass->session->puts("`-------------------------'\r\n\r\n");

        pass->session->puts(" [d]elete, [e]dit, [i]nsert, [m]ove, [q]uit : \x1b[1;44m \x1b[D");	    
	    
		get_chr(pass,ch);		
		switch(toupper(ch)) {
		
			case 'A':
			    pass->session->puts("\r\n\r\n Description: \x1b[1;44m                              \x1b[30D");
       		    get_str(pass,rBuffer);
       		    if (strcmp(rBuffer," ") == 0) break;          
                strcpy(mbl.mbdisplay,rBuffer);
				break;
				
			case 'B':
			    pass->session->puts("\r\n\r\n New Filename: \x1b[1;44m                              \x1b[30D");
       		    get_str(pass,rBuffer);
       		    if (strcmp(rBuffer," ") == 0) break;          
                strcpy(mbl.mbfile,rBuffer);
				break;
				
			case 'C':
			    if(mbl.Type == MSGTYPE_JAM)	mbl.Type = MSGTYPE_SQUISH;
				else						mbl.Type = MSGTYPE_JAM;
				break;
				
            case 'D':
                ++mbl.Kind;
				if(mbl.Kind > 4)
					mbl.Kind = 0;
				switch(mbl.Kind){
					case LOCAL    : mbl.Pubpriv = PUBLIC;  break;
					case ECHOMAIL : mbl.Pubpriv = PUBLIC;  break;
					case NETMAIL  : mbl.Pubpriv = PRIVATE; break;
					case EMAIL    : mbl.Pubpriv = PRIVATE; break;
					case NEWS     : mbl.Pubpriv = PUBLIC;  break;
				}
				break;

            case 'E':
                if(mbl.Pubpriv==PUBLIC) {
					switch(mbl.Kind){
						case LOCAL	  : mbl.Pubpriv = PRIVATE; break;
						case ECHOMAIL : mbl.Pubpriv = PUBLIC;  break;
						case NETMAIL  : mbl.Pubpriv = PRIVATE; break;
						case EMAIL    : mbl.Pubpriv = PRIVATE; break;
						case NEWS	  : mbl.Pubpriv = PUBLIC;  break;
					}
				}
				else{
					switch(mbl.Kind){
						case LOCAL	  : mbl.Pubpriv = PUBLIC;  break;
						case ECHOMAIL : mbl.Pubpriv = PUBLIC;  break;
						case NETMAIL  : mbl.Pubpriv = PRIVATE; break;
						case EMAIL    : mbl.Pubpriv = PRIVATE; break;
						case NEWS	  : mbl.Pubpriv = PUBLIC;  break;
					}
				}
				break;
				
            case 'F': 
                pass->session->puts("\r\n\r\n Info File: \x1b[1;44m                              \x1b[30D");
       		    get_str(pass,rBuffer);
       		    if (strcmp(rBuffer," ") == 0) break;          
                strcpy(mbl.infofile,rBuffer);
				break;

            case 'G': mbl.Allowalias = (mbl.Allowalias)?0:1;  break;                

			case 'H': ACS(&mbl.WriteACS,mbl.mbdisplay); break;
			case 'I': ACS(&mbl.ReadACS, mbl.mbdisplay); break;
			case 'J': ACS(&mbl.SysopACS,mbl.mbdisplay); break;
			
			case 'K':
				mbl.sigtype++;
				if(mbl.sigtype<0 || mbl.sigtype>2)
					mbl.sigtype=0;
				break;
				
            case 'L':
			    edit_aka(&mbl);
				//mbl.fidoaddr=pick_aka();
				break;
									
			case 'M':
   		        pass->session->puts("\r\n\r\n Origin Line: \x1b[1;44m                              \x1b[30D");
       		    get_str(pass,rBuffer);
       		    if (strcmp(rBuffer," ") == 0) break;          
                strcpy(mbl.OriginLine,rBuffer);
                break;						
			
			case 'Q':
				_mfunc.save_mbaselist(&mbl, mbnum); 
				_mfunc.save_msgbase(&mbl);
                return;

		}
	}
}

