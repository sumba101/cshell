bool historyp(const size_t argc,char **argv){
    int limit=10;
    if (argc == 2){
        limit=atoi(argv[1]);
    } //takes value given by the user as limit

    for (int i=(limit>hist_counter)?hist_counter:limit;i>0;--i) {

        printf("%s%s\n",KRED,history[(hist_counter-i)%20]);
    }
    return false;
}
void add_history(char *command) {
    int pos=hist_counter%20;

    strcpy(history[pos],command);
    ++hist_counter;
}
