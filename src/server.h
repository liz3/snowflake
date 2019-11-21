//
// Created by Yann Holme Nielsen on 21/11/2019.
//

#ifndef SNOWFLAKE_SERVER_H
#define SNOWFLAKE_SERVER_H

#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <pthread.h>


#define TRUE   1
#define FALSE  0
#define PORT 8888

struct message_payload {
    int index;
    int connection_id;

};
int client_socket[30];
int start_server();
#endif //SNOWFLAKE_SERVER_H
