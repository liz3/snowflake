#include <stdio.h>
#include "src/snowflake.h"
#include "src/server.h"
int main() {
    init(1,1);
    start_server();
    return 0;
}