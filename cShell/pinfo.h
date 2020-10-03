bool pinfo(const size_t argc,char **argv) {
    char path[200];
    strcpy(path, "/proc/");

    char pid[100];
    if(argc==1){
        sprintf(pid, "%d", getpid());
    }else{
        strcpy(pid,argv[1]);
    }

    strcat(path, pid);

    char exepath[200];
    strcpy(exepath,path);
    strcat(exepath,"/exe");

    strcat(path, "/stat");
    int fd = open(path, O_RDONLY);
    char readstat[800];
    read(fd, readstat, 100);

    char* list[800];
    char * temp = strtok(readstat, " \r\t\n");
    int j=0;
    for(;temp != NULL;++j)
    {
        list[j] = temp;
        temp = strtok(NULL, " \r\t\n");
    }
    list[j] = NULL;

    char execlink[200] = "";
    readlink(exepath, execlink, sizeof(execlink));
    adjust_start(execlink);
    printf("\npid -- %s\n",  list[0]);//pid
    printf("Process Status -- %s\n",  list[2]);//process status
    printf("memory -- %s\n", list[23]); //memory amount
    printf("Executable Path -- %s\n",execlink);
    return false;
}
