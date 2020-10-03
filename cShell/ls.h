bool ls(const size_t argc,char **argv){

    int i=0;
    char timer[14];
    struct dirent **listr;
    struct stat details;

    ///argv[0] is ls, for 1 to argc, check if first letter is a .
    char filepath[BUFSIZE];
    strcpy(filepath,".");///default

    int state=0;// 0 if just ls, 1 if -a , 2 if -l, 3 if -a and -l

    for (int j = 1; argv[j] ; ++j) {

        if (strcmp(argv[j],"-a")!=0 && strcmp(argv[j],"-l")!=0 && strcmp(argv[j],"-al")!=0 && strcmp(argv[j],"-la")!=0){
            strcpy(filepath,argv[j]);
        }
        else if (strcmp(argv[j],"-a")==0){
            state+=1;
        }
        else if (strcmp(argv[j],"-l")==0){
            state+=2;
        }
        else if (strcmp(argv[j],"-al")==0||strcmp(argv[j],"-la")==0){
            state+=3;
        }
    }

    char current[BUFSIZE];
    ///finding the current directory

    if(getcwd(current, sizeof(current)) == NULL)
        perror("+--- Error in getcwd() : ");

    ///changing into the directory before doing ls. changing out of it is done in the end

    if(filepath[0]=='~'){
        char new_homeC[10000];
        strcpy(new_homeC,new_home);
        bubbleout(filepath);
        strcat(new_homeC,filepath);
        strcpy(filepath,new_homeC);
    }//changing the meaning of ~

    if (chdir(filepath)){//if unsuccessful, goes into the condition
        printf("%sError, directory %s is not found",KRED,filepath);
    }
    ///control now exists in the given directory
    int listn = scandir(".",&listr,NULL,alphasort);//takes everything in the input directory

    if(listn > 0)//listn contains the number of entries in the specified directory
    {
        for ( i = 0; i < listn; i++)
        {
            if(state==0||state==1){ //nothing || -a
                if(( (listr[i]->d_name)[0]=='.' || !strcmp(listr[i]->d_name,".."))&&state==0)
                {
                    continue;
                }//skips hidden files

                if(listr[i]->d_type == DT_DIR)    // a directory
                {
                    printf("%s/%s", listr[i]->d_name, "\n");
                }
                else                              // unknown file types
                {
                    printf("%s%s", listr[i]->d_name, "\n");
                }

                //this functions as a just ls or ls with -a
            }
            else if(state==2||state==3){   //l|| -la

                if(( (listr[i]->d_name)[0]=='.' || !strcmp(listr[i]->d_name,".."))&&state==2)
                {
                    continue;
                }//skips hidden files


                if(stat(listr[i]->d_name,&details)==0)
                {
                    // owner permissions - group permissions - other permissions
                    printf("%s%1s",DEF,(S_ISDIR(details.st_mode)) ? "d" : "-");
                    printf("%s%1s",DEF,(details.st_mode & S_IRUSR) ? "r" : "-");
                    printf("%s%1s",DEF,(details.st_mode & S_IWUSR) ? "w" : "-");
                    printf("%s%1s",DEF,(details.st_mode & S_IXUSR) ? "x" : "-");
                    printf("%s%1s",DEF,(details.st_mode & S_IRGRP) ? "r" : "-");
                    printf("%s%1s",DEF,(details.st_mode & S_IWGRP) ? "w" : "-");
                    printf("%s%1s",DEF,(details.st_mode & S_IXGRP) ? "x" : "-");
                    printf("%s%1s",DEF,(details.st_mode & S_IROTH) ? "r" : "-");
                    printf("%s%1s",DEF,(details.st_mode & S_IWOTH) ? "w" : "-");
                    printf("%s%1s",DEF,(details.st_mode & S_IXOTH) ? "x" : "-");
                    // links associated - owner name - group name
                    printf("%2ld ",(unsigned long)(details.st_nlink));
                    printf("%s ",(getpwuid(details.st_uid))->pw_name);
                    printf("%s ",(getgrgid(details.st_gid))->gr_name);
                    // file size (bytes) - time modified - name
                    printf("%5lld ",(unsigned long long)details.st_size);
                    strftime (timer,14,"%h %d %H:%M",localtime(&details.st_mtime));
                    printf("%s ",timer);
                    if(listr[i]->d_type == DT_DIR)    // a directory
                    {
                        printf("%s/%s", listr[i]->d_name, "\n");
                    }
                    else                              // unknown file types
                    {
                        printf("%s%s", listr[i]->d_name, "\n");
                    }

                }
            }
        }
    }
    else
    {
        printf("+--- Empty directory\n" );
    }

    ///now shifting back into original directory
    if (chdir(current)){//if unsuccessful, goes into the condition
        printf("%sError, directory %s is not found",KRED,current);
    }


    for (int i = 0; i < listn; i++)
    {
        free(listr[i]);
    }

    free(listr);

    return false;

}