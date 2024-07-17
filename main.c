#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include "main.h"
int main()
{   
    using_history();
    printf("\n-------------------------CS22BTECH11013 - Software Development Fundamentals - My Shell-------------------------\n\n");
    while(1)
    {   
        char *str=malloc(sizeof(char)*4096);
        char name[200],host[200];
        char test[512];
        
        getlogin_r(test,sizeof(test));
    	gethostname(host,sizeof(host));
	    getcwd(name,sizeof(name));
        // strcat(test,orange);
        
        strcat(test,"@");
	    strcat(test,host);
       
    	strcat(test,":");
        
    	strcat(test,name);
       
	    strcat(test,"$ ");
        
        str=readline(test);
        removetrailingspaces(str);
        if(strcmp(str,"exit")==0)
        {
            printf("\n----------------------------------------Session Over----------------------------------------\n\n");
            break;
        }
        if(str[0]=='c'&&str[1]=='d'&&(str[2]==' '||str[2]=='\0'))
		{
			char line[100];
			if(strcmp(str,"cd")==0)
			{
				chdir("/");
                
				continue;
			}
			strncpy(line,str+3,100);
			chdir(line);
			add_history(str);
            
			continue;
		}
        if(strcmp(str,"help")==0||strcmp(str,"ls --help")==0||strcmp(str,"ps --help")==0||strcmp(str,"cp --help")==0||strcmp(str,"mv --help")==0||strcmp(str,"grep --help")==0)
        {
            help(str);
        }
        else if(str[0]=='l'&&str[1]=='s')
        {
            lscombined(str);
        }
        else if(str[0]=='p'&&str[1]=='s')
        {
            pscombined(str);
        }
        else if(str[0]=='g'&&str[1]=='r'&&str[2]=='e'&&str[3]=='p')
        {
            grepcombined(str);
        }
        else if(str[0]=='c'&&str[1]=='p')
        {
            cpcombined(str);
        }
        else if(str[0]=='m'&&str[1]=='v')
        {
            cpcombined(str);
        }
        else
        {
            printf("Invalid Command\nRead \"help\" \n");
        }
        add_history(str);
    }
    return 0;
}
