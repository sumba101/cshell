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
#include <readline/readline.h>
#include <readline/history.h>

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
char new_home[1000];

////modules for the commands
#include "prototypes.h"

#include "jobs.h"
#include "cd.h"
#include "clear.h"
#include "echo.h"
#include "exiter.h"
#include "helper_func.h"
#include "ls.h"
#include "pinfo.h"
#include "pwd.h"
#include "history.h"

pid_t Shellpid, childPID=-1;

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
}Values; //structure for storing info of all background processes
Values v[100];



bool unsetenvF(const size_t argc,char **argv);
bool setenvF(const size_t argc,char **argv);
bool overkill(const size_t argc,char **argv);
bool kjob(const size_t argc,char **argv);

const char * builtin_C[]={"exit","cd","clear","pwd","echo","ls","history","pinfo","quit","setenv","unsetenv","overkill","kjob"};

const bool (*builtin_F[])(const size_t ,char **)={
        (const bool (*)(const size_t ,char **)) &exiter,
        (const bool (*)(const size_t ,char **)) &cd,
        (const bool (*)(const size_t, char **)) &clear,
        (const bool (*)(const size_t, char **)) &pwd,
        (const bool (*)(const size_t, char **)) &echo,
        (const bool (*)(const size_t, char **)) &ls,
        (const bool (*)(const size_t, char **)) &historyp,
        (const bool (*)(const size_t, char **)) &pinfo,
        (const bool (*)(const size_t, char **)) &exiter,
        (const bool (*)(const size_t, char **)) &setenvF,
        (const bool (*)(const size_t, char **)) &unsetenvF,
        (const bool (*)(const size_t, char **)) &overkill,
        (const bool (*)(const size_t, char **)) &kjob};
//setup for function call back mechanism


bool HandlePipe(char **pString, char **pString1);


void catchCTRL_Z(int sig);

void catchCTRL_C(int sig);

int main(){    //semincolon separated commands in the command line due to assignment requirement
    printf("%s%s",KBLU,greeting);
    getcwd(new_home, sizeof(new_home));

    char cwdstr[BUFSIZE],usernhoststring[2*BUFSIZE];
    bool exit=false;

    while(!exit){
        signal(SIGTSTP,catchCTRL_Z);
        signal(SIGINT,catchCTRL_C);
        childPID=-1;

        //prompt();
        char **commands=malloc(sizeof(char)*BUFSIZE);
        input1(commands,cwdstr,usernhoststring); //taking all the commands given
        for (int i = 0; commands[i] && !exit; ++i) {
            size_t argp=0;
            char **argv_p = malloc(sizeof(char)*BUFSIZE);
            argp=input2(argp,argv_p,commands[i]);
            exit=execution(argp,argv_p);

            //exit takes value true in case of errors and whatnot
        }
        free(commands[0]);
        free(commands); //frees up command
    }

    printf("%s%s",KBLU,ending);
    return 0;
}


