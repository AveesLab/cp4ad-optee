#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <tee_client_api.h>
#include "ta.h"

int main(int  argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    uint32_t err_origin;

    /*
    [Initialize the context]
        The context is used to manage the connection to the TEE
        and to handle the sessions with 
        The context is created with the TEEC_InitializeContext function
        The first parameter is NULL, which means that the context is created
        in the default mode (i.e. no special flags)
        The second parameter is a pointer to the context structure
        The function returns a TEEC_Result value, which indicates the success or failure of the operation
        If the function fails, it returns a TEEC_Result value that indicates the error
        The TEEC_InitializeContext function initializes the context
        and prepares it for use with the TEE
    */
    TEEC_InitializeContext(NULL, &ctx);
    printf("[Initialized Context!]\n");

    /*
    [Set the UUID]
        The UUID is a unique identifier for the TA
        The UUID is defined in the ta.h file
        The UUID is used to identify tha TA
            when opening a session
            when invoking a command
            when closing a session
            when finalizing the context
            when allocating memory
            when freeing memory
            when creating and destroying [thread, mutex, condition variable, semaphore, event, timer, 
                                            file, directory, socket, pipe, message queue, 
                                            shared memory region, shared memory pool, 
                                            shared memory segment, shared memory object]
    */
    TEEC_UUID uuid = TA_UUID;

    /*
    [Open a session]
        The session is used to communicate with the TA
        The session is created with the TEEC_OpenSession function
        The first parameter is a pointer to the context structure
        The second parameter is a pointer to the session structure
        The third parameter is a pointer to the UUID of the TA
        The fourth parameter is the login type (public or secure)
        The fifth parameter is a pointer to the parameters for the session
        The sixth parameter is a pointer to the session ID
        The function returns a TEEC_Result value, which indicates the success or failure of the operation
        If the function fails, it returns a TEEC_Result value that indicates the error
        The TEEC_OpenSession function opens a session with the TA
        and prepares it for use with the TEE
    */
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    printf("[Session Opened!]\n");

    /*
    [Set Parameters]
        The parameters are defined in the ta.h file
        The parameters are used to pass data to the TA
        The parameters are used to pass data from the TA
    */
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    /*
    [Invoke the command]
        The command is defined in the ta.h file
        The command is used to execute a function in the TA
        The command is used to pass data to the TA
        The command is used to pass data from the TA
    */
    TEEC_InvokeCommand(&sess, CMD_HELLO, &op, &err_origin); // (CMD_HELLO, CMD_GOODBYE)
    int res = op.params[0].value.a;
    if (res == 0) {
        printf("Hello OP-TEE World!\n");
    }
    else {
        printf("Goodbye OP-TEE World!\n");
    }

    /*
    [Close the session]
        The session is closed with the TEEC_CloseSession function
        The first parameter is a pointer to the session structure
        The function returns a TEEC_Result value, which indicates the success or failure of the operation
        If the function fails, it returns a TEEC_Result value that indicates the error
        The TEEC_CloseSession function closes the session
        and prepares it for use with the TEE
        The TEEC_CloseSession function frees the resources used by the session
    */
    TEEC_CloseSession(&sess);
    printf("[Closed Session!]\n");
    
    /*
    [Finalize the context]
        The context is finalized with the TEEC_FinalizeContext function
        The first parameter is a pointer to the context structure
        The function returns a TEEC_Result value, which indicates the success or failure of the operation
        If the function fails, it returns a TEEC_Result value that indicates the error
        The TEEC_FinalizeContext function finalizes the context
        and frees the resources used by the context
        The TEEC_FinalizeContext function frees the resources used by the context
    */
    TEEC_FinalizeContext(&ctx);
    printf("[Finalized Context!]\n");
    return 0;
}