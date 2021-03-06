
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
void stradd (char *main, const char *add, size_t *c)
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
}


int getops ()
{

}
int getcork (int s)
{
//int optval;
//getsockopt(s, SOL_SOCKET, TCP_CORK,&optval , sizeof(int));

//return optval;

}

int corkon (int s)
{
int result;
int optval = 1;

result = setsockopt(s, SOL_SOCKET, TCP_CORK,&optval , sizeof(int));

if (result == 0)
{
return (1);
}else{
logging ("error, cork on", 50, 0);
}


}


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

int corkoff (int s) 
{

int result;
int optval = 0;

result = setsockopt(s, SOL_SOCKET, TCP_CORK,&optval , sizeof(int));

if (!result)
{
return (1);
}else{
logging ("error, cork off", 100, 0);
}

}

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
void geturi (char *buf, char *rtn)
{
//printf("%s\n", buf);
int d1, d2, cnt;
cnt = 0;
for (d1 = 0;d1 < 100; ++d1)
if (buf[d1] == 32)
break;
++d1;
for (d2 = d1; d2 < 200;++d2)
if (buf[d2] == 32)
{break;}

while (d1 != d2)
{
rtn[cnt] =  buf[d1];
++cnt; ++d1;
} // while
//++cnt;
rtn[cnt] = 0;

}// end get uri

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
return -1;
} // end instr
int main (int argc, char **argv)
{
struct sockaddr_in address;
int editmode = 1;
int localfd;
int editorfd;
int connfd;
int intfiletype;
int sv_fd = prepsocket(9999);
socklen_t addrlen = sizeof(address);
size_t max_read = 10000;
size_t max_buff = 100000;
size_t res;
size_t read_in = max_read;
size_t write_out;
char  buffer_in[max_read];
char buffer_out1[max_buff];
char buffer_out2[max_buff];
char buffer_file[max_buff];
char dep_buffer[500];
char method;
char uri[200];
char mime_ext[10];
const char *mime_txt;
char path[200];
char mode;
char *editor = "aceeditor.htm";
size_t count;
DIR *dp;
struct dirent *ep;
struct stat finfo;
size_t fsize;
int a, b, c, d, e, i;
const char *closehead = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\nContent-Length: ";

const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

const char *hthead = "HTTP/1.1 200 OK\n";

const char *contjava = "Content-Type: text/javascript\n";
const char *conthtml = "Content-Type: text/html\n";
const char *conttxt = "Content-Type: text/plain\n";
//const char *contjava = "Content-Type: text/javascript\n";


const char *connclose = "Connection: close\n";
const char *contlen = "Content-Length: ";
if (argc == 2)
if (!strcmp(argv[1], "static"))
{logging ("static mode set", 1, 0); editmode = 0;}

while (1)
{
mode = 'z';
memset (buffer_in, 0, max_read);
memset (buffer_out1, 0, max_buff);
memset (buffer_out2, 0, max_buff);
memset (buffer_file, 0, max_read);
count = 0;
localfd = -1;
logging("+++++++ Waiting for new connection ++++++++", 1, 0);

connfd = accept(sv_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

if (!connfd)
{logging("fail on connect",1,0);continue;}

read_in = read(connfd, buffer_in, max_read);

method = buffer_in[0];
geturi (buffer_in, uri);

logging ("resource requested", 1, 0);
logging (uri, 1, 0);

if (!strcmp(uri, "/favicon.ico"))
	mode = '4';

if (!strcmp(uri, "/upload.cst"))
	mode = 'u';

if (uri[0] == '/' && uri[1] == 0)
{getcwd (path, 100); mode = 'r';}

// open file and set mode
if (mode == 'z')
{
midstr (uri, path, 1, strlen(uri));
localfd = open (path, O_RDONLY);
if (localfd > 0)
{
fstat (localfd, &finfo);
if (S_ISDIR(finfo.st_mode))
	{mode = 'd'; close (localfd);}

if (S_ISREG(finfo.st_mode))
	mode = 'f';
}else{
logging ("invalid file", 1, 0);
} //if valid file
} // final set f or d

// prg body
if (mode == 'u' && method == 'G')
{

stradd (buffer_out1, "<!DOCTYPE html>\n<html>\n<head>\n", &count);

stradd (buffer_out1, "<style>\n", &count);
stradd (buffer_out1, "body\n{\ntext-align:center;\nbackground-color:aqua;\nfont-size:52px;\n}\n", &count);
stradd (buffer_out1, "a:link\n{\ncolor:midnightblue;\ntext-decoration:none;\n}\n", &count);
stradd (buffer_out1, "</style>\n</head>\n<body>\n", &count);


stradd (buffer_out1, "<form enctype=\"multipart/form-data\" action=\"/upload.cst\" method=\"post\">\n", &count);

stradd (buffer_out1, "<input type=\"file\" name=\"myFile\">\n", &count);

stradd (buffer_out1, "<input type=\"submit\" value=\"upload\">\n", &count);

stradd (buffer_out1, "</form>\n</body>\n</html>", &count);

res = sprintf(buffer_out2, "%s%d\n\n%s", closehead, count, buffer_out1);
 write (connfd, buffer_out2, res);
logging ("bytes written: ", 2, res);

} // if get upload.cst

if (mode == 'u' &&  method == 'P')
{
//printf("%s\n", buffer_in);
a = instr(buffer_in, "boundary=", 1, read_in);
//logging ("a is: ", 2, a);

for (i = a +1;i < read_in;++i)
{
if (buffer_in[i] == 10)
{b = i; break;}
}
++a;
--b;
memset (uri, 0, 200);
midstr(buffer_in, uri, a, b);

a = instr (buffer_in, uri, b, read_in);
// a is first boundary

e = strlen(uri);
d = read_in - e - 10;

for (i = read_in; i > d; --i)
{
if (uri[e] ==  buffer_in[i])
{
if (e == 0)
{b = i; break;}
--e;
} // if

//printf("buffer: (%d) %c, uri: (%d) %c\n", i, buffer_in[i], e, uri[e]);
} // for

// b is set to rear boundary
// a is 1st boundary, b is 2nd
//
//midstr(buffer_in, buffer_out1, a, b);
//printf ("%s\n\n", buffer_out1);

c = instr (buffer_in, "filename=", a, b);
c += 2;
//printf("\nnew line\n");
for (i = c;i < b;++i)
{

//printf ("%c",  buffer_in[i]);
if (buffer_in[i] == 10)
{d = i; break;}
}
printf("\nnew line\n");
memset (path, 0, 200);
d -= 2;
midstr (buffer_in, path, c, d);

d += 5;
//printf("%s\n", path);


for (i = d;i < b;++i)
{

//printf ("%c",  buffer_in[i]);
if (buffer_in[i] == 10)
{d = i; break;}
}
// file contents bwtween d & b resp.

//e = b - d = filelen

c = open (path, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
b -= 3; // remove dashes and new line
e = b - d;
for (i = 0; i < e; ++i)
{
buffer_out1[i] = buffer_in[i + d];
} // for write file

buffer_out1[e] = 0;
//printf ("%s", buffer_out1);
write (c, buffer_out1, e);
close (c);

mode = 'z';
} // if handle up post

if (mode == 'r' || mode == 'd')
{

dp = opendir (path);

if (dp != NULL)
{
stradd (buffer_out1, "<!DOCTYPE html>\n<html>\n<head>\n", &count);

//stradd (buffer_out1, "<meta name=\"viewport\" content=\"width=device-width, intitial-scale=1\">", &count);

stradd (buffer_out1, "<style>\n", &count);
stradd (buffer_out1, "body\n{\ntext-align:center;\nbackground-color:aqua;\nfont-size:52px;\n}\n", &count);
stradd (buffer_out1, "a:link\n{\ncolor:midnightblue;\ntext-decoration:none;\n}\n", &count);
stradd (buffer_out1, "</style>\n</head>\n<body>\n", &count);

if (mode == 'r')
{
stradd (buffer_out1, "<a href=\"upload.cst\">[* UPLOAD FILE *]</a>\n", &count);
} // if root add upload link

while (ep = readdir (dp))
{
//if (ep->d_type == 4)
// 4 is dir 8 is file
//ep->d_name
if (ep->d_name[0] == '.')
	continue;

if (ep -> d_type == 4)
{
stradd (buffer_out1, "<br><a href=\"", &count);
if (mode == 'd')
{
stradd (buffer_out1, path, &count);
buffer_out1[count] = '/'; ++count;
} // if not root add full path
stradd (buffer_out1, ep->d_name, &count);
stradd (buffer_out1, "\">", &count);
stradd (buffer_out1, ep->d_name, &count);
stradd (buffer_out1, "/</a>\n", &count);
} // if dir


if (ep -> d_type == 8)
{
stradd (buffer_out1, "<br><a href=\"", &count);
if (mode == 'd')
{
stradd (buffer_out1, path, &count);
buffer_out1[count] = '/'; ++count;
} // if not root add full path
stradd (buffer_out1, ep->d_name, &count);
stradd (buffer_out1, "\">", &count);
stradd (buffer_out1, ep->d_name, &count);
stradd (buffer_out1, "</a>\n", &count);
} // if file

} // while dir list
 closedir (dp);
}else{
// if dir null here
} // done with dir entries if dir  null

stradd (buffer_out1, "</body>\n</html>", &count);
res = sprintf(buffer_out2, "%s%d\n\n%s", closehead, count, buffer_out1);
 write (connfd, buffer_out2, res);
logging ("bytes written: ", 2, res);
} // if mode d | r

if (mode == 'f')
{ // if file mode

fsize = finfo.st_size;

b = strlen(path);
for (i = b; i > 0; --i)
{
if (path[i] == '.')
{a = i; break;}
} // for
midstr (path, mime_ext, a, b);
 i = 0;

if (!strcmp(mime_ext, ".txt"))
{i=1;  mime_txt = conttxt;}


if (!strcmp(mime_ext, ".htm"))
{i=1;  mime_txt = conthtml;}


if (!strcmp(mime_ext, ".html"))
{i=1;  mime_txt = conthtml;}

if (!strcmp(mime_ext, ".js"))
{i=1;  mime_txt = contjava;}

if (!i)
{mime_txt = conttxt;}
// lone printf
//printf("%s\n", mime_txt);

logging(mime_txt, 1, 0);
//if (!strcmp(mime_ext, ".txt"))
//{i=1;  mime_txt = conttxt;}

//mode = 'z';
stradd (buffer_out1, hthead, &count);
stradd (buffer_out1, mime_txt, &count);
// if cont close
stradd (buffer_out1, connclose, &count);
stradd (buffer_out1, contlen, &count);

res = sprintf(buffer_out2, "%s%d\n\n", buffer_out1, fsize);

corkon (connfd);
write (connfd, buffer_out2, res);

// sendfile returns total sent for error checks
sendfile(connfd, localfd, 0, fsize);
close (localfd);

} // if file mode


if (mode == '4')
	write (connfd, "HTTP/1.1 410 GONE\n", strlen("HTTP/1.1 410 GONE\n"));


if (mode == 'z')
	write (connfd, "HTTP/1.1 500 ERROR\n", strlen("HTTP/1.1 500 ERROR\n"));

shutdown (connfd,  SHUT_WR);
	depleteSendBuffer(connfd);
	
	// now wait for the remote to close its socket
	
	for(;;) {
		res=read(connfd, dep_buffer, 500);
		if(res < 0) {
			perror("reading");
			exit(1);
		}
		if(!res) {
			printf("Correct EOF\n");
			break;
		}
	}
	close(connfd);

}// server loopp
}// end main
