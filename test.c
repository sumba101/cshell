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

/* list cwd contents*/
void function_ls()
 {
// if(argc==1 || argv[1]=="."){
//     int i=0;
//     struct dirent **listr; //object of dirent
//     int listn = scandir(".", &listr, NULL, alphasort); //count of number of elements in the directory
//     if (listn >= 0)
//     {
//         for(i = 0; i < listn; i++ )
//         {
//            nameFile(listr[i],"\n");
            
//         }
//         printf("\n");
//     }
//     else
//     {
//         perror ("+--- Error in ls ");
//     }

// }
// elseif(argv[1]=="-l"){

    int i=0,total=0;
    char timer[14];
    struct dirent **listr;
    struct stat details;
    int listn = scandir(".",&listr,0,alphasort);
    if(listn > 0)
    {
       for ( i = 0; i < listn; i++)
        {    
        	if(argc==1 || argv[1]=="."||!strcmp(argv[1],"-a")){
        		if(((listr[i]->d_name)[0]=="." || strcmp(listr[i]->d_name,"..")==0)&&strcmp(argv[1],"-a"))
            	{
                	continue;
            	}//skips hidden files
            	

        		if(listn[i]->d_type == DT_REG)          // regular file
    				{
    			    printf("%s%s", listn[i]->d_name, "\n");
    				}
    			else if(listn[i]->d_type == DT_DIR)    // a directory
    			{
        			printf("%s/%s", listn[i]->d_name, "\n");
    			}
    			else                              // unknown file types
    			{
        			printf("%s%s", listn[i]->d_name, "\n");
    			}

        	}
        	else if(strcmp(argv[1],"-l")==0 ||!strcmp(argv[1],"-al")||!strcmp(argv[1],"-la")){
        		
        		if(((listr[i]->d_name)[0]=="." || strcmp(listr[i]->d_name,"..")==0)&&!strcmp(argv[1],"-l"))
            	{
                	continue;
            	}//skips hidden files
            	

        		if(stat(listr[i]->d_name,&details)==0)
            	{
                // example - -rwxrwxr-x 1 user user  8872 Jan 26 10:19 a.out*
                // owner permissions - group permissions - other permissions
                // links associated - owner name - group name
                // file size (bytes) - time modified - name
                total += details.st_blocks; // block size
                
                // owner permissions - group permissions - other permissions
                printf("%s%1s",DEF,(S_ISDIR(details.st_mode)) ? "d" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IRUSR) ? "r" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IWUSR) ? "w" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IXUSR) ? "x" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IRGRP) ? "r" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IWGRP) ? "w" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IXGRP) ? "x" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IROTH) ? "r" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IWOTH) ? "w" : "-");
                printf("%s%1s ",DEF,(details.st_mode & S_IXOTH) ? "x" : "-");
                // links associated - owner name - group name
                printf("%2ld ",(unsigned long)(details.st_nlink));
                printf("%s ",(getpwuid(details.st_uid))->pw_name);
                printf("%s ",(getgrgid(details.st_gid))->gr_name);
                // file size (bytes) - time modified - name
                printf("%5lld ",(unsigned long long)details.st_size);
                strftime (timer,14,"%h %d %H:%M",localtime(&details.st_mtime));
                printf("%s ",timer);
                nameFile(listr[i],"\n");
            	}	
        	}
        	else
        	{
        		//error handling
        	}
            
        }
        printf("+--- Total %d object contents\n",total/2);
    }
    else
    {
            printf("+--- Empty directory\n" );
    }

// 	}
// elseif(argv[1]=="-a"){

// }
// elseif(argv[1]=="-la"){

// }
// else{
// 	perror("incorrect");
// }
}

int main(int argc,char **argv) { 
function_ls();

return 0; }


