/*
 * Author: Alex Anderson
 * Date: 10-26-14
 * Notes: Contains functions which
 *          attempts to execute the
 *          given command(s).
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "redirections.c"
#include "log.c"

//executes a single command
void exec_cmd(char** cmd1);
//single command with <
void exec_cmd_in(char** cmd1, char* infile);
//single command with >> and possibly <
void exec_cmd_opt_in_append(char** cmd1, char* infile, char* outfile);
//single command with > and possibly <
void exec_cmd_opt_in_write(char** cmd1, char* infile, char* outfile);
//two commands piped
void exec_pipe(char** cmd1, char** cmd2);
//pipe with <
void exec_pipe_in(char** cmd1, char** cmd2, char* infile);
//pipe with >> and possibly <
void exec_pipe_opt_in_append(char** cmd1, char** cmd2, char* infile, char* outfile);
//pipe with > and possibly <
void exec_pipe_opt_in_write(char** cmd1, char** cmd2, char* infile, char* outfile);

/*
 * Executes a single command
 */
void exec_cmd(char** cmd1)
{
   log_line("Attempting to execute single command\nAttempting fork()\n");
   char buff[256];

   //fork
   pid_t pid = fork();

   //error occurred
   if(pid < 0)
   {
      log_line("Fork Faild\n");
      return;
   }
   //child process
   else if(pid == 0)
   {
      sprintf(buff, "Child is attempting to execute \"%s\" with execvp()\n", cmd1[0]);
      log_line(buff);

      //execute cmd1
      execvp(cmd1[0], cmd1);

      sprintf(buff, "Child could not find a command or program \"%s\"\n", cmd1[0]);
      log_line(buff);

      log_line("Child is terminating\n");
      exit(0);   //quit child process
   }
   //parent process
   else
   {
      sprintf(buff, "Parent process is waiting. Child's PID=%d\n", pid);
      log_line(buff);

      wait(NULL);

      log_line("Parent process has finished waiting\n");
   }
}

/*
 * Executes a single command with input redirection
 */
void exec_cmd_in(char** cmd1, char* infile)
{
   log_line("Attempting to execute single command with <\nAttempting fork()\n");
   char buff[256];

   //fork
   pid_t pid = fork();

   //error occurred
   if(pid < 0)
   {
      log_line("Fork Faild\n");
      return;
   }
   //child process
   else if(pid == 0)
   {
      sprintf(buff, "Child is applying input redirection from %s\n", infile);
      log_line(buff);

      //input redirection
      int t1, t2;
      redirIn(infile, &t1, &t2);

      sprintf(buff, "Child is attempting to execute \"%s\" with execvp()\n", cmd1[0]);
      log_line(buff);

      //execute cmd1
      execvp(cmd1[0], cmd1);

      sprintf(buff, "Child could not find a command or program \"%s\"\n", cmd1[0]);
      log_line(buff);
      log_line("Child is restoring stdin\n");

      //restore from redirection
      resIn(t1, t2);

      log_line("Child is terminating\n");
      exit(0);	//terminate child process
   }
   //parent process
   else
   {
      sprintf(buff, "Parent process is waiting. Child's PID=%d\n", pid);
      log_line(buff);

      wait(NULL);

      log_line("Parent process has finished waiting\n");
   }
}

/*
 * Executes single command with output redirection (append)
 *    and possibly input redirection.
 */
