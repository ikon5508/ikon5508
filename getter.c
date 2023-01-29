#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

struct buffer_data
{
char *p;
int len;
int max;
};
typedef struct buffer_data buffer_t;

void error(char *msg)
{
    perror(msg);
    exit(0);
}




int main(int argc, char *argv[])
{
const int buff_sz = 1000;
char cbuffer[buff_sz];
buffer_t buffer;
buffer.p = cbuffer;
buffer.max = buff_sz;

if (argc == 2)
{
printf("connecting localhost, request: %s\n", argv[1]);
}else{
printf("undefined\n");
return 0;
}

    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = 9999;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("localhost"); // get hostbyname
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

 bzero((char *) &serv_addr, sizeof(serv_addr));
 serv_addr.sin_family = AF_INET;

 bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    
 serv_addr.sin_port = htons(portno);


    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


buffer.len = sprintf(buffer.p, "GET %s HTTP/1.1 OK\n", argv[1]);

    n = write(sockfd, buffer.p, buffer.len);
    if (n < 0) 
         error("ERROR writing to socket");

buffer.len = read(sockfd,buffer.p, buffer.max);
if (buffer.len < 0) 
  error("ERROR reading from socket");

buffer.p[buffer.len] = 0;

//printf("%s\n", buffer.p);
//get content len

char *p1 = strcasestr (buffer.p, "Content-Length: ");
if (p1 != 0)
{
//printf ("it is found:!\n");
char strnum [100];
memset (strnum, 0, 100);

int d1 = p1 - buffer.p + 16;

char *p2 = memchr (buffer.p + d1, 10, buffer.len - d1);
 int d2 = p2 - buffer.p;
int strnumlen = d2 - d1;
memcpy (strnum, buffer.p + d1, strnumlen);

//printf ("d1: %d, d2: %d\n", d1, d2);

printf ("the length is: [%s]\n", strnum);
int content_len = atoi (strnum);
int read_prog = 0;

while (read_prog < content_len)
{
int rtn = read (sockfd, buffer.p, buffer.max);
if (rtn <= 0) {printf ("0 rtn\n"); break;}

read_prog += rtn;
printf ("%d / %d\n", read_prog, content_len);
} // read loop


} // if content len




//    return 0;
//int fd;
//fd = open ("output.txt", O_CREAT | O_WRONLY);
//write (fd, buffer, n);

} // main
