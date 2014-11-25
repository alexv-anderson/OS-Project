/*
 * Author: Alex Anderson
 * ID #:   010620309
 * Date:   9-28-14
 * Class:  CSCE 3613: Operating Systems
 *
 * Notes:     Implements the parse_command
 *         function to be used by main.c
 */

#include <stdio.h>
#include <string.h>

//Delimiter for parsing the command string
#define DELIMITER " "

//function used by main.c to parse command strings
int pase_command(char* line, char** cmd1, char** cmd2, char* infile, char* outfile);

//function used by parse_command to parse command options
int parseOption(char** option);

int parse_command(char* line,
		  char** cmd1, char** cmd2,
		  char* infile, char* outfile)
{
   //initialize strtok
   char* token = strtok(line, DELIMITER);

   if(token != NULL)
   {
      //assume the first argument is a command
      cmd1[0] = token;

      //if the command is quit the function is done
      if(strcmp(token, "quit") == 0)
         return 0;
   }

   //flags used trackt the presence of pipes and redirections
   short pipe = 0;
   short outRed = 0;	// (outRed < 0) ? ">" : ">>"

   //initialize variables used in the loop
   char** curCmd = cmd1;
   int i = 1;
   int done = 0;

   while(done == 0)
   {
      //parse an option
      char* option = NULL;
      int optCode = parseOption(&option);

      //use the option code to store the option correctly
      if(optCode == 0)
      {
         //no option was found
         done = 1;
      }
      else if(optCode == 1)
      {
         //regular option found
         curCmd[i] = option;
         i++;
      }
      else if(optCode == 2)
      {
         //pipe
         curCmd = cmd2;
         pipe = 1;
         i = 0;
      }
      else if(optCode == 3)
         strcpy(infile, option);
      else if(optCode == 4)
      {
         //Output redirection overwrite (>)
         strcpy(outfile, option);
         outRed = -1;
      }
      else if(optCode == 5)
      {
         //Output redirection append (>>)
         strcpy(outfile, option);
         outRed = 1;
      }
      else
      {
         //An unexpected option code was returned
         done = 1;
      }
   }

   //build up the return code according to the flags
   int retCode = 1;
   if(infile[0] != '\0')
      retCode = 2;
   if(outRed == -1)
      retCode = 4;
   if(outRed == 1)
      retCode = 3;

   if(pipe != 0)
      retCode += 4;

   return retCode;
}

//Parses the options to a command
//returns:0 if no option was found
//        1 if an option was found
//        2 if a pipe was found
//        3 if a input redirection was found
//        4 if a output redirection was found
//        5 if a output append was found
int parseOption(char** option)
{
   char* token = strtok(NULL, DELIMITER);
   int retCode = 0;

   //if a token could be parsed from the command string
   if(token != NULL)
   {
      //if a pipe is detected
      if(strcmp(token, "|") == 0)
      {
         retCode = 2;
      }
      else if(strcmp(token, "<") == 0)
      {
         //if a input redirection was detected, set option to the filename
         *option = strtok(NULL, DELIMITER);
         retCode = 3;
      }
      else if(strcmp(token, ">") == 0)
      {
         //if a input redirection was detected, set option to the filename
         *option = strtok(NULL, DELIMITER);
         retCode = 4;
      }
      else if(strcmp(token, ">>") == 0)
      {
         //if a input redirection was detected, set option to the filename
         *option = strtok(NULL, DELIMITER);
         retCode = 5;
      }
      else
      {
         //The option is just a standard option
         *option = token;
         retCode = 1;
      }
   }

   return retCode;
}
