
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX 300
struct path_stat {
    struct stat file_stat;
    char m_path[MAX];
};


char get_filetype(int m)
{
    char c;
    if (S_ISREG(m))
        c = '-';
    else if (S_ISCHR(m))
        c = 'c';
    else if (S_ISDIR(m))
        c = 'd';
    else if (S_ISBLK(m))
        c = 'b';
    else
        c = '?';
    return c;
}
void user_permission (int mode, char* per)
{
    per[0] = mode&S_IRUSR? 'r':'-';
    per[1] = mode&S_IWUSR? 'w':'-';
    per[2] = mode&S_IXUSR? 'x':'-';
}

void group_permission (int mode, char* per)
{
    per[3] = mode&S_IRGRP? 'r':'-';
    per[4] = mode&S_IRGRP? 'w':'-';
    per[5] = mode&S_IXGRP? 'x':'-';
}

void other_permission (int mode, char* per)
{
    per[6] = mode&S_IROTH? 'r':'-';
    per[7] = mode&S_IWOTH? 'w':'-';
    per[8] = mode&S_IXOTH? 'x':'-';
    
}

char* permission (int mode, char* per)
{
    user_permission(mode,per);
    group_permission(mode,per);
    other_permission(mode,per);
    per[9] = '\0';
    return per;
}

char* get_UserName(uid_t uid)
{
    struct passwd* pw = getpwuid(uid);
    return pw->pw_name ? pw->pw_name : "";
}

char* get_GroupName(gid_t gid)
{
    struct group* grp = getgrgid(gid);
    return grp->gr_name ? grp->gr_name : "";
}

void last_modification_and_name(char* tmbuff, struct stat status,char* d_name)
{
    struct tm *tm;
    tm = localtime(&status.st_mtime);
    strftime(tmbuff, sizeof(tmbuff), "%b %d", localtime(&status.st_mtime));
    printf(" %s %d:%d %s\n",tmbuff, tm->tm_hour, tm->tm_min, d_name);
}



void file_info (char* d_name, struct stat status)
{
    char per[11], tmbuff[100];
    char type = get_filetype(status.st_mode);
    printf("%c%s", type, permission(status.st_mode, per ));
    printf(" %d %s %s  %5jd",status.st_nlink, get_UserName(status.st_uid), get_GroupName(status.st_gid),(intmax_t)status.st_size);
    last_modification_and_name(tmbuff,status, d_name);
}


//line exception 2 --- line = 6
void is_directory (char* file_path,int aflg, int Aflg, int Lflg, char* file_list[], int* file_idx)
{
    DIR* dirp;
    if(!(dirp= opendir(file_path)))
    {
        perror(file_path);
        return;
    }
    struct dirent* direntp;
    int index =(*file_idx);
    while((direntp = readdir(dirp))){
        //printf("%d \n", index);
        if (aflg == 1)
        {
            file_list[index]= direntp->d_name;
            //strcpy(file_list[index], direntp->d_name );
            index++;
        }
        else {//becasue weather we have A or not we shouldnt print . & .. anyway
            if(strcmp(direntp->d_name, "..") != 0 && strcmp(direntp->d_name, ".") != 0 && strcmp(direntp->d_name, "") != 0){
                
                file_list[index]= direntp->d_name;
                //strcpy(file_list[index], direntp->d_name );
                index++;
            }
        }
    }
    (*file_idx) = index;
    closedir(dirp);
}



void process_options(int argc, char * argv[], int* aflg, int* Aflg, int* Lflg) {
    int c;
    
    while ((c = getopt(argc, argv, "aAL")) != EOF) {
        switch (c) {
            case 'a':
                (*aflg)++;
                break;
            case 'A':
                (*Aflg)++;
                break;
            case 'L':
                (*Lflg)++;
                break;
            case '?':
                exit(1);
        }
    }
}
void process_file_dir_from_argv(char* path, int Lflg, char* file_list[], int* file_idx, char* dir_list[], int* dir_idx)
{
    struct stat s;
    
    if(Lflg ==1)// L : Symbolic_file
    {
        if(stat(path, &s) == -1)
        {
            perror(path);
            return;
        }
        
    }
    else// no L : Symbolic_file -> hard_file
    {
        if(lstat(path, &s) == -1)
        {
            perror(path);
            return;
        }
    }
    
    if(S_ISREG(s.st_mode))
        file_list[(*file_idx)++] = path;
    
    if(S_ISDIR(s.st_mode))
        
        dir_list[(*dir_idx)++] = path;
    
}

