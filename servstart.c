
#include <arpa/inet.h>          /* inet_ntoa */
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LISTENQ  1024  /* second argument to listen() */
#define MAXLINE 1024   /* max length of a line */
#define RIO_BUFSIZE 1024

typedef struct {
    int rio_fd;                 /* descriptor for this buf */
    int rio_cnt;                /* unread byte in this buf */
    char *rio_bufptr;           /* next unread byte in this buf */
    char rio_buf[RIO_BUFSIZE];  /* internal buffer */
} rio_t;

/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;

typedef struct {
    char filename[512];
    off_t offset;      //off_t for file sizes        /* for support Range */
    size_t end;        // size_t for objecgt size
} http_request;

typedef struct {
    const char *extension;
    const char *mime_type;
} mime_map;

mime_map meme_types [] = {
    {".css", "text/css"},
    {".gif", "image/gif"},
    {".htm", "text/html"},
    {".html", "text/html"},
    {".jpeg", "image/jpeg"},
    {".jpg", "image/jpeg"},
    {".ico", "image/x-icon"},
    {".js", "application/javascript"},
    {".pdf", "application/pdf"},
    {".mp4", "video/mp4"},
    {".png", "image/png"},
    {".svg", "image/svg+xml"},
    {".xml", "text/xml"},
    {NULL, NULL},
};

char *default_mime_type = "text/plain";


int open_listenfd(int port){
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;

    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;

    // 6 is TCP's protocol number
    // enable this, much faster : 4000 req/s -> 17000 req/s
    if (setsockopt(listenfd, 6, TCP_CORK,
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;

    /* Listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
}

int instr(int n1, int n2, char *str1, char *strmain)
{
	int del = 0;

int stlen = strlen(str1);
//printf ("%i", stlen);
for (int i = n1; i < n2; i++)
	{
		if (str1[del] == strmain[i])
		{
			del++;
			if (str1[del] == 0) return (i + 1 - stlen);
		}

	}
return -1;

}

void midstr(int n1, int n2, char *base, char *source)

{
int lenstr = n2 - n1;
for (int i = 0;i < lenstr; i++)
	base[i] = source[n1 + i];
	base[lenstr] = 0;
}


int main(int argc, char** argv)
{
//char hello[] = "HTTP/1.1 200 OK\r\n Content-Type: text/html\r\n\r\n<html><head></head><body>Hello, <br> This is a test</body></html>\r\n\r\n";
char buffer[1000];
char root_file[100];
char chr[100];


long fsize = 0;
int end_line = 0;
int in_search = 0;
int saveget = 0;

FILE *f = fopen("config.txt", "rb");

fseek(f, 0, SEEK_END);
fsize = ftell(f);
fseek(f, 0, SEEK_SET);

fread(buffer, fsize, 1, f);
buffer[fsize] = 0;
fclose(f);

//printf ("%s", buffer);

//get end of first line, for rootfile/=
end_line = instr(0, 1000, "\n", buffer);

//seperate rootfile with midstr
midstr(10, end_line, root_file, buffer);

//search and isolate saveget=0 or 1 into chr string
in_search = instr(0, 1000, "saveget=", buffer);
midstr(in_search + 8, in_search + 9, chr, buffer);

if (chr[0] == '1')
	saveget = 1;

// open socket for lsitening

    struct sockaddr_in clientaddr; //
    int default_port = 9999, listenfd, connfd; //

    
    socklen_t clientlen = sizeof clientaddr; // socklen_t = int

	listenfd = open_listenfd(default_port); // openlistenfd added above

    if (listenfd > 0) {
        printf("listen on port %d, fd is %d\n", default_port, listenfd);
    } else {
        perror("ERROR");
        exit(listenfd);
    }
    // Ignore SIGPIPE signal, so if browser cancels the request, it
    // won't kill the whole process.
    signal(SIGPIPE, SIG_IGN);


    while(1){
        connfd = accept(listenfd, (SA *)&clientaddr, &clientlen);




// use connfd directly    

    int n = read(connfd, buffer,512);
     if (n < 0) perror("ERROR reading from socket");
 
if (saveget) {
char time_stamp[100];
    time_t current_time;
    char* c_time_string;

	
    /* Obtain current time. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1))
    {
        (void) fprintf(stderr, "Failure to obtain the current time.\n");
        exit(EXIT_FAILURE);
    }

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    if (c_time_string == NULL)
    {
        (void) fprintf(stderr, "Failure to convert the current time.\n");
        exit(EXIT_FAILURE);
    }

    /* Print to stdout. ctime() has already added a terminating newline character. */
    //(void) printf("%s", c_time_string);

f = fopen(c_time_string, "w");


//fwrite(buffer ,512, 1, f);
    fprintf(f, "%s", buffer);
//printf("%s", buffer);
fclose(f);

}




f = fopen(root_file, "rb");

fseek(f, 0, SEEK_END);
fsize = ftell(f);
fseek(f, 0, SEEK_SET);

fread(buffer, fsize, 1, f);
buffer[fsize] = 0;
fclose(f);




write(connfd,buffer,fsize);

    close(connfd);




    }

    return 0;



}
