#include "project.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> 

#define BUFFER_SIZE 1000
#define LOG_SUCCESS (printf("SUCCESS\n"))
#define LOG_FAILURE(cmd) (printf("Invalid command %c\n", cmd))
#define LOG_ARG_FAILURE (printf("Invalid argument(s)\n"))
#define LOG_DRIVER_ALREADY_ERR (printf("Driver already added\n"))
#define LOG_DRIVER_NOT_FOUND_ERR (printf("Driver not found\n"))
#define LOG_WRITE_ERR (printf("Write to file failed\n"))
#define LOG_LOAD_ERR (printf("Failed to load from file\n"))
#define LOG_PRINT_ERR (printf("Print failed\n"))
#define LOG_SEC_MIN_ERR (printf("Seconds and minutes must be in the range of [0, 59]\n"))
#define LOG_HOUR_NEG_ERR (printf("Hours cannot be negative\n"));
#define LOG_NUM_ARG_ERR(n) (printf("That command must be followed by exactly %d arguments\n", n))

uint32_t numOfDrivers = 0;
int numOfCmdArgs = 0;

void freeDrivers(Driver** driverArr) {
    for (uint32_t i = 0;i<numOfDrivers;i++) {
        free(driverArr[i]->surname);
        free(driverArr[i]->team);
        free(driverArr[i]);
    }
    free(driverArr);
    numOfDrivers = 0;
}
void** allocDriver(Driver** driverArr) {
    driverArr = (Driver**)realloc(driverArr, sizeof(Driver*)*numOfDrivers);
    driverArr[numOfDrivers-1] = (Driver*)malloc(sizeof(Driver));
    driverArr[numOfDrivers-1]->surname = (char*)malloc(300);
    driverArr[numOfDrivers-1]->team = (char*)malloc(300);
    return (void**)driverArr;
}

int cmpFunc (const void * a, const void * b) {
    Driver* d1 = *(Driver**)a;
    Driver* d2 = *(Driver**)b;
    return ((d1->hour*3600 + d1->minute*60 + d1->second) - (d2->hour*3600 + d2->minute*60 + d2->second));
}

int fprintfDrivers(Driver** driverArr, FILE* f) {
    if (!f) {return -1;}
    char str[BUFFER_SIZE];
    qsort(driverArr, numOfDrivers, sizeof(Driver*), cmpFunc);
    for (uint32_t i = 0;i<numOfDrivers;i++) {
        sprintf(str, "%s %s %u %u %u\n", driverArr[i]->surname, driverArr[i]->team, driverArr[i]->hour, driverArr[i]->minute, driverArr[i]->second);
        fprintf(f, "%s", str);
    }
    return 0;
}
int checkForDriver(Driver** driverArr, char* surname) {
    for (uint32_t i = 0;i<numOfDrivers;i++) {
        if (!strcmp(surname, driverArr[i]->surname)) {return i;}
    }
    return -1;
}

int checkCmdArgs(char cmd_space) {
    return (!(cmd_space != ' ' && cmd_space != '\n'));
}

char** splitString(const char* str, const char* split) {
    char** strings = (char**)calloc(BUFFER_SIZE, sizeof(char*));
    for (int i = 0;i<64;i++) {
        strings[i] = (char*)calloc(300, sizeof(char));
    }
    int str_count = 0;
    int split_len = 0;
    while (split[split_len] != '\0') {split_len++;}
    char* p = strstr(str, split);
    if (!p) {
        numOfCmdArgs = 0;
        return strings;
    }
    if (p != str) {
        int i = 0;
        while (str[i] != *p) {
            strings[0][i] = str[i];
            i++;
        }
        str_count++;
    }
    int i = 0;
    char* split_p = NULL;
    p = &p[split_len];
    split_p = strstr(p, split);
    while (split_p != NULL) {
        i = 0;
        while (&p[i] != split_p) {
            strings[str_count][i] = p[i];
            i++;
        }
        p = &p[i+split_len];
        strings[str_count][i] = '\0';
        str_count++;
        split_p = strstr(p, split);
    }
    i = 0;
    while (p[i] != '\0') {
        strings[str_count][i] = p[i];
        i++;
    }
    strings[str_count][i] = '\0';
    str_count++;
    free(strings[str_count]);
    strings[str_count] = NULL;

    numOfCmdArgs = str_count - 1;

    char *chr;
    for (int j = 1;j<str_count;j++) {
        chr = strings[j];
        if (*chr == '\0' || *chr == ' ' || *chr == '\n') {
            numOfCmdArgs--;
        }
    }
    return strings;
}