static int intcompare(const void *p1, const void *p2)
{
    
    struct path_stat  file_1 = *((struct path_stat *)p1);
    struct path_stat  file_2 = *((struct path_stat *)p2);
    long size1=0, size2=0;
    size1 =(long)file_1.file_stat.st_size;
    size2 =(long)file_2.file_stat.st_size;
    
    if (size1 >size2)
        return -1;
    else if ( size1 <size2 )
        return 1;
    else
        return strcmp(file_1.m_path, file_2.m_path);
    
}

//add them to the final list then sort
void process_file_list (char* dir_path, char* file_list[], int file_idx,int Lflg)
{
    struct path_stat ps[file_idx];
    int ps_idx=0;
    for (int i=0; i < file_idx; ++i)
    {
        char file_path[200];
        if(strcmp(dir_path, ""))
        {
            if(dir_path[strlen(dir_path)-1] != '/')
                sprintf(file_path, "%s/%s", dir_path, file_list[i]);
            else
                sprintf(file_path, "%s%s", dir_path, file_list[i]);
        }
        else
            sprintf(file_path, "%s%s", dir_path, file_list[i]);
        
        
        struct stat s;
        if(Lflg == 0)//no L option, still display symbolic's stat -->lstat
        {
            //check symbolic link's stat -->lstat
            //but the name should be symbolic link --> actul file
            if(lstat(file_path, &s) == -1)
                perror(file_path);
            else
            {
                //add ---> hard
            }
            
        }
        else// there is L option, display original file's stat --> stat
        {
            if(stat(file_path, &s) == -1)
                perror(file_path);
            
        }
        
        //update stuct, with apporate name and stat
        strcpy(ps[ps_idx].m_path,file_list[i]);
        ps[ps_idx].file_stat = s;//save stat
        ps_idx++;
    }
    //sort all the files, based on file size
    //note qsort is assending order
    qsort(ps, ps_idx, sizeof(struct path_stat), intcompare);
    
    //show the output for each file
    
    for (int k = 0; k < ps_idx; k++) {
        file_info(ps[k].m_path, ps[k].file_stat);
    }
}
void process_dir_list(char* dir_list[], int dir_idx, int aflg, int Aflg, int Lflg)
{
    char* dir_file_list[1024];
    for(int i=0; i <dir_idx; ++i)
    {
        printf("\n %s \n", dir_list[i]);
        
        int file_count =0;
        //find all the files in current directory
        is_directory(dir_list[i],aflg, Aflg, Lflg, dir_file_list, &file_count);
        //then start to process all the files
        process_file_list(dir_list[i], dir_file_list, file_count, Lflg);
    }
}

int main(int argc, char* argv[])
{
    int aflg = 0, Aflg = 0, Lflg = 0;
    //extern int optind;
    char* file_list[200];
    char* dir_list[200];
    int file_idx=0, dir_idx =0;
    
    
    process_options(argc, argv, &aflg, &Aflg, &Lflg );
    
    // need to find all the files and directoies first.
    if(optind >= argc)
        dir_list[dir_idx++] = ".";//no file provide -> current directory
    else
        for ( ; optind < argc; optind++)
            process_file_dir_from_argv(argv[optind], Lflg, file_list, &file_idx, dir_list, &dir_idx);
    
    //if (access(argv[optind], R_OK))
    //start to process all the files, and sort then print.
    
    if ( file_idx >0)
    {
        process_file_list("", file_list, file_idx, Lflg);
        
    }
    if (dir_idx >0)
    {
        process_dir_list(dir_list,dir_idx,aflg, Aflg, Lflg);
    }
    
    return 0;
}//Readlink

/*
 ////////////////////////////////////////////////////////////////////////////////
 ///// about printing the file details is heavily borrowed from
 ////  my old assignment when I was taking ics53
 ////////////////////////////////////////////////////////////////////////////////
 NOTE:
 1. A cannot over-write a, so as long as there is option a, has to
 list . and ..
 2. no a, no ignore files..., unless user passed in,
 so even with a, still need to print that file.
 
 3. when processing the user input, always process the files first
 then process the directories, with printing the directory name first
 
 4. in terms of file name display:
 for the files user provide, display the same as user provided(include path)
 for the files inside of directory user provide, display just the name
 5. need to fix if enter a directory , need to check if it has / in the end.
 */

