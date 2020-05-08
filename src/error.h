#ifndef ERROR_H
#define ERROR_H

#define LOGPATH "log.txt"

void warning(char* src, char* msg);
void append_log(char* path, char* src, char* msg);
void fatal(char* src, char* msg, int ret);

char* current_time();

#endif
