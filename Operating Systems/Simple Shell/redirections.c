/*
 * File:   redirections.c
 * Author: Alex Anderson
 * Date:   10-26-14
 * Notes:  Handles input and output redirections
 *            and restorations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

/*
 * Perform a input redirection and return file descriptors
 *    for stdin and the input file through stdin_loc and
 *    input_loc respectively.
 * Returns  1 if successful
 *         -1 if unsuccessful
 */
int redirIn(char* infile, int* stdin_loc, int* input_loc)
{
   //open input file
   int fd = open(infile, O_RDONLY);
   if(fd == -1)
   {
      printf("Could not open file %s\n", infile);
      return -1;
   }

   //save stdin
   int stdin_temp = dup(STDIN_FILENO);
   if(stdin_temp == -1)
   {
      close(fd);
      return -1;
   }

   //duplicate
   int success = dup2(fd, STDIN_FILENO);
   if(success == -1)
   {
      close(stdin_temp);
      close(fd);
      return -1;
   }

   *stdin_loc = stdin_temp;
   *input_loc = fd;

   return 1;
}

/*
 * Perform output redirection (append) and returns file descriptors
 *    for stdout and the output file through stdout_loc and
 *    output_loc respectively
 * Returns  1 if successful
 *         -1 if unsuccessful
 */
int redirOutAppend(char* outfile, int* stdout_loc, int* output_loc)
{
   //open output file
   int fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
   if(fd == -1)
   {
      printf("Could not open file %s\n", outfile);
      return -1;
   }

   //save stdout
   int stdout_temp = dup(STDOUT_FILENO);
   if(stdout_temp == -1)
   {
      close(fd);
      return -1;
   }

   //duplicate
   int success = dup2(fd, STDOUT_FILENO);
   if(success == -1)
   {
      close(stdout_temp);
      close(fd);
      return -1;
   }

   *stdout_loc = stdout_temp;
   *output_loc = fd;

   return 1;
}

/*
 * Performs ouput redirection (overwrite) and returns file
 *    descriptors for stdout and the output file through
 *    stdout_loc and output_loc respectively.
 * Returns  1 if successful
 *         -1 if unsuccessful
 */
int redirOut(char* outfile, int* stdout_loc, int* output_loc)
{
   //open file
   int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
   if(fd == -1)
   {
      printf("Could not open file %s\n", outfile);
      return -1;
   }

   //save stdout
   int stdout_temp = dup(STDOUT_FILENO);
   if(stdout_temp == -1)
   {
      close(fd);
      return -1;
   }

   //duplicate
   int success = dup2(fd, STDOUT_FILENO);
   if(success == -1)
   {
      close(stdout_temp);
      close(fd);
      return -1;
   }

   *stdout_loc = stdout_temp;
   *output_loc = fd;

   return 1;
}

/*
 * Restores an input redirection and closes the input file
 */
void resIn(int stdin_curr, int input)
{
   //restore stdin
   int success = dup2(stdin_curr, STDIN_FILENO);
   if(success == -1)
      printf("Could not restore stdin file descriptor\n");

   close(stdin_curr);
   if(input > 2)
      close(input);
}

/*
 * Restores an output redirection and closes the output file
 */
void resOut(int stdout_curr, int output)
{
   //restore stdout
   int success = dup2(stdout_curr, STDOUT_FILENO);
   if(success == -1)
      printf("Could not restore stdout file descriptor\n");

   close(stdout_curr);
   if(output > 2)
      close(output);
}
