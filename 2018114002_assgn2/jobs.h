//
// Created by sumanth on 01/09/19.
//

#ifndef INC_2018114002_CSHELL_JOBS_H
#define INC_2018114002_CSHELL_JOBS_H
#endif //INC_2018114002_CSHELL_JOBS_H

#define MAXJOBS 1000
typedef struct job
{
    char name[1000];
    pid_t pid;
    bool stat; //true is running, false is stopped
}Jobs; //structure to implement the stack for jobs

Jobs j[MAXJOBS];
int itr=-1;

bool empty(){
    return itr==-1;
}

bool checkJob(int jno){
    if(itr<jno)
        return false;
    return true;
}

Jobs getJob(int jno){
    return j[jno];
}

void change(const pid_t pid, bool state ){
    bool found=false;
    for (int i = 0; i <= itr && !found ; ++i) {
        if(j[i].pid==pid){
            j[i].stat=state;
            found=true;
        }
    }
}

void push(const char name[],const pid_t pid, bool state ){
    ++itr;
    if(itr > MAXJOBS){
        perror("Jobs stack is full");
    }
    strcpy(j[itr].name,name);
    j[itr].stat=state;
    j[itr].pid=pid;

}
void pop(){
    if(itr == -1){
        return; //stack is empty
    }
    --itr;
}
Jobs top(){
    Jobs t;
    t.pid=j[itr].pid;
    t.stat=j[itr].stat;
    stpcpy(t.name,j[itr].name);
    return t;
}
