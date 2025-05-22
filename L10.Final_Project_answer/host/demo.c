#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"

#include "secure_storage.h"
#include "crypto.h"
#include "can.h"

void process(TEEC_Session sess, struct can_frame frame) {
    if(frame.can_id == 0x123) {
        printf("--------------------------------------------\n");
        printf("message ID: %x\n", frame.can_id);
        print_bytes(frame.data, 8);
        
        unsigned char buffer[5];
        memcpy(buffer, frame.data, sizeof(buffer));
        unsigned char signature[3];
        micro_sign(&sess, buffer, signature);
        printf("Message     : ");
        for (int i = 0; i < 5; i++) printf("%02X ", frame.data[i]);
        printf("\nSignature   : %02X %02X %02X\n\n", signature[0], signature[1], signature[2]);
    }
}


int main(int argc, char *argv[]) {
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_UUID uuid = {0x4014bf39, 0x668c, 0x4253, {0x90, 0xb9, 0x70, 0x4b, 0x82, 0x57, 0xd1, 0x29}};
    uint32_t origin;

    TEEC_InitializeContext(NULL, &ctx);
    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (res == TEEC_SUCCESS) {
        printf("session opened successfully!\n");
    }
    else {
        printf("session error!\n");
        return -1;
    }

    struct can_frame frame;
    frame.can_dlc = 8;
    int recvbytes = 0;
    open_port("can0");
    while(1)
    {
        struct timeval timeout = {1, 0};
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(soc, &readSet);
        if (select((soc + 1), &readSet, NULL, NULL, &timeout) >= 0) {
            if (FD_ISSET(soc, &readSet)) {
                recvbytes = read(soc, &frame, sizeof(struct can_frame));
                if(recvbytes) {
                    process(sess, frame);
                }
            }
        }
    }
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}