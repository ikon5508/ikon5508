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

void error(char *msg)
{
    perror(msg);
    exit(0);
}




int main(int argc, char *argv[])
{

    char buffer[10000];


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


sprintf(buffer, "GET %s HTTP/1.1 OK\n", argv[1]);

    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");

    bzero(buffer,1000);
    n = read(sockfd,buffer,10000);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
//    return 0;
int fd;
fd = open ("output.txt", O_CREAT | O_WRONLY);
write (fd, buffer, n);

}
