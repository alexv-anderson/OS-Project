/*
 * Author: Alex Anderson
 * Date: 10-26-14
 * Notes: Contains the portion of the
 *           code which handles the user
 *           input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "execute.c"
#include "log.c"

int parse_command(char* line,
		  char** cmd1, char** cmd2,
		  char* infile, char* outfile);

int handleCommand(char* line);

int main(int argc, char* argv[])
{
   char buff[256];

   sprintf(buff, "Main process PID=%d\n", getpid());
   log_line(buff);

   if(argc > 1)
      handleCommand(argv[1]);
   else
   {
      int retCode = 1;

      //continue to process while the return code is in the valid range
      while(retCode > 0 && retCode < 9)
      {
         log_info("\nWaiting for user input...\t");

         //get user input
         char line[100];
         printf("myshell-%% ");
         gets(line);

         log_line(line);

         //handle user input
         retCode = handleCommand(line);
      }
   }

   return 0;
}

/*
 * Handles user input and begin executing commands as needed
 */
int handleCommand(char* line)
{
   //set aside and initialize memory for the commands
   const int CMD_SIZE = 100;
   char** cmd1 = malloc(CMD_SIZE * sizeof(char*));
   char** cmd2 = malloc(CMD_SIZE * sizeof(char*));

   int i = 0;
   for(i = 0; i < CMD_SIZE; i++)
   {
      cmd1[i] = NULL;
      cmd2[i] = NULL;
   }

   //initialize memory for the in and out files
   char infile[100];
   char outfile[100];

   infile[0] = '\0';
   outfile[0] = '\0';

   //parse the command line from the user
   int ret = parse_command(line, cmd1, cmd2, infile, outfile);

   //   Use the return code from parse_command
   //to determine which senerio should be performed
   switch(ret)
   {
      case 0:   //Quit terminal
         break;
      case 1:   //Simple command
         exec_cmd(cmd1);
         break;
      case 2:   //Simple command with input redirection
         exec_cmd_in(cmd1, infile);
         break;
      case 3:   //Simple command with output redirection (append)
         exec_cmd_opt_in_append(cmd1, infile, outfile);
         break;
      case 4:   //Simple command with output redirection (overwrite)
         exec_cmd_opt_in_write(cmd1, infile, outfile);
         break;
      case 5:   //Two commands piped
         exec_pipe(cmd1, cmd2);
         break;
      case 6:   //Two commands piped with input redirection
         exec_pipe_in(cmd1, cmd2, infile);
         break;
      case 7:   //Two commands piped with output redirection (append)
         exec_pipe_opt_in_append(cmd1, cmd2, infile, outfile);
         break;
      case 8:   //Two commands piped with output redirection (overwrite)
         exec_pipe_opt_in_write(cmd1, cmd2, infile, outfile);
         break;
      default:   //parse_command returned a bad code
         printf("Not handled at this time!\n");
   }

   //free the allocated memory
   free(cmd1);
   free(cmd2);

   return ret;
}
