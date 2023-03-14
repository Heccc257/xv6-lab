#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31};

int main() {
    // 1表示主父进程,2-6表示子进程
    // 在6进程输出

    int factor = 1;
    // 创建1-5管道
    int p[6][2];
    for (int i=1; i<=5; i++)    
        pipe(p[i]);

    for (int i=2; i<=6; i++) { // sqrt 35
        if (fork()==0) {
            factor = i;
            break;
        }
    }

    // 关闭不需要管道

    for (int i=1; i<=5; i++) {
        if (i != factor) close(p[i][1]);
        if (i != factor-1) close(p[i][0]);
    }
    
    if (factor == 1) {
        for (int i=0; i<11; i++) {
            write(p[factor][1], (void*)&primes[i], sizeof(int));
        }
        close(p[factor][1]);
    } else {
        while(1) {
            int recv;
            if (read(p[factor-1][0], (void*)&recv, sizeof(int)) == 0) break; // 已经没有能读的了
            if (recv<=factor || recv%factor) { // 输出或者传入管道
                if (factor == 6) {
                    printf("prime %d\n", recv);
                } else {
                    write(p[factor][1], (void*)&recv, sizeof(int));
                }
            }
        }
        close(p[factor-1][0]);
        if (factor < 6) close(p[factor][1]);
    }
    
    if (factor == 1) {
        int status;
        while (wait(&status) > 0);
    }
    exit(0);
}

/*
        
gcc user/primes.c user/user.h kernel/types.h kernel/stat.h -o primes
*/