void exec_cmd_opt_in_append(char** cmd1, char* infile, char* outfile)
{
   log_line("Attempting to execute single command with >> and possibly <\nAttempting fork()\n");
   char buff[256];

   //fork
   pid_t pid = fork();

   //error occurred
   if(pid < 0)
   {
      printf("Fork Faild\n");
      return;
   }
   //child process
   else if(pid == 0)
   {
      int stdin_fd, in_fd, stdout_fd, out_fd;
      if(infile[0] != '\0')
      {
         sprintf(buff, "Child is applying input redirection from %s\n", infile);
         log_line(buff);

         //input redirection
         redirIn(infile, &stdin_fd, &in_fd);
      }

      sprintf(buff, "Child is applying output redirection (append) to %s\n", outfile);
      log_line(buff);

      //output redirection (append)
      redirOutAppend(outfile, &stdout_fd, &out_fd);

      sprintf(buff, "Child is attempting to execute \"%s\" with execvp()\n", cmd1[0]);
      log_line(buff);

      //execute cmd1
      execvp(cmd1[0], cmd1);

      sprintf(buff, "Child could not find a command or program \"%s\"\n", cmd1[0]);
      log_line(buff);

      if(infile[0] != '\0')
      {
         log_line("Child is restoring stdin\n");
         //restore input redirection
         resIn(stdin_fd, in_fd);
      }

      log_line("Child is restoring stdout\n");
      //restore output redirection
      resOut(stdout_fd, out_fd);

      log_line("Child is terminating\n");
      exit(0); //terminate child process
   }
   //parent process
   else
   {
      sprintf(buff, "Parent process is waiting. Child's PID=%d\n", pid);
      log_line(buff);

      wait(NULL);

      log_line("Parent process has finished waiting\n");
   }
}

/*
 * Executes a single command with output redirection (overwrite)
 *    and possibly input redirection.
 */
void exec_cmd_opt_in_write(char** cmd1, char* infile, char* outfile)
{
   log_line("Attempting to execute single command with > and possibly <\nAttempting fork()\n");
   char buff[256];

   //fork
   pid_t pid = fork();

   //error occurred
   if(pid < 0)
   {
      log_line("Fork Faild\n");
      return;
   }
   //child process
   else if(pid == 0)
   {
      int stdin_fd, in_fd, stdout_fd, out_fd;
      if(infile[0] != '\0')
      {
         sprintf(buff, "Child is applying input redirection from %s\n", infile);
         log_line(buff);

         //input redirection
         redirIn(infile, &stdin_fd, &in_fd);
      }

      sprintf(buff, "Child is applying output redirection (overwrite) to %s\n", outfile);
      log_line(buff);

      //output redirection (overwrite)
      redirOut(outfile, &stdout_fd, &out_fd);

      sprintf(buff, "Child is attempting to execute \"%s\" with execvp()\n", cmd1[0]);
      log_line(buff);

      //execute cmd1
      execvp(cmd1[0], cmd1);

      sprintf(buff, "Child could not find a command or program \"%s\"\n", cmd1[0]);
      log_line(buff);

      if(infile[0] != '\0')
      {
         log_line("Child is restoring stdin\n");
         //restore input redirection
         resIn(stdin_fd, in_fd);
      }

      log_line("Child is restoring stdout\n");
      //restore output redirection
      resOut(stdout_fd, out_fd);

      log_line("Child is terminating\n");
      exit(0); //Terminate child process
   }
   //parent process
   else
   {
      sprintf(buff, "Parent process is waiting. Child's PID=%d\n", pid);
      log_line(buff);

      wait(NULL);

      log_line("Parent has finished waiting.\n");
   }
}

/*
 * Executes two commands pipe together
 */
