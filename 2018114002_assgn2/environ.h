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
