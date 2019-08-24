bool echo(const size_t argc,char **argv){
    //as of now prints whatever it is told
//    if (!background){
    for (int i = 1; argv[i]; ++i) {
        printf("%s%s",KRED,argv[i]);
    }
    printf("\n");
    return false;

}
