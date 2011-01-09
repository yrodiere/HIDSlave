#include <stdio.h>
#include <stdlib.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

sdp_session_t *register_service()
{
    char * c;

    uint32_t service_uuid_int[] = { 0x00001124, 0x1000, 0x80000080, 0x5F9B34FB };
    uint8_t rfcomm_channel = 11;
    const char *service_name = "sdp-test";
    const char *service_dsc = "An experimentation about sdp registering service";
    const char *service_prov = "sdp-test-provider";

    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid;
    sdp_list_t *l2cap_list = 0, 
               *rfcomm_list = 0,
               *root_list = 0,
               *proto_list = 0, 
               *access_proto_list = 0;
    sdp_data_t *channel = 0, *psm = 0;

    sdp_record_t *record = sdp_record_alloc();

    // set the general service ID
    sdp_uuid128_create( &svc_uuid, &service_uuid_int );
    sdp_set_service_id( record, svc_uuid );

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( record, root_list );

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    proto_list = sdp_list_append( 0, l2cap_list );

    // set rfcomm information
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    sdp_list_append( rfcomm_list, channel );
    sdp_list_append( proto_list, rfcomm_list );

    // attach protocol information to service record
    access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( record, access_proto_list );

    // set the name, provider, and description
    sdp_set_info_attr(record, service_name, service_prov, service_dsc);


    // ICI, il manque peut être des lignes de codes.

    int err = 0;
    sdp_session_t *session = 0;

    // connect to the local SDP server, register the service record, and 
    // disconnect
    session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );
    err = sdp_record_register(session, record, 0);

    printf("error code return by register : %d\n", err);

    printf("session : \n sock : %d\n state : %d\n local : %d\n flags :%d\n tid : %d\n priv : %d\n", session->sock, session->state, session->local, session->flags, session->tid, session->priv);

    scanf("%c", c);    


    // cleanup
    sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );

    return session;
}

int main(int argc, char **argv) {
    sdp_session_t * session;
    
    session = (sdp_session_t*) malloc(sizeof(sdp_session_t));

    printf("register_service()\n");
    session = register_service();

    sdp_close(session);

    printf("end\n");
}
