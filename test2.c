#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int i;
    pid_t pid;

    for (i = 0; i < 4; i++) {
        pid = fork();

        if (pid < 0) {
            printf("Fork failed.\n");
            return 1;
        } else if (pid == 0) {
            printf("Child process %d, PID: %d\n", i+1, getpid());
            return 0;
        }
    }

    // Parent process waits for all child processes to complete
    // for (i = 0; i < 4; i++) {
    //     wait(NULL);
    // }

    return 0;
}