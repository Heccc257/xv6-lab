#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

char*
lst_char(char *path) {
    char *p;
    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}
void Find(char *dir, char *file_name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    fd = open(dir, 0);
    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", dir);
        close(fd);
        return;
    }
    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
        break;
    case T_DIR:
        if (strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: dir too long\n");
            break;
        }
        strcpy(buf, dir);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0) {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            if (strcmp(".", lst_char(buf)) == 0) continue ;
            if (strcmp("..", lst_char(buf)) == 0) continue ;
            if (st.type == 1) { // dir
                char new_dir[512];
                memcpy(new_dir, buf, sizeof(buf));
                // printf("go to %s\n", new_dir);
                Find(new_dir, file_name);
            } else if(strcmp(lst_char(buf), file_name) == 0) {
                
                printf("%s\n", buf);
                // printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) exit(0);

    char *dir = argv[1];
    char *file_name = argv[2];
    
    int fd;
    if ((fd = open(dir, 0)) < 0) {
        fprintf(2, "find: %s not a dir\n", dir);
        return 0;
    }
    close(fd);

    Find(dir, file_name);
    return 0;
}
/*
gcc user/find.c kenel/types.h -I ./ -w -o find.o
./find.o . b
*/