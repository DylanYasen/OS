#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAXLINE 80 /* The maximum length command */
#define MAXCMDLENGTH 256

int main(void)
{
  char *commands[MAXLINE+1]; /* command line arguments */
  for (int i = 0; i < sizeof(commands) / sizeof(char*); i++)
    commands[i] = NULL;

  int isRunning = 1; /* flag to determine when to exit program */
  int line = 0;
  while (isRunning) {

    // prompt
    printf("yadikae1>"); 
    fflush(stdout);

    // get user input
    char* input = malloc (MAXCMDLENGTH);
    fgets (input, MAXCMDLENGTH, stdin);

    // remove new line char
    if ((strlen(input)>0) && (input[strlen (input) - 1] == '\n'))
      input[strlen (input) - 1] = '\0';

  // ============== SPECIAL COMMANDS ============== 
  // terminate
  if (strcmp(input, "exit") == 0)
  {
    isRunning = 0;
    continue;
  } 
  // history
  else if(strcmp(input, "history") == 0) {
    for (int i = sizeof(commands) / sizeof(char*) - 1; i >= 0; i--)
    {
      if(commands[i] == NULL)
        continue;

      printf("#=>%d %s\n",i+1,commands[i]);
    }
  }

  else{
     // tokenize
    char raw [MAXCMDLENGTH];
    strcpy(raw, input);
    
    char* args[MAXCMDLENGTH];

    char* token = strtok(raw, " ");
    int count = 0;
    while (token) {
        args[count] = token;
        token = strtok(NULL, " ");
        count++; 
      }

      // get last param to check if parent should wait
      int waitForChild = 1;
      for (int i = 0; i < sizeof(args) / sizeof(char*); ++i)
      {
        if (args[i] == NULL)
        {
          // last param is &
          // run concurrently
          if(args[i-1][0] == '&'){
            waitForChild = 0;  
            args[i-1] = NULL;
          }
          break;
        }
      }

      // create child process
      pid_t childpid = fork();
      if(childpid < 0){
        perror("fork error\n");
        exit(-1);
      }

      // parent process
      if(childpid != 0){

        // dont wait
        if (waitForChild)
        {
          printf("%s\n", "#=>Waiting for child process to finish");
          wait(NULL);
          printf("%s\n", "#=>Child process finished");
        }
        else{
            printf("%s\n", "#=>Child process is running concurrently in the background");      
        }
      }

      // child process
      else{
        execvp(args[0],args);
      }

      commands[line] = input;
      line++;
    }
  }
  printf("%s\n", "#=>Exiting Shell");
  return 0;
}