void exec_pipe(char** cmd1, char** cmd2)
{
   log_line("Attempting to execute two piped commands\nAttempting fork()");
   char buff[256];

   //fork
   pid_t pid = fork();

   if(pid < 0)
   {
      log_line("Fork Faild\n");
      return;
   }
   //if this is the child process
   if(pid == 0)
   {
      log_line("Child is saving stdin and stdout descriptors and creating pipe\n");

      //create pipe:	pipe[0] is read, pipe[1] is write
      int pipefd[2];
      int stdout_fd = dup(STDOUT_FILENO);
      int stdin_fd = dup(STDIN_FILENO);

      if(pipe(pipefd) == -1)
      {
         log_line("Child could not create pipe\nChild is treminating\n");
         exit(0);
      }
      else
         log_line("Child created pipe\n");

      //fork
      pid_t pid = fork();

      if(pid < 0)
      {
         log_line("Child fork failed\nChild is terminating\n");
         exit(0);
      }
      //if this is the child process
      if(pid == 0)
      {
         sprintf(buff, "cmd1(PID=%d): Attempting to connect to write end of pipe\n", getpid());
         log_line(buff);

         close(pipefd[0]);
         //put write end of pipe on stdout of cmd1
         if(dup2(pipefd[1], STDOUT_FILENO) == -1)
            sprintf(buff, "cmd1: \"%s\" could not connect the write end of the pipe\n", cmd1[0]);
         else
            sprintf(buff, "cmd1: \"%s\" connected to the write end of the pipe\n", cmd1[0]);

         log_line(buff);

         sprintf(buff, "cmd1: Attempting to execute \"%s\" with execvp()\n", cmd1[0]);
         log_line(buff);

         //execute cmd1
         execvp(cmd1[0], cmd1);

         sprintf(buff, "cmd1: Could not find a command or program \"%s\"\n", cmd1[0]);
         log_line(buff);

         log_line("cmd1: Closing write end of pipe and restoring stdout\n");
         resOut(stdout_fd, pipefd[1]);

         log_line("cmd1: Terminating.\n");
         exit(0);
      }
      //if this is the parent process
      else
      {
         sprintf(buff, "cmd2(PID=%d): Attempting to connect to read end of pipe\n", getpid());
         log_line(buff);

         close(pipefd[1]);
         //put read end of pipe on stdin of cmd2
         if(dup2(pipefd[0], STDIN_FILENO) == -1)
            sprintf(buff, "cmd2: \"%s\" could not connect the read end of the pipe\n", cmd2[0]);
         else
            sprintf(buff, "cmd2: \"%s\" connected to the read end of the pipe\n", cmd2[0]);

         log_line(buff);

         sprintf(buff, "cmd2: Attempting to execute \"%s\" with execvp()\n", cmd2[0]);
         log_line(buff);

         //execute cmd2
         execvp(cmd2[0], cmd2);

         sprintf(buff, "cmd2: Could not find a command or program \"%s\"\n", cmd2[0]);
         log_line(buff);

         log_line("cmd2: Closing read end of pipe and restoring stdin\n");
         resIn(stdin_fd, pipefd[0]);

         log_line("cmd2: Terminating\n");
         exit(0);
      }
   }
   //if this is the parent process
   else
   {
      sprintf(buff, "Parent process is waiting. Child's PID=%d\n", pid);
      log_line(buff);

      wait(NULL);

      log_line("Parent has finished waiting.\n");
   }
}

/*
 * Executes two commands piped together with input redirection
 */
