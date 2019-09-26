/*  Author:- sumanth balaji
 *  IIIT Hyderabad
 *
 * */

#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
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

char nowProcess[20];
pid_t Shellpid, childPID=-1;

//a greeting string which is constant
const char greeting[BUFSIZE]= "\t\t\t\t          __\n"
                             "\t\t  ___  \t             ____/ /_    _____  __      __   \n"
                             "\t\t / __\\    ____      / __/  _ \\  /      / /     / /          \n"
                             "\t\t/ /___   |____|\t   (_  )  / // /===== / /___  / /___ \n"
                             "\t\t\\____/\t          /___//_/ // /_____ /_____/ /_____/ \t\t\t\t\t  \t\t                                                         \n....by Sumanth Balaji!\n\n";


//an exiting line
const char ending[BUFSIZE]="Exiting the C shell, Thank You.";


bool unsetenvF(size_t argc,char **argv);
bool setenvF(size_t argc,char **argv);
bool overkill(size_t argc,char **argv);
bool kjob(size_t argc,char **argv);
bool fg(size_t argc,char **argv);
bool bg(size_t argc,char **argv);
bool job_display(size_t argc,char **argv);

const char * builtin_C[]={"exit","cd","clear","pwd","echo","ls","history","pinfo","quit","setenv","unsetenv","overkill","kjob","fg","bg","jobs"};

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
        (const bool (*)(const size_t, char **)) &kjob,
        (const bool (*)(const size_t, char **)) &fg,
        (const bool (*)(const size_t, char **)) &bg,
        (const bool (*)(const size_t, char **)) &job_display};
//setup for function call back mechanism


void catchCTRL_Z(int sig);

void catchCTRL_C(int sig);

