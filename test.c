#include <unistd.h>
#include <poll.h>
#include <stdio.h>

int main ()
{

int pipefd[2];
pipe(pipefd);

if (fork() == 0)
{
close(pipefd[0]);    // close reading end in the child

dup2(pipefd[1], 1);  // send stdout to the pipe
dup2(pipefd[1], 2);  // send stderr to the pipe

//close(pipefd[1]);    // this descriptor is no longer needed
execlp ("make", "make", (char *) NULL);
//execlp("./hellow", "hellow", "hello world", NULL);
}
else
{
    // parent
char buffer[1024];
//close(pipefd[1]);  // close the write end of the pipe in the parent

struct pollfd pll;
pll.fd = pipefd[0];
pll.events = POLLIN;
while (1)
//for (int i = 0; i < 10; ++i)
{
    int events = poll (&pll, 1, 10000);
if (events == 0) {printf ("no events\n"); continue;}
    int len = read (pipefd[0], buffer, 1024);

printf ("%d events, recieved(%d): %.*s\n", events, len, len, buffer);
}

//while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
//{
//}

    
    
}
}
