// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#define PORT 9999

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>

#ifdef __linux__
#include <linux/sockios.h>
#endif
#include <fcntl.h>
#include <signal.h>

const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

const char *hthead = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";

const char *closehead = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\nContent-Length: ";

int inchar (char *major, int minor, int start)
{
int i;
for (i = start; i < 10000; ++i)
{
if (major[i] == minor)
	return i;
}
return -1;

} // end inchar

void midstr(char *major, char *minor, int start, int end)
{
int count = 0;
while (start < end)
{
minor[count] = major[start];
++start;
++count;
}
minor[count] = 0;

} // end midstr


void depleteSendBuffer(int fd) 
{
#ifdef __linux__
	int lastOutstanding=-1;
	int outstanding;
	for(;;) {
		ioctl(fd, SIOCOUTQ, &outstanding);
		if(outstanding != lastOutstanding) 
			printf("Outstanding: %d\n", outstanding);
		lastOutstanding = outstanding;
		if(!outstanding)
			break;
		usleep(1000);
	}
#endif
}
void stradd (char *main, char *add, int *c)
{
int sc = 0;

while (1)
{
main[*c] = add[sc];
++*c;
++sc;
if (add[sc] == 0)
	break;

} //end while
//--*c;
}// ene stradd

int main(int argc, char const *argv[])
{
	int optval = 1;
 int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
int hellow = 0;
int n, res;
DIR *dp;
struct dirent *ep;
//char menu[10000];
//char editor[10000];
char buffer[10000] = {0};
char path[100];
char uri[100];
char method;
int d1, d2;
char mode;
char buffer1[10000];
char buffer2[10000];
int body_sz, message_sz;
int count = 0;

char editor1[10000];
char editor2[10000];
        struct linger ling;
        ling.l_onoff=1;
        ling.l_linger=1;




//int body-count = 0;
int remainder = 0;

 // main execution body
n = open ("editor.html", O_RDONLY);
valread = read (n, buffer, 10000);
close (n); n = 0;
int instr (char *main, char *minor, int s, int nd)
{
int sc = 0;
int lc;
for (lc = s; lc < nd; ++lc)
{
if (main[lc] == minor[sc])
{
//printf("main: %c, minor: %c\n", main[lc], minor[sc]);
 
++sc;
if (minor[sc] == 0)
return lc;

} // end if
} // end for
} // end instr
d2 = instr(buffer, "DELIMETER", 1, 10000);
midstr (buffer, editor1, 0, d2 - 8);
midstr (buffer, editor2, d2 + 1, valread);
//sprintf(buffer1, "%s\n mememe \n %s", editor1, editor2);

memset (buffer, 0, valread);

if (argc == 2)
	if (!strcmp("hellow", argv[1]))
		hellow = 1;
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;


setsockopt(server_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

while(1) // server loop
{
mode = 'z';
count = 0;
printf("\n+++++++ Waiting for new connection ++++++++\n\n");

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
// if argv hellow, run hellow diag test
if (hellow)
{write(new_socket , hello , strlen(hello));continue;}

memset(buffer, 0, 10000);
valread = read( new_socket , buffer, 10000);

method = buffer[0];
d1 = inchar (buffer, 32, 0);
d2 = inchar (buffer, 32, d1 + 1);
midstr(buffer, uri, d1 + 1, d2);

printf("%s requested\n", uri);

if (uri[0] == '/' && uri[1] == 0)
{
mode = 'r';
getcwd(path, 100);
}else{

midstr(uri, path, 3, strlen(uri));

if (uri[1] == 'd')
	mode = 'd';

if (uri[1] == 'e')
	mode = 'e';

if (uri[1] == 't')
	mode = 't';

} // end if root request
// t e d _
// text, edit, dir request

memset (buffer, 0, 10000);

if (mode == 'd' || mode == 'r')
{

printf ("the mode is: %c\n", mode);
printf ("dir request: %s\n", path);
dp = opendir (path);

if (dp == NULL)
{printf("dir error");write (new_socket, hello, strlen(hello));continue;}

stradd (buffer, "<!DOCTYPE html>\n<html>\n<head>\n<style>\n", &count);
stradd (buffer, "body\n{\ntext-align:center;\nbackground-color:aqua;\nfont-size:52px;\n}\n", &count);
stradd (buffer, "a:link\n{\ncolor:midnightblue;\ntext-decoration:none;\n}\n", &count);
stradd (buffer, "</style>\n</head>\n<body>\n", &count);
if (mode == 'r')
	{path[0] = '/'; path[1] = 0;}
while (ep = readdir (dp))
{
//if (ep->d_type == 4)
// 4 is dir 8 is file
//ep->d_name
if (ep->d_name[0] == '.')
	continue;

if (ep->d_type == 4)
{
stradd(buffer, "\n<br>\n<a href=\"d_", &count);
//stradd(buffer, path, &count);
stradd(buffer, ep->d_name, &count);
stradd(buffer, "\">", &count);
stradd(buffer, ep->d_name, &count);
stradd(buffer, "</a>", &count);
} // if directory

if (ep->d_type == 8)
{

stradd(buffer, "\n<br>\n<a href=\"e_", &count);
//stradd(buffer, path, &count);
stradd(buffer, ep->d_name, &count);
stradd(buffer, "\">", &count);
stradd(buffer, ep->d_name, &count);
stradd(buffer, "</a>", &count);
} // if is file

}// while read dir
(void) closedir (dp);

/*
stradd (buffer, "<script>\nfunction clickme(me)\n{\n", &count);
stradd (buffer, "window.alert(me);\n}\n</script>\n", &count);
*/

stradd(buffer, "\n</body>\n</html>", &count);

//body_sz = strlen(buffer);
body_sz = count;
sprintf(buffer1, "%s%d\n\n%s", closehead, body_sz, buffer);

message_sz = strlen(buffer1);

valread = write (new_socket, buffer1, message_sz);
printf("%d  bytes written of: %d\n", valread, message_sz);
//printf("------------------Hello message sent-------------------");

} // if mode = d

if (mode == 'e')
{
printf ("the mode is: %c\n", mode);
n = open (path, O_RDONLY);
if (n < 0)
{
printf("error opening file\n");
mode = 'w';
}
valread = read (n, buffer, 10000);
close (n);

stradd (buffer1, editor1, &count);
stradd (buffer1,  buffer, &count);
stradd (buffer1, editor2, &count);

sprintf(buffer2, "%s%d\n\n%s", closehead, count, buffer1);
write (new_socket, buffer2, strlen(buffer2));


} // if mode = e
if (mode == 'w')
	write (new_socket, hello, strlen(hello));

//if (mode == 'z')
//	write (new_socket, "HTTP/1.1 410 GONE\n", strlen("HTTP/1.1 410 GONE\n"));

if (mode == 'z')
	write (new_socket, "HTTP/1.1 500 ERROR\n", strlen("HTTP/1.1 500 ERROR\n"));

shutdown (new_socket,  SHUT_WR);
	depleteSendBuffer(new_socket);
	
	// now wait for the remote to close its socket
	
	for(;;) {
		res=read(new_socket, buffer, sizeof(buffer));
		if(res < 0) {
			perror("reading");
			exit(1);
		}
		if(!res) {
			printf("Correct EOF\n");
			break;
		}
	}
	close(new_socket);
} // server loop 
return 0;
} 

