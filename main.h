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

const char* orange = "\033[38;2;255;165;0m";
const char* magenta = "\033[0;35m";
const char* reset = "\033[0m";

/* Function Declarations */

void ps();
void pse();
void psd();
void psa();
void psj();
void grep(char*);
void help(char*);

/* Struct Declarations */

struct filp
{
	char *name,access[100],time[100],*pwname,*grname;
	long int nlink,size;
};
struct pspro
{
    int pid,ppid,sid,uid,psr,pgrp;
    char username[512],status,ntime[512],s_time[512];
    unsigned long long int rss,sz,utime,stime,cuttime,cstime;
    time_t STime,c;
    char command[512];
};

/* Function Definitons */

char prompt()
{
    char name[200],host[200];
    char *test=(char*)malloc(sizeof(char)*512);
    getlogin_r(test,sizeof(test));
	gethostname(host,sizeof(host));
	getcwd(name,sizeof(name));
    strcat(test,"@");
	strcat(test,host);
	strcat(test,":");
	strcat(test,name);
	strcat(test,"$ ");
    return *test;
}
void removetrailingspaces(char* str)
{
	int a=0,b=-1;
	while(str[a]!='\0')
	{
		if(str[a]!=' '&&str[a]!='\t'&&str[a]!='\n')
			b=a;
		a++;
	}
	str[b+1]='\0';
}
void sort_by_name(struct filp* list,int filecount)
{
	struct filp temp;
	for(int i=0;i<filecount;i++)
	{
		for(int j=i+1;j<filecount;j++)
		{
			if(strcmp(list[i].name,list[j].name)>0)
			{
				temp=list[i];
				list[i]=list[j];
				list[j]=temp;
			}
		}
	}
}
void lscombined(char* str)
{
    if(strcmp(str,"ls")!=0&&strcmp(str,"ls -l")!=0&&strcmp(str,"ls -a")!=0&&strcmp(str,"ls -al")!=0&&strcmp(str,"ls -1")!=0)
    {
        printf("Invalid Command\nRead \"ls --help\" for more information\n");
        return;
    }
    else
    {
        char name1[512];
        memset(name1, '\0', 512);
		getcwd(name1,sizeof(name1));
		int filecount=0;
		struct dirent *a;
		DIR *dir=opendir(name1);

		while((a=readdir(dir))!=NULL)
		{
			filecount++;
		}

		closedir(dir);

		dir=opendir(name1);

		struct filp* list=(struct filp*)calloc(filecount,sizeof(struct filp));
		struct stat s;
		struct passwd *p;
		struct group *g;
        int blockcount=0;

		stat(".",&s);

		for(int i=0;i<filecount;i++)
		{
			a=readdir(dir);
			stat(a->d_name,&s);

			strcat(list[i].access, (S_ISDIR(s.st_mode)) ? "d" : "-");
            strcat(list[i].access, (s.st_mode & S_IRUSR) ? "r" : "-");
            strcat(list[i].access, (s.st_mode & S_IWUSR) ? "w" : "-");
            strcat(list[i].access, (s.st_mode & S_IXUSR) ? "x" : "-");
            strcat(list[i].access, (s.st_mode & S_IRGRP) ? "r" : "-");
            strcat(list[i].access, (s.st_mode & S_IWGRP) ? "w" : "-");
            strcat(list[i].access, (s.st_mode & S_IXGRP) ? "x" : "-");
            strcat(list[i].access, (s.st_mode & S_IROTH) ? "r" : "-");
            strcat(list[i].access, (s.st_mode & S_IWOTH) ? "w" : "-");
            strcat(list[i].access, (s.st_mode & S_IXOTH) ? "x" : "-");

			list[i].name=a->d_name;

			list[i].nlink=s.st_nlink;

			list[i].size=s.st_size;

			p=getpwuid(s.st_uid);
			list[i].pwname=p->pw_name;

			g=getgrgid(s.st_gid);
			list[i].grname=g->gr_name;

            if(strcmp(str,"ls -l")!=0||a->d_name[0]!='.')
            blockcount+=s.st_blocks;

			struct tm timelocal;
			timelocal=*localtime(&s.st_mtime);
			strftime(list[i].time,sizeof(list[i].time),"%b %d %H:%M",&timelocal);
		}

		sort_by_name(list,filecount);

		if(strcmp(str,"ls")==0||strcmp(str,"ls -a")==0||strcmp(str,"ls -1")==0)
		for(int i=0;i<filecount;i++)
		{
			if(list[i].name[0]=='.'&&(strcmp(str,"ls")==0||strcmp(str,"ls -1")==0))
			continue;
			printf("%s     ",list[i].name);
			if(strcmp(str,"ls -1")==0)
			printf("\n");
			
		}
		else if(strcmp(str,"ls -l")==0||strcmp(str,"ls -al")==0)
        {
            printf("total %d\n",blockcount/2);
		    for(int i=0;i<filecount;i++)
    		{
	    		if(list[i].name[0]=='.'&&strcmp(str,"ls -l")==0)
		    	continue;

	    		printf("%s  %3ld  %s  %s  %8ld  %s  %s\n",list[i].access,list[i].nlink,list[i].pwname,list[i].grname,list[i].size,list[i].time,list[i].name);
		    }
		    
        }
        free(list);
        printf("\n");
    }
}
void pscombined(char*str)
{
    if(strcmp(str,"ps")==0)
    {
        ps();
    }
    else if(strcmp(str,"ps -e")==0||strcmp(str,"ps -A")==0)
    {
        pse();
    }
    else if(strcmp(str,"ps -d")==0)
    {
        psd();
    }
    else if(strcmp(str,"ps -a")==0)
    {
        psa();
    }
    else if(strcmp(str,"ps -j")==0)
    {
        psj();
    }
    else
    {
        printf("Invalid Command\nRead \"ps--help\" for more information\n");
        return;
    }
}
void printtime(time_t a)
{
    time_t s,m,h;
    s=a%60;
    m=(a/60)%60;
    h=(a/3600)%3600;
    printf("        %02lu:%02lu:%02lu",h,m,s);
    return;
}
int getty(char* pid)
{
    char *path=malloc(sizeof(char)*512);

    sprintf(path,"/proc/%s/fd/0",pid);

    int ttyval=open(path,O_RDONLY);

    return ttyval;
}
void getstat(char* pid,struct pspro *present)
{
    FILE *fileptr;

    char dest[512]="/proc/";
    strcat(dest,pid);
    strcat(dest,"/stat");

    fileptr=fopen(dest,"r");

    fscanf(fileptr , "%d %s %c %d %d %d %*d %*d %*u %*d %*d %*d %*d %lld %lld %lld %lld %*d %*d %*d %*d %ld %*d %llu %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %d",
    &present->pid , present->command , &present->status , &present->ppid,&present->pgrp,&present->sid
    ,&present->utime, &present->stime  , &present->cuttime,&present->cstime,&present->STime
    , &present->rss , &present->psr /*, &present->c*/);

    present->c = (double)(present->stime+present->utime)/sysconf(_SC_CLK_TCK);
    present->rss*=4;
    for(int i=0;i<strlen(present->command);i++)
    {
        present->command[i]=present->command[i+1];
    }
    present->command[strlen(present->command)-1]='\0';

    fclose(fileptr);
}
void ps()
{
    char *mytty=ttyname(STDIN_FILENO);
    char mytty2[512];
    strcpy(mytty2,mytty);

    struct dirent *a;
    DIR *dir = opendir("/proc");

    printf("    PID      TTY            TIME          CMD\n");

    while((a=readdir(dir))!=NULL)
    {
        if(a->d_name[0]<48 || a->d_name[0]>57)
        {
            continue;
        }
        else
        {
            char tty3[512];
            memset(tty3, '\0', 512);
            struct pspro present;
            getstat(a->d_name, &present);
            char path[512];
            memset(path, '\0', 512);
            sprintf(path, "/proc/%s/fd/0",a->d_name);
           
            int val = open(path, O_RDONLY);
            
            //printf("hi %s\n", ttyname(getty(present.pid)));
            if (ttyname(val)!=NULL)
            strcpy(tty3, ttyname(val));
            close(val);
            if (tty3 != NULL)
            {
                
                if (strcmp(tty3, mytty2) != 0)
                {
                    // printf("yo!");
                    continue;
                }
            }
            printf("%7d   %7s ",present.pid,tty3+5);
            printtime(present.c);
            printf("      %s\n",present.command);
        }
    }
    closedir(dir);
}
void pse()
{
    char *mytty=malloc(sizeof(char)*4096);
    mytty=ttyname(STDIN_FILENO);
    char mytty2[512];
    strcpy(mytty2,mytty);

    struct dirent *a;
    DIR *dir = opendir("/proc");

    printf("     PID       TTY          TIME          CMD\n");

    while((a=readdir(dir))!=NULL)
    {
        if(a->d_name[0]<48 || a->d_name[0]>57)
        {
            continue;
        }
        else
        {
            char tty3[512];
            memset(tty3, '\0', 512);
            struct pspro present;
            getstat(a->d_name, &present);
            char path[512];
            memset(path, '\0', 512);
            sprintf(path, "/proc/%s/fd/0",a->d_name);
           
            int val = open(path, O_RDONLY);
            
            //printf("hi %s\n", ttyname(getty(present.pid)));
            if (ttyname(val)!=NULL)
            strcpy(tty3, ttyname(val));
            else
            strcpy(tty3,"     ?");
            close(val);
            // if (tty3 != NULL)
            // {
                
            //     if (strcmp(tty3, mytty2) != 0)
            //     {
            //         // printf("yo!");
            //         continue;
            //     }
            // }
            printf("%7d   %7s ",present.pid,tty3 ? tty3+5 : "?");
            printtime(present.c);
            printf("      %s\n",present.command);
        }
    }
    closedir(dir);
    return;
}
void psd()
{
    char *mytty=ttyname(STDIN_FILENO);
    char mytty2[512];
    strcpy(mytty2,mytty);

    struct dirent *a;
    DIR *dir = opendir("/proc");

    printf("     PID       TTY          TIME          CMD\n");

    while((a=readdir(dir))!=NULL)
    {
        if(a->d_name[0]<48 || a->d_name[0]>57)
        {
            continue;
        }
        else
        {
            char tty3[512];
            memset(tty3, '\0', 512);
            struct pspro present;
            getstat(a->d_name, &present);
            if(present.pid==present.sid)
            continue;
            char path[512];
            memset(path, '\0', 512);
            sprintf(path, "/proc/%s/fd/0",a->d_name);
           
            int val = open(path, O_RDONLY);
            
            //printf("hi %s\n", ttyname(getty(present.pid)));
            if (ttyname(val)!=NULL)
            strcpy(tty3, ttyname(val));
            else
            strcpy(tty3,"     ?");
            close(val);
            
            
            printf("%7d   %7s ",present.pid,tty3+5);
            printtime(present.c);
            printf("      %s\n",present.command);
        }
    }
}
void psa()
{
    char *mytty=ttyname(STDIN_FILENO);
    char mytty2[512];
    memset(mytty2, '\0', 512);
    strcpy(mytty2,mytty);

    struct dirent *a;
    DIR *dir = opendir("/proc");

    printf("    PID      TTY            TIME          CMD\n");

    while((a=readdir(dir))!=NULL)
    {
        if(a->d_name[0]<48 || a->d_name[0]>57)
        {
            continue;
        }
        else
        {
            char tty3[512];
            memset(tty3, '\0', 512);
            struct pspro present;
            getstat(a->d_name, &present);
            if(present.pid==present.sid)
            continue;
            char path[512];
            memset(path, '\0', 512);
            sprintf(path, "/proc/%s/fd/0",a->d_name);
           
            int val = open(path, O_RDONLY);
            
            //printf("hi %s\n", ttyname(getty(present.pid)));
            if (ttyname(val)!=NULL)
            strcpy(tty3, ttyname(val));
            else
            continue;
            close(val);
            
            printf("%7d   %7s ",present.pid,tty3+5);
            printtime(present.c);
            printf("      %s\n",present.command);
        }
    }
}
void psj()
{
    char *mytty=ttyname(STDIN_FILENO);
    char mytty2[512];
    strcpy(mytty2,mytty);

    struct dirent *a;
    DIR *dir = opendir("/proc");

    printf("    PID      PGID     SID    TTY           TIME          CMD\n");

    while((a=readdir(dir))!=NULL)
    {
        if(a->d_name[0]<48 || a->d_name[0]>57)
        {
            continue;
        }
        else
        {
            char tty3[512];
            memset(tty3, '\0', 512);
            struct pspro present;
            getstat(a->d_name, &present);
            char path[512];
            memset(path, '\0', 512);
            sprintf(path, "/proc/%s/fd/0",a->d_name);
           
            int val = open(path, O_RDONLY);
            
            //printf("hi %s\n", ttyname(getty(present.pid)));
            if (ttyname(val)!=NULL)
            strcpy(tty3, ttyname(val));
            close(val);
            if (tty3 != NULL)
            {
                
                if (strcmp(tty3, mytty2) != 0)
                {
                    // printf("yo!");
                    continue;
                }
            }
            printf("%7s   %7d %7d %7s",a->d_name,present.pgrp,present.sid,tty3+5);
            printtime(present.c);
            printf("      %s\n",present.command);
        }
    }
}
void grepcombined(char* str)
{
    if(str[0]=='g'&&str[1]=='r'&&str[2]=='e'&&str[3]=='p')
    {
        if(str[5]!='-')
        {
            grep(str);
        }
        else
        {
            if(str[7]!=' ')
            {
                printf("Invalid Command\nRead \"grep --help\" for more information");
                return;
            }
            else if(str[6]=='n')
            grep(str);
            else if(str[6]=='i')
            grep(str);
            else if(str[6]=='c')
            grep(str);
            else if(str[6]=='w')
            grep(str);
            else
            {
                printf("Invalid Command\nRead \"grep --help\" for more information");
                return;
            }
        }
    }
}
void grep(char* str)
{
    int j=5;
    if(strlen(str)<=5)
    return;
    if(str[5]=='-')
    {
        while(str[j]!=' ')
        j++;
        j++;
    }
    int b;
    char *word=malloc(sizeof(char)*512);
    char a;
    if(str[j]=='"')
    a='"';
    else
    a=' ';

    if(a=='"')
    j++;
    b=j;
    while(j<strlen(str)&&str[j]!=a)
    {
        word[j-b]=str[j];
        j++;
    }
    word[j-b]='\0';
    int size=strlen(word);
    if(a=='"')
    {
        j++;
    }

    if(str[5]=='-'&&str[6]=='i')
    {
        for(int i=0;i<size;i++)
        {
            if(word[i]>=65&&word[i]<=90)
            {
                word[i]+=32;
            }
        }
    }

    int mud=j;

    while(j!=strlen(str))
    {
        char *file=malloc(sizeof(char)*16);
        j++;
        int k=j;
        while(str[j]!=' '&&j!=strlen(str))
        {
            file[j-k]=str[j];
            j++;
        }
        file[j-k]='\0';
        FILE* mudptr=fopen(file,"r");
        if(mudptr==NULL)
        {
            printf("%s doesn't exist (or) Invalid\n",file);
            return;
        }
        else
        fclose(mudptr);
    }

    j=mud;

    while(j!=strlen(str))
    {
        char *file=malloc(sizeof(char)*16);
        j++;
        int k=j;
        while(str[j]!=' '&&j!=strlen(str))
        {
            file[j-k]=str[j];
            j++;
        }
        file[j-k]='\0';
        FILE *fileptr=fopen(file,"r");
        char line[2048],dummy[2048];
        int linenumber=0,linecount=0;
        while(fgets(line,2048,fileptr)!=NULL)
        {
            linenumber++;
            int pos[2048],position=0,z=1;
            for(int i=0;i<2048;i++)
            {
                pos[i]=-1;
            }
            if(str[5]=='-'&&str[6]=='i')
            {
                for(int i=0;i<strlen(line);i++)
                {
                    dummy[i]=line[i];
                    if(line[i]>=65&&line[i]<=90)
                    {
                        line[i]+=32;
                    }
                }
            }
            for(int i=0;i<strlen(line);i++)
            {
                if(line[i]!=word[0])
                continue;
                else
                {
                    z=1;
                    for(int j=0;j<strlen(word);j++)
                    {
                        if(i+j>=strlen(line))
                        {
                            z=0;
                            break;
                        }
                        if(word[j]!=line[i+j])
                        {
                            z=0;
                            break;
                        }
                    }
                    if(z==1)
                    {
                        pos[position]=i;
                        position++;
                        i+=strlen(word)-1;
                    }
                }
            }
            if(position!=0)
            linecount++;

            position=0;

            int wcount=0;

           
            for(int i=0;i<4096;i++)
            {
                if(pos[i]==-1)
                break;

                
                if((line[pos[i]-1]==' '||pos[i]==0)&&(line[pos[i]+strlen(word)]==' '||pos[i]+strlen(word)==strlen(line)-1))
                {
                    wcount++;
                }
                
            }
            
            if(wcount==0&&str[5]=='-'&&str[6]=='w')
            {
                continue;
            }

            if(pos[position]==-1)
            continue;

            if(str[5]=='-'&&str[6]=='c')
            continue;

            printf("%s:",file);

            if(str[5]=='-'&&str[6]=='n')
            printf(" %d:",linenumber);

            if(str[5]=='-'&&str[6]=='i')
            {
                for(int i=0;i<strlen(line);i++)
                {
                    line[i]=dummy[i];
                }
            }

            for(int i=0;i<strlen(line);i++)
            {
                if(i!=pos[position])
                printf("%c",line[i]);
                else if((str[5]!='-'||str[6]!='w')&&i==pos[position])
                {
                    printf("\033[1;31m");
                    for(int l=0;l<strlen(word);l++)
                    {
                        printf("%c",line[l+i]);
                    }
                    printf("\x1b[0m");
                    i+=strlen(word)-1;
                    position++;
                }
                else if(str[5]=='-'&&str[6]=='w'&&i==pos[position])
                {
                    
                    if((line[i-1]==' '||i==0)&&(line[i+strlen(word)]==' '||i+strlen(word)==strlen(line)-1))
                    {
                        printf("\033[1;31m");
                        printf("%s",word);
                        printf("\x1b[0m");
                    }
                    else
                    {
                        printf("%s",word);
                    }
                    i+=strlen(word)-1;
                    position++;
                }
            }
        }
        if(str[5]=='-'&&str[6]=='c')
        {
            printf("%s:%d",file,linecount);
        }
        printf("\n");
    }
}
void cpcombined(char* str)
{
    char *dest=malloc(sizeof(char)*512);
			int a=0;
			for(int i=strlen(str)-2;i>=0;i--)
			{
				if(str[i]==' ')
				{
					a=i;
					break;
				}
			}
			for(int i=a+1;i<=strlen(str)-1;i++)
			{
				dest[i-a-1]=str[i];
			}
			dest[strlen(str)-a-1]='\0';
            // if(fopen(dest,"r")==NULL)
            // {
            //     if(opendir(dest)==NULL)
            //     {
            //         printf("Idiot\n");
            //         return;
            //     }
            // }
			int j=2,argcount=0,p=4;
			char cpop='a';
			if(str[3]=='-')
			{
				cpop=str[4];
			}
            if(str[4]=='n'&&str[5]=='i'&&str[6]==' ')
            cpop='i';
            else if(str[4]=='i'&&str[5]=='n'&&str[6]==' ')
            cpop='i';

            if(cpop!='i'&&cpop!='n'&&cpop!='v'&&str[3]=='-'&&cpop!='u')
            {
                printf("Invalid option\nRead \"help\" for more information\n");
            }


			while(j!=a)
			{
				char *src=malloc(sizeof(char)*512);
				j++;
				int k=j;
				while(str[j]!=' ')
					j++;

				if(str[3]=='-'&&k==3)
				continue;

				for(int i=k;i<j;i++)
				{
					src[i-k]=str[i];
				}
				src[j-k]='\0';
                argcount++;
                FILE *dummy=fopen(src,"r");
                if(dummy==NULL)
                {
                    printf("%s doesn't exist\n",src);
                    return;
                }
                else
                {
                    fclose(dummy);
                }
			}
			
			if(str[3]=='-')
			argcount--;				
			if(argcount>1&&opendir(dest)==NULL)
			{
				printf("Invalid\nRead \"help\" for more information");
				return;
			}
			j=2;


			DIR *dir=opendir(dest);
				while(j!=a)
				{
					char *src=malloc(sizeof(char)*512);
					j++;
					int k=j;
					while(str[j]!=' ')
						j++;

					if(str[3]=='-'&&k==3)
					continue;

					for(int i=k;i<j;i++)
					{
						src[i-k]=str[i];
					}

					src[j-k]='\0';

                    

                    char *file_path=malloc(sizeof(char)*512);
					FILE *source,*destination;
					source=fopen(src,"rb");
					if(dir==NULL)
					{ 
						FILE *fileptr;
						if(fopen(dest,"rb")!=NULL)
						{
							if(cpop=='n')
							continue;
							else if(cpop=='i')
							{
								char confirm='a';
								printf("Do you want to overwrite %s ? ",dest);
								scanf("%c",&confirm);
                                 while(getchar()!='\n');
								if(confirm!='y')
								continue;
							}
                            else if(cpop=='u')
                            {
                                struct stat s1,s2;
                                stat(src,&s1);
                                stat(dest,&s2);
                                if(s2.st_mtime>s1.st_mtime)
                                continue;
                            }
						}
						destination=fopen(dest,"wb");

                        if(destination==NULL)
                        {
                            printf("Invalid\nRead \"help\"for more information\n");
                            return;
                        }


                        if(str[0]=='m'&&str[1]=='v'&&cpop=='v')
                        {
                            printf("renamed ");
                        }
			    		if(cpop=='v')
                        {
					        printf("'%s' ->  ",src);
                        }
						if(cpop=='v')
						printf("'%s'\n",dest);

                        for(int i=0;i<=strlen(dest);i++)
                        {
                            file_path[i]=dest[i];
                        }
					}
					else
					{
						char *destdir=malloc(sizeof(char)*512);
						for(int i=0;i<strlen(dest);i++)
						{
							destdir[i]=dest[i];
						}
						destdir[strlen(dest)]='\0';
						strcat(destdir,"/");
						strcat(destdir,src);
						if(fopen(destdir,"r")!=NULL)
                        {
                            if(cpop=='n')
                            continue;
                            else if(cpop=='i')
                            {
                                char confirm;
								confirm='a';
                            	printf("Do you want to overwrite %s ? ",destdir);
                                scanf("%c",&confirm);
                                while(getchar()!='\n');
                                if(confirm!='y')
								{
                                    printf("fhis");
                                    continue;
								}
                            }
                            else if(cpop=='u')
                            {
                                struct stat s1,s2;
                                stat(src,&s1);
                                stat(destdir,&s2);
                                if(s2.st_mtime>s1.st_mtime)
                                continue;
                            }
                        }
						destination=fopen(destdir,"wb");

                        if(destination==NULL)
                        {
                            printf("Invalid\nRead \"help\"for more information\n");
                            return;
                        }

                        if(str[0]=='m'&&str[1]=='v'&&cpop=='v')
                        {
                            printf("renamed ");
                        }
			    		if(cpop=='v')
                        {
					        printf("'%s' ->  ",src);
                        }

						if(cpop=='v')
						{
							printf("'%s'\n",destdir);
						}

                        for(int i=0;i<=strlen(destdir);i++)
                        {
                            file_path[i]=destdir[i];
                        }
						free(destdir);
					}

                    

                    char buffer[4096];
                    size_t bytes_read;

                    struct stat s;
                    stat(src,&s);
                    mode_t newperm=s.st_mode;
                    

                    while ((bytes_read = fread(buffer, sizeof(char), 4096 , source)) > 0) 
                    {
                        fwrite(buffer, sizeof(char), bytes_read, destination);
                    }

                    
                    chmod(file_path,newperm);

					fclose(source);
					fclose(destination);
                    if(str[0]=='m'&&str[1]=='v')
                    {
                        remove(src);
                    }
					free(src);
				
				}
			closedir(dir);
			puts("");
}
void help(char*str)
{
    if(strcmp(str,"help")==0)
    {
        printf("\n--------------------------------------------------------------MyShell - HELP-------------------------------------------------------------\n\n");
        printf("--> Use arrow keys to navigate through history of used commands\n");
        printf("\n--> \"exit\" Command to exit MyShell");
        printf("\n--> Commands Implemented: ls, ps, cp, mv & grep");
        printf("\n--> \"ls --help\" To see more about ls");
        printf("\n--> \"ps --help\" To see more about ps");
        printf("\n--> \"cp --help\" To see more about cp");
        printf("\n--> \"mv --help\" To see more about mv");
        printf("\n--> \"grep --help\" To see more about grep");
        printf("\n--> Do not append '/' while changing directories\n");
        printf("\n--> \"cd\" goes into root\n" );
        printf("\n-->Syntax must be followed strictly\n");
        printf("\nAUTHOR\n        Written by Bonthu Mani Hemanth Reddy, CS22BTECH11013\n\n");
    }
    else if(strcmp(str,"ls --help")==0)
    {
        printf("\nNAME\n");
        printf("\tls - list directory contents\n");
        printf("SYNOPSIS\n");
        printf("\tls [OPTION]... [FILE]...\n");
        printf("DESCRIPTION\n");
        printf("\tList information about the files in the current directory sorted by alphabetically increasing order\n");
        printf("OPTIONS\n");
        printf("-a\n\tdo not ignore entries starting with .\n");
        printf("-l \n \t uses a long list format\n ");
        printf("-al\n\tuses a long list format including entries starting with .\n");
        printf("-1\n\tlists files in current directory with one file per line\n\n");
        printf("AUTHOR\n\tWritten by Bonthu Mani Hemanth Reddy, CS22BTECH11013\n\n");
    }
    else if(strcmp(str,"ps --help")==0)
    {
        printf("\nNAME\n\tps - prints a snapshot of the current processes\n\n");

        printf("SYNOPSIS\n\tps [OPTIONS]\n\n");

        printf("OPTIONS\n");
        printf("-e\n\t\tprints every process in the system\n");
        printf("-a\n\t\tprints all processes except session leaders ans processes not associated with a terminal\n");
        printf("-d\n\t\tprints all processes except session leaders\n");
        printf("-A\n\t\tidentical to -e\n");
        printf("-j\n\t\tprints in job format\n\n");

        printf("\nAUTHOR\n\tWritten by Bonthu Mani Hemanth Reddy, CS22BTECH11013\n\n");
    }
    else if(strcmp(str,"grep --help")==0)
    {
        printf("\nNAME\n\tprints lines that match the given patterns\n\n");

        printf("SYNOPSIS\n\t\tgrep [OPTION...] PATTERN [FILE...]\n\n");

        printf("DESCRIPTON\n\tIt searchs for the given PATTERN in the given FILES\n\tIt prints the line which has the pattern subject to the options\n\n");

        printf("OPTIONS\n");
        printf("-c\n\t\tprints the number of lines which contain the PATTERN\n");
        printf("-n\n\t\tprints the line number along with normal grep\n");
        printf("-i\n\t\tcase-insensitive\n");
        printf("-w\n\t\tsearchs for the exact word\n");

        printf("NOTE\n\tdot(.) is not considered as a special character and cannot be replaced by any other character\n\n");

        printf("AUTHOR\n\tWritten by Bonthu Mani Hemanth Reddy, CS22BTECH11013\n\n");
    }
    else if(strcmp(str,"cp --help")==0)
    {
        printf("\nNAME\n\tcp - copies files into other file or directory\n\n");

        printf("SYNOPSIS\n\tcp [OPTION]... SOURCE... DESTINATION\n\n");

        printf("DESCRIPTION\n\tcopies source file(s) into other file or into other directory\n\n");

        printf("OPTIONS\n");
        printf("-i\n\t\tasks before overriting if a file with name of source already exist\n");
        printf("-n\n\t\tdoesn't overwrite existing files\n");
        printf("-v\n\t\tprints the actions which has been done\n");
        printf("-ni\n\t\tidentical to -i\n");
        printf("-in\n\t\tidentical to -i\n");
        printf("-u\n\t\tdoesn't overwrite based on modification time\n\n");

        printf("NOTE\n\tCan't copy directories\n\tCan't copy source file(s) into multiple destinations\n\tCan't copy file from above directory to below directory\n\n");

        printf("AUTHOR\n\tWritten by Bonthu Mani Hemanth Reddy, CS22BTECH11013\n\n");
    }
    else if(strcmp(str,"mv --help")==0)
    {
        printf("\nNAME\n\tmv - moves files into other file or directory\n\n");

        printf("SYNOPSIS\n\tmv [OPTION]... SOURCE... DESTINATION\n\n");

        printf("DESCRIPTION\n\tmoves source file(s) into other file or into other directory\n\n");

        printf("OPTIONS\n");
        printf("-i\n\t\tasks before overriting if a file with name of source already exist\n");
        printf("-n\n\t\tdoesn't overwrite existing files\n");
        printf("-v\n\t\tprints the actions which has been done\n");
        printf("-ni\n\t\tidentical to -i\n");
        printf("-in\n\t\tidentical to -i\n");
        printf("-u\n\t\tdoesn't overwrite based on modification time\n\n");

        printf("NOTE\n\tCan't move directories\n\tCan't move source file(s) into multiple destinations\n\tCan't move file from above directory to below directory\n\n");

        printf("AUTHOR\n\tWritten by Bonthu Mani Hemanth Reddy, CS22BTECH11013\n\n");
    }
}
