#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <fcntl.h>
#define DEF "\x1B[0m"
/* Just a fancy name printing function*/
void nameFile(struct dirent* name,char* followup)
{
    if(name->d_type == DT_REG)          // regular file
    {
        printf("%s%s", name->d_name, followup);
    }
    else if(name->d_type == DT_DIR)    // a directory
    {
        printf("%s/%s", name->d_name, followup);
    }
    else                              // unknown file types
    {
        printf("%s%s", name->d_name, followup);
    }
}


int main(int argc,char **argv) { 
	// int i=0;
	// char file[100];
	// scanf("%s",file);

 //    struct dirent **listr; //object of dirent
 //    int listn = scandir(file, &listr, NULL, alphasort); //count of number of elements in the directory
    
 //    if (listn >= 0)
 //    {
 //        for(i = 0; i < listn; i++ )
 //        {
 //           nameFile(listr[i],"\n");
            
 //        }
 //        printf("\n");
 //    }
for (int i = 0; i < 100; ++i)
{

	printf("\a\n");	/* code */
}
return 0; 
}


