#ifndef LAB3_PROXY_LOGGER_H
#define LAB3_PROXY_LOGGER_H

#include <string.h>
#include <pthread.h>
#include <stdio.h>

#define RESET "\033[0m"
#define RED   "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE  "\033[1;34m"
#define PURPLE  "\033[1;35m"
#define BACK_PURP "\033[1;45m"

void logg(char *msg, char *color);
void logg_char(char *msg, char *info, char *color);
void logg_int(char *msg, long info, char *color);

#endif //LAB3_PROXY_LOGGER_H
