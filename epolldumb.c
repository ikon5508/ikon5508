
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
//#define PORT 9999;

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <dirent.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#ifdef __linux__
#include <linux/sockios.h>
#endif
#include <fcntl.h>
#include <signal.h>
#include <sys/epoll.h>


#define MAX_EVENTS 64
const char *closehead = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\nContent-Length: ";

//const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 0000012\n\nHello world!";

const char *hthead = "HTTP/1.1 200 OK\n";

const char *contjava = "Content-Type: text/javascript\n";
const char *conthtml = "Content-Type: text/html; charset=utf-8\n";
const char *conttxt = "Content-Type: text/plain\n";
//const char *contjava = "Content-Type: text/javascript\n";


const char *connclose = "Connection: close\n";
const char *contlen = "Content-Length: ";

int logging(const char *msg, int level, int num)
{
if (level == 1)
	printf ("%s\n", msg);

if (level == 2)
	printf ("%s%d\n", msg, num);
	
if (level == 100)
{
printf ("%s\n", msg);

exit(0);
}// if
} // logging


int prepsocket (int PORT)
{
int result = 0;
int optval = 1;
struct linger ling;
ling.l_onoff=1;
ling.l_linger=4;

int server_fd = socket(AF_INET, SOCK_STREAM, 0);

struct sockaddr_in address;
int addrlen = sizeof(address);

memset(&address, 0, sizeof (address));

address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons( PORT );
//memset(address.sin_zero, 0, sizeof address.sin_zero);

result = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval , sizeof(int));
if (result == -1)
	logging("error, reuse addr", 100, 0);

//result = setsockopt(server_fd, SOL_SOCKET, TCP_CORK,&optval , sizeof(int));
//if (result == -1)
//	logging("error, TCP-CORK", 100, 0);


result = setsockopt(server_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
if (result == -1)
	logging("error, linger", 100, 0);

result = bind(server_fd, (struct sockaddr *)&address,(socklen_t) sizeof(address));
if (result == -1)
	logging("error, bind", 100, 0);

result = listen(server_fd, 10);
if (result == -1)
	logging("error, reuse listen", 100, 0);

return (server_fd);
}// end prep socket


void send_txt(int conn, char *txt)
{
char outbuff[500];
int a;
a = sprintf (outbuff, "%s%s%s%s%d\n\n%s", hthead, conttxt, connclose, contlen, strlen(txt), txt);

write (conn, outbuff, a);


} // send_txt

int main ()
{


struct epoll_event ev, events[MAX_EVENTS];
int listen_sock, n, conn_sock, nfds, epollfd;

struct sockaddr_in addr;
socklen_t addrlen = sizeof(addr);

listen_sock = prepsocket (9999);

epollfd = epoll_create1(0);
if (epollfd == -1) {
perror("epoll_create1");
exit(EXIT_FAILURE);
}

ev.events = EPOLLIN;
ev.data.fd = listen_sock;
if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
perror("epoll_ctl: listen_sock");
exit(EXIT_FAILURE);
}

for (;;) {
nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
if (nfds == -1) {
perror("epoll_wait");
exit(EXIT_FAILURE);
}

for (n = 0; n < nfds; ++n) {
if (events[n].data.fd == listen_sock) {
conn_sock = accept(listen_sock,
(struct sockaddr *) &addr, &addrlen);
if (conn_sock == -1) {
perror("accept");
exit(EXIT_FAILURE);
}
fcntl(conn_sock, F_SETFL, fcntl(conn_sock, F_GETFL, 0) | O_NONBLOCK);
ev.events = EPOLLIN | EPOLLET;
ev.data.fd = conn_sock;
if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
perror("epoll_ctl: conn_sock");
exit(EXIT_FAILURE);
}
logging ("accepted fd: ", 2, conn_sock);
continue;                   
} else {
logging ("connection fd: ", 2, conn_sock);
send_txt(events[n].data.fd, "the test worked");
}
}
}
}
