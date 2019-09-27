bool exiter(size_t argc,char **argv);
bool cd(size_t argc,char **argv);
bool clear(size_t argc,char **argv);
bool pinfo(size_t argc,char **argv);
bool pwd(size_t argc,char **argv);
bool echo(size_t argc,char **argv);
bool ls(size_t argc,char **argv);
bool historyp(size_t argc,char **argv);
bool unsetenvF(size_t argc,char **argv);
bool setenvF(size_t argc,char **argv);
bool overkill(size_t argc,char **argv);
bool kjob(size_t argc,char **argv);
bool fg(size_t argc,char **argv);
bool bg(size_t argc,char **argv);
bool job_display(size_t argc,char **argv);
void catchCTRL_Z(int sig);
void catchCTRL_C(int sig);



void cwd(char* cwdstr, int command);
//helper function
void UsernHost_names(char usernhoststring[]);
//helper function
void adjust_start(char temp[10000]);
//helper function

void bubbleout(char *string);
//helper function

void prompt(char cwdstr[],char usernhoststring[]);

bool execution(size_t argc, char **argv);
//execution module that directs the action

void add_Chistory(char *command);

void input1(char **pString,char cwdstr[],char usernhoststring[]);

size_t input2(size_t argc, char **argv, char *command);
