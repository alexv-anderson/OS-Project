/*
 * File:   log.c
 * Author: Alex Anderson
 * Date:   10-26-14
 * Notes:  Logs c-strings into a log file.
 */

#ifndef LOG_C
#define LOG_C

#include <unistd.h>
#include <string.h>

int log_fd = -1;
const char nl = '\n';
char* log_filename = "foo.txt";

//logs a single c-string
void log_info(char* info)
{
   //if the log file has not been initialized
   if(log_fd == -1)
      log_fd = open(log_filename, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

   //if the log file has been initialized
   if(log_fd != -1)
   {
      write(log_fd, info, strlen(info)); //write
      fsync(log_fd); //flush buffer
   }
}

//logs single c-string and ensures a newline after the information
void log_line(char* info)
{
   //if the log file has not been initialized
   if(log_fd == -1)
      log_fd = open(log_filename, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

   //if the log file has benn initialized
   if(log_fd != -1)
   {
      int len = strlen(info);
      write(log_fd, info, len); //write

      //ensure a new line is started
      if(info[len-1] != '\n')
         write(log_fd, &nl, 1);

      fsync(log_fd); //flush buffer
   }
}


#endif //LOG_C