void exec_pipe_in(char** cmd1, char** cmd2, char* infile)
{
   log_line("Attempting to execute two piped commands with <\nAttempting fork()\n");
   char buff[256];

   //fork
   pid_t pid = fork();

   //error occurred
   if(pid < 0)
   {
      log_line("Fork Failed\n");
      return;
   }
   //child process
   else if(pid == 0)
   {
      log_line("Child is saving stdin and stdout descriptors and creating pipe\n");

      //create pipe:	pipe[0] is read, pipe[1] is write
      int pipefd[2];
      int stdout_fd = dup(STDOUT_FILENO);
      int stdin_fd = dup(STDIN_FILENO);

      if(pipe(pipefd) == -1)
      {
         //error occurred
         log_line("Child could not create pipe\nChild is treminating\n");
         exit(0);
      }
      else
         log_line("Child created pipe\n");

      //fork
      pid_t pid1 = fork();

      //error occurred
      if(pid1 < 0)
      {
         log_line("Child fork failed\nChild terminating\n");
         exit(0);
      }
      //cmd1 process
      else if(pid1 == 0)
      {
         sprintf(buff, "cmd1(PID=%d): Attempting to connect to write end of pipe\n", getpid());
         log_line(buff);

         close(pipefd[0]);   //close unneeded end of pipe

         //connect to pipe
         if(dup2(pipefd[1], STDOUT_FILENO) == -1)
            sprintf(buff, "cmd1: \"%s\" could not connect the write end of the pipe\n", cmd1[0]);
         else
            sprintf(buff, "cmd1: \"%s\" connected to the write end of the pipe\n", cmd1[0]);

         log_line(buff);

         sprintf(buff, "cmd1: Applying input redirection from %s\n", infile);
         log_line(buff);

         //input redirection
         int t1, t2;
         redirIn(infile, &t1, &t2);

         sprintf(buff, "cmd1: Attempting to execute \"%s\" with execvp()\n", cmd1[0]);
         log_line(buff);

         //execute cmd1
         execvp(cmd1[0], cmd1);

         sprintf(buff, "cmd1: Could not find a command or program \"%s\"\n", cmd1[0]);
         log_line(buff);

         log_line("cmd1: Restoring stdin\n");
         //restore input redirection
         resIn(t1, t2);

         log_line("cmd1: Closing write end of pipe and restoring stdout\n");
         resOut(stdout_fd, pipefd[1]);

         log_line("cmd1: Terminating\n");
         exit(0);   //terminate cmd1
      }
      //cmd2 process
      else
      {
         sprintf(buff, "cmd2(PID=%d): Attempting to connect to read end of pipe\n", getpid());
         log_line(buff);

         close(pipefd[1]);   //close unneeded end of pipe

         //connect to pipe
         if(dup2(pipefd[0], STDIN_FILENO) == -1)
            sprintf(buff, "cmd2: \"%s\" could not connect the read end of the pipe\n", cmd2[0]);
         else
            sprintf(buff, "cmd2: \"%s\" connected to the read end of the pipe\n", cmd2[0]);

         log_line(buff);

         sprintf(buff, "cmd2: Attempting to execute \"%s\" with execvp()\n", cmd2[0]);
         log_line(buff);

         //execute cmd2
         execvp(cmd2[0], cmd2);

         sprintf(buff, "cmd2: Could not find a command or program \"%s\"\n", cmd2[0]);
         log_line(buff);

         log_line("cmd2: Closing read end of pipe and restoring stdin\n");
         //close pipe and restore stdin
         resIn(stdin_fd, pipefd[0]);

         log_line("cmd2: Terminating\n");
         exit(0);   //terminate cmd2
      }
   }
   //parent process
   else
   {
      sprintf(buff, "Parent process is waiting. Child's PID=%d\n", pid);
      log_line(buff);

      wait(NULL);

      log_line("Parent has finished waiting\n");
   }
}

/*
 * Executes two commands piped together with output redirection (append)
 *    and possibly input redirection
 */
