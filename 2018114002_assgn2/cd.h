bool cd(const size_t argc,char **argv){
    //returns true in case of failure after warning
    if(argc!=2){
        printf("%sError, command is incorrect",KRED);
    }
    if(argv[1][0]=='~'){
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
