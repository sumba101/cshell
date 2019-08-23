bool exiter(size_t argc,char **argv);
bool cd(size_t argc,char **argv);
bool clear(size_t argc,char **argv);
bool pinfo(size_t argc,char **argv);
bool pwd(size_t argc,char **argv);
bool echo(size_t argc,char **argv);
bool ls(size_t argc,char **argv);
bool historyp(size_t argc,char **argv);

void cwd(char* cwdstr, int command);
//helper function
void UsernHost_names(char usernhoststring[]);
//helper function
void adjust_start(char temp[10000]);
//helper function

void bubbleout(char *string);
//helper function

void prompt(char cwdstr[],char usernhoststring[]);

bool execution(size_t argc, char **argv, bool background);
//execution module that directs the action

void add_history(char *command);

void input1(char **pString);

size_t input2(size_t argc, char **argv, bool *background, char *command);
