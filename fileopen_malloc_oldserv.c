#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>


int main()
{
char ht410[] = "HTTP/1.1 410 Gone";

char htbod[] = "HTTP/1.1 200 OK\r\n Content-Type: text/html\r\n\r\n";

	char htbod2[] = "HTTP/1.1 200 OK\r\n Content-Type: text/html\r\n\r\n<html><head></head><body>Hello\n";

FILE *f = fopen("htest.txt", "rb");
fseek(f, 0, SEEK_END);
long fsize = ftell(f);
fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

char *string = malloc(fsize + 1);
fread(string, fsize, 1, f);
fclose(f);

string[fsize] = 0;

printf("%s", string);


     int sockfd, newsockfd, portno, clilen;
     char buffer[512];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(9999);

	bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));


	listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

//bzero(buffer,256);
     n = read(newsockfd,buffer,512);
     //if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);

	//char *ostring = malloc(fsize + 1 + sizeof(htbod));

//sprintf(ostring, "%s%s", htbod, string);
//     n = write(newsockfd,ht410,sizeof ht410);
//     n = write(newsockfd,ostring,sizeof ostring);
     n = write(newsockfd,htbod2,sizeof htbod2);
//     n = write(newsockfd,string,sizeof string);
     //if (n < 0) error("ERROR writing to socket");

//printf("%s", ostring);

close (sockfd);
close (newsockfd);
free (string);
     return 0;
}