void freeSplitString(char** split_string) {
    for (int i = 0;i<64;i++) {
        free(split_string[i]);
    }
    free(split_string);
}


Driver** loadDrivers(const char* filename, Driver** oldDriverArr) {
    FILE *f = fopen(filename, "r");
    freeDrivers(oldDriverArr);
    long num_of_lines = 0;
    char _surname[300];
    char _team[300];
    uint32_t _hour = 0;
    uint8_t _minute = 0;
    uint8_t _second = 0; 
    while(!feof(f)) {
        num_of_lines++;
        fscanf(f, "%s %s %u %hhu %hhu\n", _surname, _team, &_hour, &_minute, &_second);
    }
    rewind(f);
    Driver** driverArr = (Driver**)malloc(num_of_lines*sizeof(Driver*));
    for (int i = 0;i<num_of_lines;i++) {
        driverArr[i] = (Driver*)malloc(sizeof(Driver));
        driverArr[i]->surname = (char*)malloc(300);
        driverArr[i]->team = (char*)malloc(300);
    } 
    char surname[300];
    char team[300];
    uint32_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0; 
    for (int i = 0;i<num_of_lines;i++) {
        fscanf(f, "%s %s %u %hhu %hhu\n", surname, team, &hour, &minute, &second);
        strcpy(driverArr[i]->surname, surname);
        strcpy(driverArr[i]->team, team);
        driverArr[i]->hour = hour;
        driverArr[i]->minute = minute;
        driverArr[i]->second = second;
        numOfDrivers++;
    }
    fclose(f);
    return driverArr;
}

int checkForFollowingWhitespaces(char* str) {
    char* c = str;
    c++;
    while (*c != '\0') {
        if (!(*c == '\n' || *c == ' ')) {return 0;}
        c++;
    }
    return 1;
}

