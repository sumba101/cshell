#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <wait.h>
#include <fcntl.h>

/*common macros and data*/
// Defining colors for usage later
#define KRED  "\x1B[31m"
#define KBLU  "\x1B[34m"
//defining a default buffer for inputs
#define BUFSIZE 10000
//defining a maximum number of arguments
#define ARGMAX 10
#define DEF "\x1B[0m" //colour reset

char history[20][BUFSIZE];
int hist_counter=0;


////modules for the commands
#include "prototypes.h"

#include "cd.h"
#include "clear.h"
#include "echo.h"
#include "exiter.h"
#include "helper_func.h"
#include "ls.h"
#include "pinfo.h"
#include "pwd.h"
#include "history.h"

//a greeting string which is constant
const char greeting[BUFSIZE]= "\t\t\t\t          __\n"
                             "\t\t  ___  \t             ____/ /_    _____  __      __   \n"
                             "\t\t / __\\    ____      / __/  _ \\  /      / /     / /          \n"
                             "\t\t/ /___   |____|\t   (_  )  / // /===== / /___  / /___ \n"
                             "\t\t\\____/\t          /___//_/ // /_____ /_____/ /_____/ \t\t\t\t\t  \t\t                                                         \n....by Sumanth Balaji!\n\n";


//an exiting line
const char ending[BUFSIZE]="Exiting the C shell, Thank You.";

typedef struct values{
    bool full;
    pid_t pid;
    char command[100];
}Values;
Values v[100];


const char * builtin_C[]={"exit","cd","clear","pwd","echo","ls","history","pinfo"};

const bool (*builtin_F[])(const size_t ,char **)={
        (const bool (*)(const size_t ,char **)) &exiter,
        (const bool (*)(const size_t ,char **)) &cd,
        (const bool (*)(const size_t, char **)) &clear,
        (const bool (*)(const size_t, char **))&pwd,
        (const bool (*)(const size_t, char **)) &echo,
        (const bool (*)(const size_t, char **)) &ls,
        (const bool (*)(const size_t, char **)) &historyp,
        (const bool (*)(const size_t, char **)) &pinfo};
//setup for function call back mechanism


int main(){//semincolon separated commands in the command line due to assignment requirement
    printf("%s%s",KBLU,greeting);

    char cwdstr[BUFSIZE],usernhoststring[2*BUFSIZE];
    bool exit=false;

    while(!exit){
        prompt(cwdstr,usernhoststring);
        char **commands=malloc(sizeof(char)*BUFSIZE);
        input1(commands); //taking all the commands given
        for (int i = 0; commands[i] && !exit; ++i) {
            size_t argc=0;
            char **argv = malloc(sizeof(char)*BUFSIZE);
            bool background=false;
            argc=input2(argc,argv,&background,commands[i]);
            exit=execution(argc,argv,background);

            //exit takes value true in case of errors and whatnot
        }
        free(commands[0]);
        free(commands); //frees up command
    }

    printf("%s%s",KBLU,ending);
    return 0;
}

void input1(char **pString) {
    fflush(stdout); // clear out the stdout buffer just in case

    char *command= malloc(sizeof(char)*BUFSIZE);
    size_t buf = 0;
    getline(&command, &buf, stdin);

    int i=0;
    char * arg=strtok(command,";");

    while(arg != NULL)
    {
        pString[i]=arg;
        ++i;
        arg=strtok(NULL,";"); //because in repeat strtoks, null must be passed
    }
    pString[i]=NULL;
}

size_t input2(size_t argc, char **argv, bool *background, char *command) {

    if (strcmp(command,"\n"))
        add_history(command);

    argc = 0;
    *background = false;
    char * arg=strtok(command," \t\n");

    while(arg != NULL && argc < ARGMAX-1)
    {
        argv[argc]=arg;
        if(strcmp(argv[argc],"&")==0)
        {
            --argc; //so that the & symbol is overwritten with next string
            *background = true; //run in inBackground
        }

        ++argc;
        arg=strtok(NULL," \t\n"); //because in repeat strtoks, null must be passed
    }
    argv[argc]=NULL;

    return argc;
    //the data pointed to by command is pointed to by argv now, hence it must be deleted later
}

void child_end(int sig) {
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG); //wait till child process is over
    int i=0;
    while (v[i].pid!=pid&&i<100){
        ++i;
    }//finds the position that has the name of the command
    char buffer[BUFSIZE];
    sprintf(buffer,"\n%s with pid %d exited normally\n",v[i].command,pid);
    write(1, buffer, strlen(buffer));

}

bool execution(size_t argc, char **argv, bool background) {

    if (argv[0]==NULL){
        return false;
    }//base case

    bool redirect_in,redirect_out;

    //loop through to check for redirects and piping, i.e > < >> and |
    //This is to be handled in later stage
    for (int i = 0; argv[i] ; ++i) {
        if (!strcmp(argv[i],">")){

        } //redirect out
        else if (!strcmp(argv[i],">>")){

        } //redirect out
        else if (!strcmp(argv[i],"|")){

        } //redirect in
        else if (!strcmp(argv[i],"<")){

        } //redirect in
    }

    for (int j = 0;builtin_C[j]; ++j) {

        if (!strcmp(argv[0],builtin_C[j])){

            return (builtin_F[j])(argc,argv);

        }
    }
    //if it reaches here, the command is not a builtin command
    pid_t pid;
    if ((pid=fork())<0){ perror("Error in forking\n");}
    else if(pid==0){///in child
        if (execvp(argv[0],argv)<0){
            printf("Error: execution of command failed\n");
            exit(1);
        }
    } else{
        if (!background){ ///if it is not a background process, its run and finished
            waitpid(pid,NULL,0);
        } else{
            for (int i = 0; i < 100 ; ++i) {
                if(!v[i].full){
                    v[i].full=true;
                    v[i].pid=pid;
                    strcpy(v[i].command,argv[0]);
                    i=100; ///to break out of loop
                }
            }//above loop stores the command name and pid
          signal(SIGCHLD,child_end);
        }
    }
    return false; //default return
}

