#ifndef _PROJECT__H_
#define _PROJECT__H_

#include <stdio.h>
#include <stdint.h> 

typedef struct {
    char* surname;
    char* team;
    uint32_t hour;
    uint8_t minute;
    uint8_t second;
} Driver;

void freeDrivers(Driver** driverArr);
void** allocDriver(Driver** driverArr);
int cmpFunc (const void * a, const void * b);
int fprintfDrivers(Driver** driverArr, FILE* f);
int checkForDriver(Driver** driverArr, char* surname);
char** splitString(const char* str, const char* split);
void freeSplitString(char** split_string);
Driver** loadDrivers(const char* filename, Driver** oldDriverArr);
int checkCmdArgs(char cmd_space);
int checkFile(char* filename);
int checkForFollowingWhitespaces(char* str);

#endif //! _PROJECT__H_