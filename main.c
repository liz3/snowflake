#include <stdio.h>
#include "src/snowflake.h"
#include "src/server.h"
int main(int argc, char* argv[]) {
    int region_id_x;
    int worker_id_x;
    if(argc != 3) {
        puts("Wrong amount of arguments, provide region id and worker id\n");
        return 1;
    }
    sscanf(argv[1], "%d", &region_id_x);
    sscanf(argv[2], "%d", &worker_id_x);
    printf("Starting server with region id %d and worker id %d\n", region_id_x, worker_id_x);
    init(region_id_x,worker_id_x);
    start_server();
    return 0;
}