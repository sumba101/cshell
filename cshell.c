#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>
/* List of all things to implement
 * Display requirement
 * cd
 * pwd
 * echo
 * background processes
 * vi
 * emacs and more
 * pinfo
 * clear
 * */

// Defining colors for usage later
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define BUFSIZE 10000

#define ARGMAX 10
const char greeting[BUFSIZE]= "\t\t\t\t          __\n"
                             "\t\t  ___  \t             ____/ /_    _____  __      __   \n"
                             "\t\t / __\\    ____      / __/  _ \\  /      / /     / /          \n"
                             "\t\t/ /___   |____|\t   (_  )  / // /===== / /___  / /___ \n"
                             "\t\t\\____/\t          /___//_/ // /_____ /_____/ /_____/ \t\t\t\t\t  \t\t                                                         \n....by Sumanth Balaji!\n\n";



void cwd(char* cwdstr, int command);

void UsernHost_names(char usernhoststring[]);

void adjust_start(char temp[10000]);

void prompt(char cwdstr[],char usernhoststring[]);

size_t input(size_t argc, char **argv, bool *background);

int main()
{
    printf("%s%s",KBLU,greeting);

    char cwdstr[BUFSIZE],usernhoststring[2*BUFSIZE];

    bool exit=false;

    while(!exit){

        prompt(cwdstr,usernhoststring);

        char command[BUFSIZE];
        size_t argc=0;
        char ** argv;
        bool background=false;

        argc=input(argc,argv,&background);

        if(!strcmp(command,"clear")){
            printf ("\033c");
        }
        else if(!strcmp(command,"pwd")){
            cwd(cwdstr,1); //parameter 1 makes it print out cwd
        }
        else if(command[0]=='c'&&command[1]=='d'){ //if the command is cd
            /*
             * must handle
             * cd . or ./
             * cd ../ or .. or ../../ and more
             * cd ~
             * cd ~/blah/blah
             * cd ./blah/blah
             * */
            cd();
        }
        else if(!strcmp(command,"echo")){

        }
        else if(!strcmp(command,"cd")){

        }
        else if(!strcmp(command,"cd")){

        }
        else if(!strcmp(command,"cd")){

        }
    }
    return 0;
}

size_t input(size_t argc, char **argv, bool *background) {
    fflush(stdout); // clear out the stdout buffer just in case

    char *command= NULL;
    size_t buf = 0;
    getline(&command, &buf, stdin);
    //getline automatically allocates a buffer of appropriate size

    //input1 = (char *)malloc(strlen(command) * sizeof(char));
    //strncpy(input1,input,strlen(command));
    argc = 0;
    *background = false;
    while((argv[argc] = strtok(command, " \t\n")) != NULL && argc < ARGMAX-1)
    {
        if(strcmp(argv[argc],"&")==0)
        {
            *background = true; //run in inBackground
        }
        ++argc;
        strcpy(command,NULL); //because in repeat strtoks, null must be passed
    }
    free(command);
    return argc;
}

void prompt(char cwdstr[],char usernhoststring[]) {
    cwd(cwdstr,0);
    UsernHost_names(usernhoststring);

    printf("<%s%s:%s>",KRED,usernhoststring,cwdstr);

}


void UsernHost_names(char usernhoststring[]){
    char *lgn,host[BUFSIZE];
    lgn=(char *)malloc(100* sizeof(char));
    if ((getlogin_r(lgn,100)) || (gethostname(host, sizeof(host))) == -1 ){
        perror("Error in getting user and host name");
    } else{
        strcpy(usernhoststring,lgn);
        strcat(usernhoststring, "@");
        strcat(usernhoststring,host);
    }
}


void cwd(char* cwdstr,int command)
{
    char temp[BUFSIZE];

    if(getcwd(temp, sizeof(temp)) != NULL)
    {
        adjust_start(temp);
        strcpy(cwdstr,temp);
        if(command==1)  // check if pwd is to be printed
        {
            printf("\n%s%s\n",KRED,cwdstr);
        }
    }
    else perror("+--- Error in getcwd() : ");

}

void adjust_start(char temp[]) {
    char new_home[10000];
    strcpy(new_home,"/home/sumanth/OS/2018114002_CShell");
    int i = 0;
    for (; new_home[i] && new_home[i]==temp[i] ; ++i) {}
    if (i>strlen(temp)){
        perror("Error, current location is outside of shell");
    } else{
        char replace[10000];
        strcpy(replace,"~");
        int z=1;
        while (temp[i]){
            replace[z]=temp[i];
            ++z;
            ++i;
        }
        replace[z]='\0';
        strcpy(temp,replace);
    }
}
