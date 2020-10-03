bool pwd(const size_t argc,char **argv){
    char *temp = NULL;
    temp=malloc(sizeof(char)*BUFSIZE);
    cwd(temp,1);

    free(temp);
    return false;
}
