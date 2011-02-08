#include <stdio.h>

#include <bluetooth/bluetooth.h>

#include "sdp.h"
#include "l2cap.h"
#include "hid.h"


/*
 * is : interrupt socket
 * iss : interupt server socket
 * cs : controler socket
 * css : controler server socket
 */
int is, iss, cs, css;

void sendChar(unsigned char keyCode)
{
    unsigned char pkg[12];

    pkg[0] = 0xa1;
    pkg[1] = 0x01;
    pkg[2] = 0x00; //modifiers ?
    pkg[3] = 0x00;
    pkg[4] = keyCode;
    pkg[5] = 0x00;
    pkg[6] = 0x00;
    pkg[7] = 0x00;
    pkg[8] = 0x00;
    pkg[9] = 0x00;

    if (write(is, pkg, 10) <= 0) {
            perror("write");
    }
}

int main()
{
    unsigned char keyCode;
	bdaddr_t dst;


    printf("// SDP test //////\n");
    
	sdp_open();
	printf("----- open -------------\n");	
    
	sdp_add_keyboard();
	printf("----- add keyboard -----\n");
	    
    css = l2cap_listen(BDADDR_ANY, L2CAP_PSM_HIDP_CTRL, 0, 1);
	perror("");
    printf("----- css listen--------\n");
	

	iss = l2cap_listen(BDADDR_ANY, L2CAP_PSM_HIDP_INTR, 0, 1);
	perror("");
	printf("----- iss listen -------\n");

    cs = l2cap_accept(css, NULL);
    //cs = l2cap_connect(BDADDR_ANY, &dst, L2CAP_PSM_HIDP_CTRL);
    //perror("");
    printf("----- cs accept --------\n");
    
    is = l2cap_accept(iss, NULL);
    //is = l2cap_connect(BDADDR_ANY, &dst, L2CAP_PSM_HIDP_INTR);
    //perror("");
    printf("----- is accept --------\n");
    

    
    
    printf("\n");
	
	for(;;)
	{
	    keyCode = getchar();
	    sendChar(keyCode);
	}
	
	close(is);
	close(cs);
	close(iss);
	close(css);
	sdp_remove();
	printf("----- removed ----------\n");
	
	return(1);
}
