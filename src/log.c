#include "log.h"
#include "stdheader.h"

/* logs error into a file stored in the /tmp directory
 * Returns 0 on failing to open file
 * return 1 on success */
int logerror(char *filename, int line, const char *funcname, char *msg) {
  /* Declaration */
  int fd;
  char timemsg[64];
  char total_msg[LOG_MSG_SIZE];
  time_t t;
  struct tm datetime;

  /* Initialization */
  t = time(NULL);
  datetime = *localtime(&t);

  /* concatenate date time and the total message */
  strftime(timemsg, sizeof(timemsg), "%c", &datetime);
  snprintf(total_msg, sizeof(total_msg), "[%s]%s: Line %d: Function %s-> %s\n",
           timemsg, filename, line, funcname, msg);

  /* open file and write error to it */
  fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0666);
  if (fd < 0) {
    fprintf(stderr, "Error accessing log file\n");
    return 0;
  }
  write(fd, total_msg, strlen(total_msg));

  /* close file descriptor */
  close(fd);

  return 1;
}
