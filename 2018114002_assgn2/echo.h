bool echo(const size_t argc,char **argv){
    //as of now prints whatever it is told
//    if (!background){
    for (int i = 1; argv[i]; ++i) {
        printf("%s%s",KRED,argv[i]);
    }
    printf("\n");
//    } else{
//        pid_t pid;
//
//        if ((pid=fork())<0){ perror("Error in forking\n");}
//        else if(pid==0){///in child
//            for (int i = 1; argv[i]; ++i) {
//                printf("%s%s",KRED,argv[i]);
//            }
//            printf("\n");
//            exit(1);
//        } else{ ///parent process
//                signal(SIGCHLD,child_end(argv,pid));
//        }
//    }
    return false;

}
