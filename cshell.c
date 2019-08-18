#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
/* List of all things to implement
 * Display requirement
 * cd
 * pwd
 * echo
 * background processes
 * vi
 * emacs and more
 * pinfo
 *
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

void cwd(char* cwdstr,int command);

void UsernHost_names(char usernhoststring[]);

void adjust_start(char temp[10000]);

int main()
{
//    printf("%sred\n", KRED);
//    printf("%sgreen\n", KGRN);
//    printf("%syellow\n", KYEL);
//    printf("%sblue\n", KBLU);
//    printf("%smagenta\n", KMAG);
//    printf("%scyan\n", KCYN);
//    printf("%swhite\n", KWHT);
//    printf("%snormal\n", KNRM);
    char cwdstr[BUFSIZE],usernhoststring[2*BUFSIZE];
    cwd(cwdstr,0);
    UsernHost_names(usernhoststring);

    printf("<%s%s:~%s>",KRED,usernhoststring,cwdstr);


    return 0;
}

/home/sumanth/OS/2018114002_CShell
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
            printf("%s\n",cwdstr);
        }
    }
    else perror("+--- Error in getcwd() : ");

}

void adjust_start(char temp[]) {
    //const char new_home[]="/home/sumanth/OS/2018114002_CShell";
    char new_home[10000];
    strcpy(new_home,"/home/sumanth/OS/2018114002_CShell");
    int i = 0;
    for (; new_home[i] && new_home[i]==temp[i] ; ++i) {}
    if (i<strlen(temp)){
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

