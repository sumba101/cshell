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
    free(lgn);
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
    if(strlen(temp)>=strlen(new_home)){
        int i = 0;
        for (; new_home[i] && new_home[i]==temp[i] ; ++i) {}
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