int main(){    //semincolon separated commands in the command line due to assignment requirement
    printf("%s%s",KBLU,greeting);
    getcwd(new_home, sizeof(new_home));

    char cwdstr[BUFSIZE],usernhoststring[2*BUFSIZE];
    bool exit=false;

    while(!exit){
        childPID=-1;

        signal(SIGTSTP,catchCTRL_Z);
        signal(SIGINT,catchCTRL_C);

        char **commands=malloc(sizeof(char)*BUFSIZE);
        input1(commands,cwdstr,usernhoststring); //taking all the commands given
        for (int i = 0; commands[i] && !exit; ++i) {
            size_t argp=0;
            char **argv_p = malloc(sizeof(char)*BUFSIZE);
            argp=input2(argp,argv_p,commands[i]);

            exit=execution(argp,argv_p);
            update_jobs();
            //exit takes value true in case of errors and whatnot
        }
        free(commands[0]);
        free(commands); //frees up command
    }

    printf("%s%s",KBLU,ending);
    erase_jobs(); ///clears up the list of all jobs
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
    Jobs* p=findJob(pid);

    if (p){
        char buffer[BUFSIZE];
        sprintf(buffer,"\n%s%s with pid %d exited normally\n",KRED,p->name,pid);
        write(1, buffer, strlen(buffer));
        remove_j(pid);
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
        char temp[100];
        strcpy(temp,argv[2]);

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
//    if(getpid() != Shellpid)
//        return;
    //CTRL Z caught3
    printf("\n");

    char buffer[BUFSIZE];

    if(childPID != -1)
    {
        //kill(childPID, SIGTTIN);

        sprintf(buffer,"\nHERE1 %d\n",childPID);
        write(1, buffer, strlen(buffer));

        kill(childPID, SIGTSTP);


        sprintf(buffer,"\nHERE2 %d\n",childPID);
        write(1, buffer, strlen(buffer));

        push(nowProcess, childPID, false);
        childPID=-1;
        strcpy(nowProcess,"");


        sprintf(buffer,"\nHERE3 %d\n",childPID);
        write(1, buffer, strlen(buffer));

        signal(SIGTSTP, SIG_IGN);

    }

    sprintf(buffer,"\nHERE4 %d\n",childPID);
    write(1, buffer, strlen(buffer));

}

void catchCTRL_C(int sig) {
    printf("\n");

    if(childPID != -1)
    {
        kill(childPID, SIGINT);
        strcpy(nowProcess,"");
        childPID=-1;
        signal(SIGINT, SIG_IGN);

    }
}

bool overkill(const size_t argc, char **argv) {
    ///have to end all processes
    overkill_jobs();
    return false;
}

bool kjob(const size_t argc, char **argv) {
    if(argc != 3){
        printf("%s Error, wrong number of parameters");
    } else{
        int Jnum=atoi(argv[1]); ///just converting the characters into integers
        int sig=atoi(argv[2]);
        Jobs* j=getJob(Jnum);
        if(j){

            kill(j->pid,sig);
            if (sig==9){ ///if process is supposed to be killed
                remove_j(j->pid);
            }
        }
        else{
            printf("%sError, no such job number exists");
        }
    }
    return false;
}

bool fg(const size_t argc, char **argv) {
    if(argc!=2){
        printf("error");
        return false;
    }
    else{
        int jno=atoi(argv[1]);
        Jobs *p=getJob(jno);
        if (p){
            int pid = p -> pid;
            kill(pid, SIGCONT);
            childPID = pid;
            strcpy(nowProcess, p->name); ///for use in ctrl-Z
            remove_j(pid);
            waitpid(-1,NULL,WUNTRACED);
            childPID=-1;
            strcpy(nowProcess,"");
        }
    }
    return false;
}

bool bg(size_t argc, char **argv) {
    if(argc!=2){
        printf("Error, incorrect number of arguments");
    } else{
        int jno = atoi(argv[1]);
        Jobs* job_node = getJob(jno);
        if(job_node)
        {
            int pid = job_node -> pid;
            kill(pid, SIGTTIN);
            kill(pid, SIGCONT);
            change(pid,1);
        }
    }
    return 0;
}

bool job_display(size_t argc, char **argv) {
    print_jobs();
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

bool execute_command(char **argv, bool background, int in,int out){
    size_t argc = 0;
    while(argv[argc]){
     ++argc;
    } ///find argc for future purposes


    if(in!=STDIN_FILENO){ //means the input is coming from pipe, which is pointed to by in
        dup2(in,STDIN_FILENO); //this puts pipe values for stdin
        close(in); //closing the writing end of the pipe
    }

    if (out != STDOUT_FILENO){
        dup2(out,STDOUT_FILENO); //put the values of stdout into the write end of pipe
        close(out);
    }

    for (int j = 0;builtin_C[j]; ++j) {

        if (!strcmp(argv[0],builtin_C[j])){

            bool ret= (builtin_F[j])(argc,argv);

            return ret; //exits the module if appropriate builtin command is executed
        }
    }
    //if it reaches here, the command is not a builtin command
    pid_t pid;
    pid=fork();

    if (pid<0){ perror("Error in forking\n");}
    else if(pid==0){///in child

        if(background)
            setpgid(0,0);

        if (execvp(argv[0],argv)<0){
            printf("Error: execution of command failed\n");
            exit(1);
        }
    } else{
        if (!background){ ///if it is not a background process, its run and finished

            char proc_name[100];
            strcpy(proc_name,argv[0]);
            for (int i = 1; argv[i]; ++i) {
                strcat(proc_name," ");
                strcat(proc_name,argv[i]);
            }
            strcpy(nowProcess,proc_name);
            childPID=pid;
            waitpid(pid,NULL,WUNTRACED);

            strcpy(nowProcess,"");
            childPID=-1;

        } else{ ///if background process, store in jobs queue
                tcsetpgrp(0, getpgrp());

                char proc_name[100];
                strcpy(proc_name,argv[0]);
                for (int i = 1; argv[i]; ++i) {
                    strcat(proc_name," ");
                    strcat(proc_name,argv[i]);
                }
                push(proc_name,pid,true);
                signal(SIGCHLD,child_end);
        }
    }
    return false; //default return
}


void redirection_handler(char **argv ){
    //loop through to check for redirects and piping, i.e > < >> and |

    int outfile=0;bool redirect_out = 0; bool redirect_in = 0;int infile=0;

    for (int i = 0; argv[i]; ++i) {
        if (!strcmp(argv[i], ">")) {
            outfile = open(argv[i + 1],  O_WRONLY | O_TRUNC | O_CREAT, 0644);
            argv[i] = NULL;
            redirect_out = true;

        } //redirect out
    }
    for (int i = 0; argv[i]; ++i) {
        if (!strcmp(argv[i], ">>")) {
            outfile = open(argv[i + 1],  O_WRONLY | O_APPEND | O_CREAT, 0644);
            argv[i] = NULL;
            redirect_out = true;

        } //redirect out
    }
    for (int i = 0; argv[i]; ++i) {
        if (!strcmp(argv[i], "<")) {
            infile = open(argv[i + 1], O_RDONLY);
            argv[i] = NULL;
            redirect_in = true;
        } //redirect in
    }
    if (redirect_in){
        dup2(infile,STDIN_FILENO);
        close(infile);
    }
    if (redirect_out){
        dup2(outfile,STDOUT_FILENO);
        close(outfile);
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
    }//base case when its empty

    bool background=false;
    char **argv;

    int in=0;
    int fd[2];

    size_t i=0;
    bool quit;

    for (; i < argp-1; ++i) {
        background = false;

        int stdinCpy=dup(STDIN_FILENO);
        int stdoutCpy=dup(STDOUT_FILENO);

        argv=split_command(argv_p[i], " \t\r\n\a",&background);

        pipe(fd);

        redirection_handler(argv);

        quit= execute_command(argv,background,in,fd[1]);
        close(fd[1]);

        dup2(stdoutCpy,STDOUT_FILENO);
        dup2(stdinCpy,STDIN_FILENO);

        in=fd[0];
        if (quit){ /// quit command is present in the command
            return true;
        }
    }


    int stdinCpy=dup(STDIN_FILENO);
    int stdoutCpy=dup(STDOUT_FILENO);

    argv=split_command(argv_p[i], " \t\r\n\a",&background);
    redirection_handler(argv);

    quit= execute_command(argv,background,in,STDOUT_FILENO);

    dup2(stdoutCpy,STDOUT_FILENO);
    dup2(stdinCpy,STDIN_FILENO);

    return quit;
}

