//
// Created by Yann Holme Nielsen on 21/11/2019.
//

#include "server.h"
#include "snowflake.h"
void trimTrailing(char *str) {
    int index, i;
    index = -1;
    i = 0;
    while (str[i] != '\0') {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r') {
            index = i;
        }

        i++;
    }
    str[index + 1] = '\0';
}

void *connection_handler(void *socket_handler) {

    char buffer[1024] = {0};
    int valread;
    struct message_payload payload = *(struct message_payload *) socket_handler;

    if ((valread = read(payload.connection_id, buffer, 1024)) == 0) {
        close(payload.connection_id);
        client_socket[payload.index] = 0;
        printf("Resetting client %d\n", payload.index);
    } else {
        buffer[valread] = '\0';
        trimTrailing(buffer);
        if (strcmp(buffer, "close") == 0) {
            printf("Message: %s\n", buffer);
            close(payload.connection_id);
            client_socket[payload.index] = 0;
            printf("Resetting client %d\n", payload.index);

        } else {
            char out[200];
            long int id = get_id();
            sprintf(out, "%ld\n", id);
            send(payload.connection_id, out, strlen(out), 0);

        }
    }
    pthread_exit(NULL);
}

int start_server() {
    int opt = TRUE;
    int master_socket, addrlen, new_socket,
            max_clients = 30, activity, i, sd;
    int max_sd;
    struct sockaddr_in address;
    fd_set readfds;


    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
                   sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (TRUE) {
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_sd)
                max_sd = sd;
        }
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket,
                   inet_ntoa(address.sin_addr), ntohs
                   (address.sin_port));
            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                } else {
                    printf("%d is %d\n", i, client_socket[i]);
                }
            }
            continue;
        }
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                struct message_payload payload;
                payload.index = i;
                payload.connection_id = sd;
                pthread_t thread;
                pthread_create(&thread, NULL, connection_handler, &payload); // no parentheses here
                pthread_join(thread, NULL);
                break;
            }
        }
    }
    return 0;
}