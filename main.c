#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <math.h>
#include <string.h>

char* formatSize(double size)
{
    static char buf[32];
    const char* units[] = {"  B", " KB", " MB", " GB", " TB"};
    int i;
    for (i = 0; i < 5 && size >= 1024; i++)
    {
        size /= 1024;
    }
    sprintf(buf, "%.2f%s", size, units[i]);
    return buf;
}

void display(char* dir_path, int show_all)
{
    DIR* dir = opendir(dir_path);
    if (dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    double total_dir_size = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!show_all && entry->d_name[0] == '.')
        {
            continue;
        }
        char path[1024];
        sprintf(path, "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (lstat(path, &st) == -1) {
            perror("lstat");
            continue;
        }

        switch (st.st_mode & S_IFMT)
        {
            case S_IFREG:
                printf("├─[  File] ");
                total_dir_size += st.st_size;
                break;
            case S_IFDIR:
                printf("├─[Folder] ");
                break;
            default:
                printf("├─[ Other] ");
                break;
        }

        printf("%c%c%c%c%c%c%c%c%c  ", 
            (st.st_mode & S_IRUSR) ? 'r' : '-', 
            (st.st_mode & S_IWUSR) ? 'w' : '-', 
            (st.st_mode & S_IXUSR) ? 'x' : '-', 
            (st.st_mode & S_IRGRP) ? 'r' : '-', 
            (st.st_mode & S_IWGRP) ? 'w' : '-', 
            (st.st_mode & S_IXGRP) ? 'x' : '-', 
            (st.st_mode & S_IROTH) ? 'r' : '-', 
            (st.st_mode & S_IWOTH) ? 'w' : '-', 
            (st.st_mode & S_IXOTH) ? 'x' : '-');

        printf("%o  ", st.st_mode & 0777);

        struct passwd* pw = getpwuid(st.st_uid);
        printf("%6s  ", pw->pw_name);

        struct group* gr = getgrgid(st.st_gid);
        printf("%6s  ", gr->gr_name);
        
        if (S_ISDIR(st.st_mode))
            {
                double dir_size = 0;
                DIR* sub_dir = opendir(path);
                if (sub_dir == NULL)
                    {
                        perror("opendir");
                    }
                else
                    {
                        struct dirent* sub_entry;
                        while ((sub_entry = readdir(sub_dir)) != NULL)
                            {
                                if (!show_all && sub_entry->d_name[0] == '.')
                                    {
                                        continue;
                                    }
                                char sub_path[1024];
                                sprintf(sub_path, "%s/%s", path, sub_entry->d_name);
                                
                                struct stat sub_st;
                                if (lstat(sub_path, &sub_st) == -1) {
                                    perror("lstat");
                                    continue;
                                }
                                
                                if (S_ISREG(sub_st.st_mode))
                                    {
                                        dir_size += sub_st.st_size;
                                    }
                            }
                        closedir(sub_dir);
                    }
                total_dir_size += dir_size;
                printf("[%10s] ", formatSize(dir_size));
            }
        else
            {
                printf("[%10s] ", formatSize(st.st_size));
            }

        printf("%s  ", entry->d_name);

        

        printf("\n");
    }

    printf("│\n└────── [Total Dir Size: %s]\n", formatSize(total_dir_size));

    closedir(dir);
}

int main(int argc, char* argv[])
{
    int show_all = 0;
    char* dir_path = ".";

    if (argc == 2 && strcmp(argv[1], "-a") == 0)
    {
        show_all = 1;
    }
    else if (argc == 2)
    {
        dir_path = argv[1];
    }
    else if (argc == 3 && strcmp(argv[1], "-a") == 0)
    {
        show_all = 1;
        dir_path = argv[2];
    }
    printf("Here: \n│\n");

    display(dir_path, show_all);

    return 0;
}