void exec_pipe_opt_in_append(char** cmd1, char** cmd2, char* infile, char* outfile)
{
   log_line("Attempting to execute two pipe commands with >> and possibly <\nAttempting fork()");
   char buff[256];

   //fork
   pid_t pid = fork();

   //error occurred
   if(pid < 0)
   {
      log_line("Fork Failed\n");
      return;
   }
   //child process
   else if(pid == 0)
   {
      log_line("Child is saving stdin and stdout descriptors and creating pipe\n");

      //create pipe:	pipe[0] is read, pipe[1] is write
      int pipefd[2];
      int stdout_fd = dup(STDOUT_FILENO);
      int stdin_fd = dup(STDIN_FILENO);

      //create pipe
      if(pipe(pipefd) == -1)
      {
         log_line("Child could not create pipe\nChild is terminating\n");
         exit(0);
      }
      else
         log_line("Child created pipe\n");

      //fork
      pid_t pid1 = fork();

      //error occurred
      if(pid1 < 0)
      {
         log_line("Child fork failed\nChild terminating\n");
         exit(0);
      }
      //cmd1 process
      else if(pid1 == 0)
      {
         sprintf(buff, "cmd1(PID=%d): Attempting to connect to write end of pipe\n", getpid());
         log_line(buff);

         close(pipefd[0]);   //close unneeded end of pipe

         //connect to pipe
         if(dup2(pipefd[1], STDOUT_FILENO) == -1)
            sprintf(buff, "cmd1: \"%s\" could not connect the write end of the pipe\n", cmd1[0]);
         else
            sprintf(buff, "cmd1: \"%s\" connected to the write end of the pipe\n", cmd1[0]);

         log_line(buff);

         int t1, t2;
         if(infile[0] != '\0')
         {
            sprintf(buff, "cmd1: Applying input redirection from %s\n", infile);
            log_line(buff);

            //apply input redirection
            redirIn(infile, &t1, &t2);
         }

         sprintf(buff, "cmd1: Attempting to execute \"%s\" with execvp()\n", cmd1[0]);
         log_line(buff);

         //execute cmd1
         execvp(cmd1[0], cmd1);

         sprintf(buff, "cmd1: Could not find a command or program \"%s\"\n", cmd1[0]);
         log_line(buff);

         if(infile[0] != '\0')
         {
            log_line("cmd1: Restoring stdin\n");
            //restore input redirection
            resIn(t1, t2);
         }

         log_line("cmd1: Closing write end of pipe and restoring stdout\n");
         //close pipe and restore stdout
         resOut(stdout_fd, pipefd[1]);

         log_line("cmd1: Terminating\n");
         exit(0);   //terminate cmd1
      }
      //cmd2 process
      else
      {
         sprintf(buff, "cmd2(PID=%d): Attempting to connect to read end of pipe\n", getpid());
         log_line(buff);

         close(pipefd[1]);   //close unneeded end of pipe

         //connect to pipe
         if(dup2(pipefd[0], STDIN_FILENO) == -1)
            sprintf(buff, "cmd2: \"%s\" could not connect the read end of the pipe\n", cmd2[0]);
         else
            sprintf(buff, "cmd2: \"%s\" connected to the read end of the pipe\n", cmd2[0]);

         log_line(buff);

         sprintf(buff, "cmd2: Applying output redirection (append) to %s\n", outfile);
         log_line(buff);

         //output redirection (append)
         int t1, t2;
         redirOutAppend(outfile, &t1, &t2);

         sprintf(buff, "cmd2: Attempting to execute \"%s\" with execvp()\n", cmd2[0]);
         log_line(buff);

         //execute cmd2
         execvp(cmd2[0], cmd2);

         sprintf(buff, "cmd2: Could not find a command or program \"%s\"\n", cmd2[0]);
         log_line(buff);

         log_line("cmd2: Restoring stdout\n");
         //restore output redirection
         resOut(t1, t2);

         log_line("cmd2: Closing read end of pipe and restoring stdin\n");
         //close pipe and restore stdin
         resIn(stdin_fd, pipefd[0]);

         log_line("cmd2: Terminating\n");
         exit(0);   //terminate cmd2
      }
   }
   //parent process
   else
   {
      sprintf(buff, "Parent process is waiting. Child's PID=%d\n", pid);
      log_line(buff);

      wait(NULL);

      log_line("Parent is finished waiting\n");
   }
}

/*
 * Executes two commands piped together with output redirection (overwrite)
 *    and possibly input redirection
 */
