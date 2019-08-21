#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>

// Defining colors for usage later
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
//defining a default buffer for inputs
#define BUFSIZE 10000
//defining a maximum number of arguments
#define ARGMAX 10

//a greeting string which is constant
const char greeting[BUFSIZE]= "\t\t\t\t          __\n"
                             "\t\t  ___  \t             ____/ /_    _____  __      __   \n"
                             "\t\t / __\\    ____      / __/  _ \\  /      / /     / /          \n"
                             "\t\t/ /___   |____|\t   (_  )  / // /===== / /___  / /___ \n"
                             "\t\t\\____/\t          /___//_/ // /_____ /_____/ /_____/ \t\t\t\t\t  \t\t                                                         \n....by Sumanth Balaji!\n\n";


//an exiting line
const char ending[BUFSIZE]="Exiting the C shell, Thank You.";

bool exiter(size_t argc, char **argv);

bool cd(size_t argc, char **argv);

bool clear(size_t argc, char **argv);

bool pwd(size_t argc, char **argv);
bool echo(size_t argc, char **argv);
bool ls(size_t argc,char **argv);

const char * builtin_C[]={"exit","cd","clear","pwd","echo","ls"};

const bool (*builtin_F[])(size_t ,char **)={&exiter,&cd,&clear,&pwd,&echo,&ls};//ls is left
//setup for function call back mechanism

void cwd(char* cwdstr, int command);
//helper function
void UsernHost_names(char usernhoststring[]);
//helper function
void adjust_start(char temp[10000]);
//helper function

void bubbleout(char *string);
//helper function

void prompt(char cwdstr[],char usernhoststring[]);

size_t input(size_t argc, char **argv, bool *background);
//input accepting and processing

bool execution(size_t argc, char **argv, bool background);
//execution module that directs the action

void freeing(char **pString,int count);

int main()
{
    printf("%s%s",KBLU,greeting);

    char cwdstr[BUFSIZE],usernhoststring[2*BUFSIZE];
    bool exit=false;

    while(!exit){
        prompt(cwdstr,usernhoststring);
        size_t argc=0;
        char **argv = malloc(sizeof(char)*ARGMAX);
        bool background=false;

        argc=input(argc,argv,&background);

        char *temp=malloc(BUFSIZE);

        exit=execution(argc,argv,background);

        //exit takes value true in case of errors and whatnot
        free(argv[0]);
        free(argv); //frees up argv
    }

    printf("%s%s",KBLU,ending);
    return 0;
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

    return false; //default return

}

size_t input(size_t argc, char **argv, bool *background) {
    fflush(stdout); // clear out the stdout buffer just in case

    char *command= malloc(sizeof(char)*BUFSIZE);
    size_t buf = 0;
    getline(&command, &buf, stdin);
//    getline automatically allocates a buffer of appropriate size

    argc = 0;
    *background = false;
    char * arg=strtok(command," \t\n");
    while(arg != NULL && argc < ARGMAX-1)
    {
        argv[argc]=arg;
        if(strcmp(argv[argc],"&")==0)
        {
            *background = true; //run in inBackground
        }

        ++argc;
        arg=strtok(NULL," \t\n"); //because in repeat strtoks, null must be passed
    }
    argv[argc]=NULL;

    return argc;
    //the data pointed to by command is pointed to by argv now, hence it must be deleted later
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

void prompt(char cwdstr[],char usernhoststring[]) {
    cwd(cwdstr,0);
    UsernHost_names(usernhoststring);

    printf("%s<%s:%s>",KRED,usernhoststring,cwdstr);

}

void adjust_start(char temp[]) {
    char new_home[10000];
    strcpy(new_home,"/home/sumanth/OS/2018114002_CShell");
    int i = 0;
    for (; new_home[i] && new_home[i]==temp[i] ; ++i) {}
    if (i<=strlen(temp))
    {
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
    }    //else the path is outside of shell, hence no edits

}

void bubbleout(char string[]) {
    int j = 0;
    for (; string[j+1] ; ++j) {
        string[j]^=string[j+1];
        string[j+1]^=string[j];
        string[j]^=string[j+1];
        printf("%s\n",string);
    }
    string[j]^=string[j+1];
    string[j+1]^=string[j];
    string[j]^=string[j+1];


    //this code bubbles out the first character
}

bool cd(size_t argc, char **argv) {
    //returns true in case of failure after warning
    if(argc!=2){
        printf("%sError, command is incorrect",KRED);
    }
    if(argv[1][0]=='~'){
        char new_home[10000];
        strcpy(new_home,"/home/sumanth/OS/2018114002_CShell");
        bubbleout(argv[1]);
        strcat(new_home,argv[1]);
        strcpy(argv[1],new_home);
    }//changing the meaning of ~

    if(argv[1][0]=='-'){// To be handled later
        ;
    }
    if (chdir(argv[1])){//if unsuccessful, goes into the condition
        printf("%sError, directory %s is not found",KRED,argv[1]);
    }
    pwd(argc,argv);
    return false;
}

bool exiter(size_t argc, char **argv) {

    return true;
}

bool clear(size_t argc, char **argv) {
    printf ("\033c");
    return false;
}

bool pwd(size_t argc, char **argv) {
    char *temp = NULL;
    temp=malloc(sizeof(char)*BUFSIZE);
    cwd(temp,1);

    free(temp);
    return false;
}

bool echo(size_t argc, char **argv) {
    //as of now prints whatever it is told
    for (int i = 1; argv[i]; ++i) {
        printf("%s%s",KRED,argv[i]);
    }
    printf("\n");
    return false;
}

bool ls(size_t argc, char **argv) {
    
    return false; }


//builtin commands
