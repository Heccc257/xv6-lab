#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int p[2];
    if (pipe(p) < 0) {
        printf("fail to create pipe\n");
        exit(0);
    }
    char buff[2];
    if (fork() == 0) {
        read(p[0], buff, 1);
        printf("%d: received ping\n", getpid());

        buff[0] = 1;
        buff[1] = 0;
        write(p[1], buff, 1);
    } else {
        buff[0] = 1;
        buff[1] = 0;
        write(p[1], buff, 1);

        read(p[0], buff, 1);
        printf("%d: received pong\n", getpid());
    }
    exit(0);
}