void exec_pipe_opt_in_write(char** cmd1, char** cmd2, char* infile, char* outfile)
{
   log_line("Attempting to executed two piped commands with > and possibly <\nAttempting fork()\n");
   char buff[256];

   //fork
   pid_t pid = fork();

   //error occurred
   if(pid < 0)
   {
      log_line("Fork failed\n");
      return;
   }
   //child process
   else if(pid == 0)
   {
      log_line("Child is saving stdin and stdout descriptors and creating pipe\n");

      //create pipe:	pipe[0] is read, pipe[1] is write
      int pipefd[2];
      int stdout_fd = dup(STDOUT_FILENO);
      int stdin_fd = dup(STDIN_FILENO);

      //create pipe
      if(pipe(pipefd) == -1)
      {
         log_line("Child could not create pipe\nChild is terminating\n");
         exit(0);
      }
      else
         log_line("Child created pipe\n");

      //fork
      pid_t pid1 = fork();

      //error occurred
      if(pid1 < 0)
      {
         log_line("Child fork failed\nChild terminating\n");
         exit(0);
      }
      //cmd1 process
      else if(pid1 == 0)
      {
         sprintf(buff, "cmd1(PID=%d): Attempting to connect to write end of pipe\n", getpid());
         log_line(buff);

         close(pipefd[0]);   //close unneeded end of pipe

         //connect to pipe
         if(dup2(pipefd[1], STDOUT_FILENO) == -1)
            sprintf(buff, "cmd1: \"%s\" could not connect the write end of the pipe\n", cmd1[0]);
         else
            sprintf(buff, "cmd1: \"%s\" connected to the write end of the pipe\n", cmd1[0]);

         log_line(buff);

         int t1, t2;
         if(infile[0] != '\0')
         {
            sprintf(buff, "cmd1: Applying input redirection from %s\n", infile);
            log_line(buff);

            //input redirection
            redirIn(infile, &t1, &t2);
         }

         sprintf(buff, "cmd1: Attempting to execute \"%s\" with execvp()\n", cmd1[0]);
         log_line(buff);

         //execute cmd1
         execvp(cmd1[0], cmd1);

         sprintf(buff, "cmd2: Could not find a command or program \"%s\"\n", cmd2[0]);
         log_line(buff);

         if(infile[0] != '\0')
         {
            log_line("cmd1: Restoring stdin\n");
            //restore input redirection
            resIn(t1, t2);
         }

         log_line("cmd1: Closing write end of pipe and restoring stdout\n");
         //close pipe and restore stdout
         resOut(stdout_fd, pipefd[1]);

         log_line("cmd1: Terminating\n");
         exit(0);   //terminate cmd1
      }
      //cmd2 process
      else
      {
         sprintf(buff, "cmd2(PID=%d): Attempting to connect to read end of pipe\n", getpid());
         log_line(buff);

         close(pipefd[1]);   //close unneeded end of pipe

         //connect to pipe
         if(dup2(pipefd[0], STDIN_FILENO) == -1)
            sprintf(buff, "cmd2: \"%s\" could not connect the read end of the pipe\n", cmd2[0]);
         else
            sprintf(buff, "cmd2: \"%s\" connected to the read end of the pipe\n", cmd2[0]);

         log_line(buff);

         sprintf(buff, "cmd2: Applying output redirection (overwrite) to %s\n", outfile);
         log_line(buff);

         //output redirection (overwrite)
         int t1, t2;
         redirOut(outfile, &t1, &t2);

         sprintf(buff, "cmd2: Attempting to execute \"%s\" with execvp()\n", cmd2[0]);
         log_line(buff);

         //execute cmd2
         execvp(cmd2[0], cmd2);

         sprintf(buff, "cmd2: Could not find a command or program \"%s\"\n", cmd2[0]);
         log_line(buff);

         log_line("cmd2: Restoring stdout\n");
         //restore output redirection
         resOut(t1, t2);

         log_line("cmd2: Closing read end of pipe and restoring stdin\n");
         //close pipe and restore stdin
         resIn(stdin_fd, pipefd[0]);

         log_line("cmd2: Terminating\n");
         exit(0);   //terminate cmd2
      }
   }
   //parent process
   else
   {
      sprintf(buff, "Parent process is waiting. Child's PID=%d\n", pid);
      log_line(buff);

      wait(NULL);

      log_line("Parent is finished waiting\n");
   }
}
