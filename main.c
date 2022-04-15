#include "main.h"
#include <stdio.h>
#include "keyValStore.h"

int main(){
    printf("Hello World!\n");
    printf( "%d", put("key", "value"));
    printf( "%d", get("key", "value"));
    return 0;
}