size_t input2(size_t argc, char **argv, char *command) {

    if (strcmp(command,"\n") != 0)
        add_Chistory(command);

    argc = 0;
    char * arg=strtok(command,"|");

    while(arg != NULL && argc < ARGMAX-1)
    {
        argv[argc]=arg;
        ++argc;
        arg=strtok(NULL,"|"); //because in repeat strtoks, null must be passed
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
    bool found_pid=false;

    while (i<100){
       if (v[i].full &&v[i].pid==pid)///if pid exists and if full
       {
           found_pid=true;
           break;
       }
       ++i;
    }//finds the position that has the name of the command
    if (found_pid){
        char buffer[BUFSIZE];
        sprintf(buffer,"\n%s%s with pid %d exited normally\n",KRED,v[i].command,pid);
        v[i].full=false;
        write(1, buffer, strlen(buffer));
    }
}

bool unsetenvF(const size_t argc, char **argv) {
    if (argc == 2){
        if(unsetenv(argv[1])==-1){
            perror("Unsetenv has failed");
        }
    }else{
        printf("%s Error, number of arguments is incorrect",KRED);
    }
    return false;
}

bool setenvF(const size_t argc, char **argv) {
    if(argc == 3){ //given var and value
        if(setenv(argv[1],argv[2],1)==-1){
            perror("Setenv has failed");
        }
    }
    else if(argc == 2){ //given only var
        if(setenv(argv[1],"",1) == -1){
            perror("Setenv has failed");
        }
    }
    else{ //error in input
        printf("%s Error, number of arguments is incorrect",KRED);
    }
    return false;
}

void catchCTRL_Z(int sig) {
    if(getpid() != Shellpid)
        return;
    //CTRL Z caught
    printf("\n");
    if(childPID != -1)
    {
        kill(childPID, SIGTTIN);
        kill(childPID, SIGTSTP);
     // addtoLL(head, nowProcess , childPID, 0);
      //fprintf(stderr,KMAG "[+] %d %s\n" RESET, childPID, nowProcess);
    }
    signal(SIGTSTP, catchCTRL_Z);
}

void catchCTRL_C(int sig) {
    printf("\n");
    if(getpid() != Shellpid)
        return;

    if(childPID != -1)
    {
        kill(childPID, SIGINT);
    }
    signal(SIGINT, catchCTRL_C);
}

bool overkill(const size_t argc, char **argv) {
    //have to end all processes
    Jobs temp;
    while (!empty()){
        temp=top();
        pop();
        kill(temp.pid,17);
        kill(temp.pid,SIGINT);
        printf("%s Killed %d - %s\n",KRED,temp.pid,temp.name);
    }
    return false;
}

bool kjob(const size_t argc, char **argv) {
    if(argc != 3){
        printf("%s Error, wrong number of parameters");
    } else{
        int Jnum=atoi(argv[1]);
        int sig=atoi(argv[2]);
        bool j=checkJob(Jnum);
        if(j){
            Jobs j=getJob(Jnum);
            kill(j.pid,sig);
        }
        else{
            printf("%sError, no such pid exists");
        }
    }
    return false;
}

void input1(char **pString, char *cwdstr, char *usernhoststring) {
    fflush(stdout); // clear out the stdout buffer just in case

    char *command;
    cwd(cwdstr,0);
    UsernHost_names(usernhoststring);
    char prompt[3*BUFSIZE];
    sprintf(prompt,"%s<%s:%s>",KRED,usernhoststring,cwdstr);

    if((command=readline(prompt))!=NULL){
        if(strlen(command)>0){
            add_history(command);
        }
    }

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

bool execute_command(char **argv, bool background, int redirect_in,int redirect_out,int infile,int outfile,int in,int out){
    size_t argc = 0;
    while(argv[argc]){
     ++argc;
    }

    if (in != STDIN_FILENO)
    {
        dup2 (in, STDIN_FILENO);
        close (in);
    }

    if (out != STDOUT_FILENO)
    {
        dup2 (out, STDOUT_FILENO);
        close (out);
    }

    for (int j = 0;builtin_C[j]; ++j) {

        if (!strcmp(argv[0],builtin_C[j])){

            if (redirect_in){
                dup2(infile,STDIN_FILENO);
            }
            if (redirect_out){
                dup2(outfile,STDOUT_FILENO);
            }
            bool ret= (builtin_F[j])(argc,argv);
            return ret;
        }
    }
    //if it reaches here, the command is not a builtin command
    pid_t pid;
    if ((pid=fork())<0){ perror("Error in forking\n");}
    else if(pid==0){///in child

        if (redirect_in){
            dup2(infile,STDIN_FILENO);
        }
        if (redirect_out){
            dup2(outfile,STDOUT_FILENO);
        }

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


void redirection_handler(char **argv ,int *outfile,bool *redirect_out, bool *redirect_in,int *infile){
    //loop through to check for redirects and piping, i.e > < >> and |
    for (int i = 0; argv[i]; ++i) {
        if (!strcmp(argv[i], ">")) {
            (*outfile) = open(argv[i + 1],  O_WRONLY | O_TRUNC | O_CREAT, 0644);
            argv[i] = NULL;
            (*redirect_out) = true;

        } //redirect out
    }
    for (int i = 0; argv[i]; ++i) {
        if (!strcmp(argv[i], ">>")) {
            (*outfile) = open(argv[i + 1],  O_WRONLY | O_APPEND | O_CREAT, 0644);
            argv[i] = NULL;
            (*redirect_out) = true;

        } //redirect out
    }
    for (int i = 0; argv[i]; ++i) {
        if (!strcmp(argv[i], "<")) {
            (*infile) = open(argv[i + 1], O_RDONLY);
            argv[i] = NULL;
            (*redirect_in) = true;
        } //redirect in
    }

}


char **split_command(char *command, char *DELIM,bool * background)
{
    size_t i=0;
    char **pString = malloc(BUFSIZE * sizeof(char*));
    char * arg=strtok(command,DELIM);

    while(arg != NULL)
    {
        pString[i]=arg;
        if(strcmp(pString[i],"&")==0)
        {
            --i; //so that the & symbol is overwritten with next string
            *background = true; //run in inBackground
        }

        ++i;
        arg=strtok(NULL,DELIM); //because in repeat strtoks, null must be passed
    }
    pString[i]=NULL;
    return pString;
}

bool execution(size_t argp, char **argv_p) {

    if (argv_p[0] == NULL) {
        return false;
    }//base case

    bool redirect_in = false, redirect_out = false;
    int infile = 0, outfile = 0, stdinCpy = 0, stdoutCpy = 0;

    bool background=false;
    char **argv;

    int in=0;
    int fd[2];

    size_t i=0;

    bool quit;
    for (; i < argp-1; ++i) {
        dup2(STDIN_FILENO,stdinCpy);
        dup2(STDOUT_FILENO,stdoutCpy);

        argv=split_command(argv_p[i], " \t\r\n\a",&background);
        pipe(fd);
        redirection_handler(argv, &outfile, &redirect_out,&redirect_in,&infile);

        quit= execute_command(argv,background,redirect_in,redirect_out,infile,outfile,in,fd[1]);
        close(fd[1]);

        dup2(stdinCpy,STDIN_FILENO);
        dup2(stdoutCpy,STDOUT_FILENO);
        in=fd[0];
        if (quit){
            return true;
        }
    }

    dup2(stdinCpy,STDIN_FILENO);
    dup2(stdoutCpy,STDOUT_FILENO);

    argv=split_command(argv_p[i], " \t\r\n\a",&background);
    redirection_handler(argv, &outfile, &redirect_out,&redirect_in,&infile);

    quit= execute_command(argv,background,redirect_in,redirect_out,infile,outfile,in,STDOUT_FILENO);

    dup2(stdinCpy,STDIN_FILENO);
    dup2(stdoutCpy,STDOUT_FILENO);
    return quit;
}

