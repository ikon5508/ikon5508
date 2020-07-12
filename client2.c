#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
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



const int max_buf = 1000;


int main(int argc, char *argv[])
{

    char buffer[max_buf];



FILE *f = fopen("get.txt", "rb");
8
fseek(f, 0, SEEK_END);
long fsize = ftell(f);
fseek(f, 0, SEEK_SET);

fread(buffer, fsize, 1, f);
buffer[fsize] = 0;
fclose(f);

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
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);


    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

//  printf("Please enter the message: ");
//bzero(buffer,256);
//  fgets(buffer,255,stdin);

    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");

    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    return 0;
}
