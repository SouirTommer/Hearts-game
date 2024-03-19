// lab 6C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
  int	fd[2];
  char buf[80];
  int	i, n, childid;

  if (pipe(fd) < 0) {
     printf("Pipe creation error\n");
     exit(1);
  }
  childid = fork();
  if (childid < 0) {
     printf("Fork failed\n");
     exit(1);
  } else if (childid == 0) { // child
     close(fd[1]); // close child out
     while ((n = read(fd[0],buf,80)) > 0) { // read from pipe
           buf[n] = 0;
           printf("<child> message [%s] of size %d bytes received\n",buf,n);
     }
     close(fd[0]);
     printf("<child> I have completed!\n");
  } else { // parent
     close(fd[0]); // close parent in
     while (1) {
           printf("<parent> please enter a message\n");
           n = read(STDIN_FILENO,buf,80); // read a line
           if (n <= 0) break; // EOF or error
           buf[--n] = 0;
           printf("<parent> message [%s] is of length %d\n",buf,n);
           printf("<parent> sending message [%s] to child\n",buf);
           write(fd[1],buf,n); // send the string
     }
     close(fd[1]);
     wait(NULL);
     printf("<parent> I have completed!\n");
  }
  exit(0);
}
