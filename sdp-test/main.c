#include <stdio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

#include "sdp.h"
//#include "l2cap.h" Apparement plus necessaire
#include "hid.h"

int main()
{

    /*
     * is : interrupt socket
     * iss : interupt server socket
     * cs : controler socket
     * css : controler server socket
     */
	int is, iss, cs, css;


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
    printf("----- cs accept --------\n");
    
    is = l2cap_accept(iss, NULL);
    printf("----- is accept --------\n");
    printf("\n");
    	
	printf("      Press enter to stop and remove keyboard\n");
	getchar();
	
	close(is);
	close(cs);
	close(iss);
	close(css);
	sdp_remove();
	printf("----- removed ----------\n");
	
	return(1);
}
