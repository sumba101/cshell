//
// Created by sumanth on 01/09/19.
//
typedef struct job
{
    char name[100];
    pid_t pid;
    char stat[30]; //true is running, false is stopped
    struct job * next;
}Jobs; //structure to implement the stack for jobs

Jobs *j=NULL; //implementing a queue

bool empty(){
    return j==NULL;
}

void proc_status(char pid[],char status[]){
    char path[200];
    strcpy(path, "/proc/");
    strcat(path, pid);
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

    switch (list[2][0]){
        case 'T':
            strcpy(status,"Stopped");
            break;
        case 't':
            strcpy(status,"Stopped");
            break;
//        case 'D':
//            strcpy(status,"Uninterruptible sleep");
//            break;
        case 'R':
            strcpy(status,"Running");
            break;
//        case 'S':
//            strcpy(status,"Interruptible sleep");
//            break;
//        case 'I':
//            strcpy(status,"Idle");
//            break;
//        case 'X':
//            strcpy(status,"Dead");
//            break;
//        case 'Z':
//            strcpy(status,"Zombie");
//            break;
        default:strcpy(status,"Running");
            break;

    }
}

void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        char temp=str[start];
        str[start]=str[end];
        str[end]=temp;
        start++;
        end--;
    }
}

void itoa(int num, char* str)
{
    int base=10;
    int i = 0;

    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return ;
    }

     // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = rem + '0';
        num = num/base;
    }

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);
}

void update_jobs(){
    ///using proc file to update the background process for maximum correctness
    if(j==NULL){
        return;
    }
    Jobs * temp=j;
    while (temp){
        char pid[50];
        itoa(temp->pid,pid);
        proc_status(pid,temp->stat);
        temp=temp->next;
    }
}


Jobs* getJob(int jno){ ///returns pointer to given job num
    int i=1;
    Jobs * temp=j;
    while (i<jno && temp->next){
        temp=temp->next;
        ++i;
    }
    if (i==jno){
        ///temp points to the job in question
        return temp;
    } else{
        printf("Error, incorrect Jno");
        return NULL;
    }

}

Jobs* findJob(pid_t pid){ ///returns pointer to given job pid
    if(j==NULL){
        return NULL;
    }

    Jobs * temp=j;

    while (temp->pid!=pid && temp->next){
        temp=temp->next;
    }
    if (temp->pid==pid){
        ///temp points to the job in question
        return temp;
    } else{
        return NULL;
    }
}

void change(const pid_t pid, bool state ){
    Jobs * temp=j;
    while (temp){
        if(temp->pid==pid){
            if (state){
                strcpy(temp->stat,"Running");
            } else{
                strcpy(temp->stat,"Stopped");
            }
            break;
        }
        temp=temp->next;
    }
}

void push(const char name[],const pid_t pid, bool state ){
    if (j==NULL){
        j = malloc(sizeof(Jobs));
        strcpy(j->name,name);
        j->pid=pid;
        j->next=NULL;
        if (state){
            strcpy(j->stat,"Running");
        } else{
            strcpy(j->stat,"Stopped");
        }
    } else{
        Jobs * temp=j;
        while (temp->next){
            temp=temp->next;
        }
        temp->next = malloc(sizeof(Jobs));
        temp=temp->next;

        strcpy(temp->name,name);
        temp->pid=pid;
        temp->next=NULL;
        if (state){
            strcpy(temp->stat,"Running");
        } else{
            strcpy(temp->stat,"Stopped");
        }
    }
}

void print_jobs(){
    if(j){
        Jobs *temp=j;
        int i=1;
        char buffer[500];
        do{
            sprintf(buffer,"\n%s[%d] %s %s [%d] \n",KRED,i,temp->stat,temp->name,temp->pid);
            write(1, buffer, strlen(buffer));
            temp=temp->next;
            ++i;
        }while (temp);

    }

}


void pop(){
    if(j == NULL){
        return; //list is empty
    }
    Jobs *temp=j;
    j=temp->next;
    free(temp);
}

void erase_jobs(){
    while(!empty()){pop();}
    ///TERRIBLY INEFFICIENT BUT NO TIME TO OPTIMIZE
}

void remove_j(pid_t pid){
    Jobs *temp1=j,*temp2=NULL;
    if(j->pid==pid){
        j=j->next;
        free(temp1);
        return;
    }

    temp2=temp1;
    temp1=temp1->next;

    while (temp1){
        if(temp1->pid=pid){
            temp2->next=temp1->next;
            free(temp1);
            return;
        }
        temp2=temp1;
        temp1=temp1->next;
    }
}

void overkill_jobs(){
    Jobs *temp = j;
    while (temp->next){
        kill(temp->pid, 17);
        kill(temp->pid, SIGINT);
        ///   printf("%s Killed %d - %s\n", KRED, temp->pid, temp->name); this should be handled by child end sig handler
        temp=temp->next;
    }
    kill(temp->pid, 17);
    kill(temp->pid, SIGINT);
    ///printf("%s Killed %d - %s\n", KRED, temp->pid, temp->name);
    erase_jobs(); ///clearing the background jobs list
}

