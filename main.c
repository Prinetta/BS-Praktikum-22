#include "main.h"
#include <stdio.h>
#include "keyValStore.h"
#include "sub.h"

int main() {
    startServer();
    put("key", "value");
    char value[10] = "";
    get("key", value);
    //printf("%s", value);
    del("key");
    get("key", value);
    printf("%s", value);
    return 0;
}