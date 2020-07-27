/*
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

sdp_session_t *register_service(uint8_t rfcomm_channel)
{
    // uint32_t service_uuid_int[] = { 0, 0, 0, 0xABCD }; /* service id: '00000000-0000-0000-0000-0000CDAB0000' */
    uint32_t service_uuid_int[] = { 0x01000000, 0xe8110100, 0x84f40180, 0x3412404c }; /* service id: '00000001-0001-11E8-8001-F4844C401234' */
    /* uint8_t rfcomm_channel = 11; */
    const char *service_name = "Roto-Rooter Data Router";
    const char *service_dsc = "An experimental plumbing router";
    const char *service_prov = "Roto-Rooter";

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
    int err = 0;
    sdp_session_t *session = 0;

    // connect to the local SDP server, register the service record, and
    // disconnect
    session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );
    if (session) {
        err = sdp_record_register(session, record, 0);
        if (err) {
            sdp_close( session );
            session = 0;
            fprintf(stderr, "    Error: sdp session register failed.\n");
        }
    } else {
        fprintf(stderr, "    Error: no session connected.\n");
    }

    // cleanup
    sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );

    return session;
}

int dynamic_bind_rc(int sock, struct sockaddr_rc *sockaddr, uint8_t *port)
{
    int ret_err = -1; /* default fail */
    for( *port = 1; *port <= 31; (*port) ++ ) {
        sockaddr->rc_channel = *port;
        int err = bind(sock, (struct sockaddr *)sockaddr, sizeof(sockaddr));
        if( ! err ) {
                ret_err = 0; /* ok */
                break;
        }
        if( errno == EINVAL ) {
                break;
        }
    }
    if( *port == 31 || ret_err ) {
        ret_err = -1;
        errno = EINVAL;
    }
    return ret_err;
}

int main(int argc, char **argv)
{
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[4096] = { 0 };
    int s, client, bytes_read;
    socklen_t opt = sizeof(rem_addr);
    uint8_t port_returned = 0;
    int err;
    unsigned int total_received = 0;

    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if ( s <= 0 ) {
        fprintf(stderr, "    Error: socket failed\n");
        exit(1);
    }

    // bind socket to port 1 of the first available
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    /* bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr)); */
    err = dynamic_bind_rc(s, &loc_addr, &port_returned);
    if ( err || (port_returned < 1 || port_returned > 30) ) {
        fprintf(stderr, "    Error: dynamic bind failed\n");
        close(s);
        exit(1);
    }
    fprintf(stderr, "  opened channel %u\n", port_returned);

    sdp_session_t *sdp_sess = register_service(port_returned);
    if ( sdp_sess == NULL ) {
        fprintf(stderr, "    Error: dynamic bind failed\n");
        close(s);
        exit(1);
    }

    // put socket into listening mode
    listen(s, 1);

    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);
    if ( client <= 0 ) {
        fprintf(stderr, "    Error: accept failed\n");
        close(s);
        sdp_close( sdp_sess );
        exit(1);
    }

    memset(buf, 0, sizeof(buf));
    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "  accepted connection from %s\n", buf);

    // read data from the client
    while(1) {
        memset(buf, 0, sizeof(buf));
        bytes_read = read(client, buf, sizeof(buf));
        if( bytes_read > 0 ) {
            int wlen = 0;
            total_received += bytes_read;
            if ( bytes_read > sizeof(buf)-1 ) {
                buf[sizeof(buf)-1] = 0;
                fprintf(stderr, "    Warning: bytes_read %d\n", bytes_read);
            }
            fprintf(stderr, "   received %d [%s]\n", bytes_read, buf);
            memset(buf, 0, sizeof(buf));
            wlen = snprintf(buf, sizeof(buf)-1, "received bytes %d\n", bytes_read);
            if ( wlen > 0 && wlen < sizeof(buf) ) {
                write(client, buf, wlen);
                fprintf(stderr, "   acknowledge %d [%s]\n", wlen, buf);
            }
        } else {
            fprintf(stderr, "  received closing\n");
            break;
        }
    }

    // close connection
    close(client);
    close(s);
    sdp_close( sdp_sess );

    fprintf(stderr, "  total received %u\n", total_received);
    fprintf(stderr, "  finished ok\n");
    return 0;
}


