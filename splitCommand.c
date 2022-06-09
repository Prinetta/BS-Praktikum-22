//
// Created by Daze on 08.06.2022.
//

#include <string.h>
#include <malloc.h>

char** splitCommand(char * string) { // works with space at end of command
    char ** substrings = malloc(3 * sizeof(char *));
    for (int i = 0; i < 3; ++i) {
        substrings[i] = malloc(64 * sizeof(char));
        strcpy(substrings[i], "-1");;
    }

    char substring[64] = "";
    int index = 0;
    int length = strlen(string);

    for (int i = 0; i < length && strcmp(substrings[2], "-1") == 0; ++i) {
        if (string[i] < 33 || string[i] > 126) {
            strcpy(substrings[index], substring);
            memset(substring, 0, strlen(substring));
            index++;
        } else {
            strncat(substring, &string[i], 1);
        }
    }
    if (strcmp(substrings[2], "-1") == 0) {
        strcpy(substrings[index], substring);
        memset(substring, 0, strlen(substring));
    }
    return substrings;
}