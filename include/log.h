#ifndef LOG_H
#define LOG_H

#define LOG_FILE "/tmp/error.log"
#define LOG_MSG_SIZE 512

int logerror(char *filename, int line, const char *funcname, char *msg);

#endif // !LOG_H