int checkFile(char* filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

int main(void) {
    Driver** driverArr = (Driver**)malloc(sizeof(Driver*));
    char buffer[BUFFER_SIZE];
    char cmd, cmd_space;
    char** cmd_split;
    int driverIndex = -1;
    while (1) {
        numOfCmdArgs = 0;
        fgets(buffer, BUFFER_SIZE, stdin);
        cmd = buffer[0];
        cmd_space = buffer[1];
        switch (cmd) {
            case 'A':
                if (!checkCmdArgs(cmd_space)) {
                    LOG_ARG_FAILURE;
                    break;
                }
                cmd_split = splitString(buffer, " ");
                if (numOfCmdArgs != 2) {
                    LOG_NUM_ARG_ERR(2);
                    freeSplitString(cmd_split);
                    break;
                }
                char* _surname = cmd_split[1];
                char* _team = cmd_split[2];
                _team[strlen(_team) - 1] = '\0'; //remove newline
                if (checkForDriver(driverArr, _surname) != -1) {
                    freeSplitString(cmd_split);
                    LOG_DRIVER_ALREADY_ERR;
                    break;
                }
                numOfDrivers++;
                driverArr = (Driver**)allocDriver(driverArr);
                strcpy(driverArr[numOfDrivers-1]->surname, _surname);
                strcpy(driverArr[numOfDrivers-1]->team, _team);
                driverArr[numOfDrivers-1]->hour = 0;
                driverArr[numOfDrivers-1]->minute = 0;
                driverArr[numOfDrivers-1]->second = 0;
                freeSplitString(cmd_split);
                LOG_SUCCESS;
                break;
            case 'L':
                if (!checkForFollowingWhitespaces(buffer)) {
                    LOG_NUM_ARG_ERR(0);
                    break;
                }
                if (fprintfDrivers(driverArr, stdout) == -1) {
                    LOG_PRINT_ERR;
                    break;
                }
                LOG_SUCCESS;
                break;
            case 'W':
                if (!checkCmdArgs(cmd_space)) {
                    LOG_ARG_FAILURE;
                    break;
                }
                cmd_split = splitString(buffer, " ");
                if (numOfCmdArgs != 1) {
                    LOG_NUM_ARG_ERR(1);
                    freeSplitString(cmd_split);
                    break;
                }
                cmd_split[1][strlen(cmd_split[1]) - 1] = '\0'; //remove newline
                FILE* _file = fopen(cmd_split[1], "w");
                if(fprintfDrivers(driverArr, _file) == -1) {LOG_WRITE_ERR;}
                fclose(_file);
                freeSplitString(cmd_split);
                LOG_SUCCESS;
                break;
            case 'O':
                if (!checkCmdArgs(cmd_space)) {
                    LOG_ARG_FAILURE;
                    break;
                }
                cmd_split = splitString(buffer, " ");
                if (numOfCmdArgs != 1) {
                    LOG_NUM_ARG_ERR(1);
                    freeSplitString(cmd_split);
                    break;
                }
                char* _filename = cmd_split[1];
                _filename[strlen(_filename) - 1] = '\0'; //remove newline
                if (!checkFile(_filename)) {
                    LOG_LOAD_ERR;
                    freeSplitString(cmd_split);
                    break;
                }
                driverArr = loadDrivers(_filename, driverArr);
                freeSplitString(cmd_split);
                LOG_SUCCESS;
                break;
            case 'U':
                if (!checkCmdArgs(cmd_space)) {
                    LOG_ARG_FAILURE;
                    break;
                }
                cmd_split = splitString(buffer, " ");
                if (numOfCmdArgs != 4) {
                    LOG_NUM_ARG_ERR(4);
                    freeSplitString(cmd_split);
                    break;
                }
                _surname = cmd_split[1];
                driverIndex = checkForDriver(driverArr, _surname);
                if (driverIndex == -1) {
                    LOG_DRIVER_NOT_FOUND_ERR;
                    freeSplitString(cmd_split);
                    break;
                }

                int _hour = atoi(cmd_split[2]);
                int _minute = atoi(cmd_split[3]);
                int _second = atoi(cmd_split[4]);

                if (_minute > 59 || _second > 59 || _minute < 0 || _second < 0) {
                    LOG_SEC_MIN_ERR;
                    freeSplitString(cmd_split);
                    break;
                }

                if (_hour < 0) {
                    LOG_HOUR_NEG_ERR;
                    freeSplitString(cmd_split);
                    break;
                }

                strcpy(driverArr[driverIndex]->surname, _surname);

                driverArr[driverIndex]->hour += _hour;

                if (driverArr[driverIndex]->minute + _minute > 59) {driverArr[driverIndex]->hour++;}
                driverArr[driverIndex]->minute = (driverArr[driverIndex]->minute + _minute) % 60;

                if (driverArr[driverIndex]->second + _second > 59) {driverArr[driverIndex]->minute++;}
                driverArr[driverIndex]->second = (driverArr[driverIndex]->second + _second) % 60;

                freeSplitString(cmd_split);
                LOG_SUCCESS;
                break;
            case 'Q':
                if (!checkForFollowingWhitespaces(buffer)) {
                    LOG_NUM_ARG_ERR(0);
                    break;
                }
                freeDrivers(driverArr);
                LOG_SUCCESS;
                return 0;
            default:
                LOG_FAILURE(cmd);
                break;
        }
    }
}