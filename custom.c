
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
int a, b, c, i;
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

if (uri[0] == '/' && uri[1] == 0)
{
getcwd (path, 100); mode = 'r';
}else{
if (method == 'P')
{
mode = 'p';
}else{
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
} // if valid file
} // ifmethod = G & mode is f or d
} // set mode (directory,file,post,root)

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

// if editmode add overlay java
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
 intfiletype = 0;

if (!strcmp(mime_ext, ".txt"))
{intfiletype=1;  mime_txt = conttxt;}


if (!strcmp(mime_ext, ".htm"))
{intfiletype=1;  mime_txt = conthtml;}


if (!strcmp(mime_ext, ".html"))
{intfiletype=1;  mime_txt = conthtml;}

if (!strcmp(mime_ext, ".js"))
{intfiletype=1;  mime_txt = contjava;}

if (!strcmp(mime_ext, ".c"))
{intfiletype=101;  mime_txt = conttxt;}


//if (!strcmp(mime_ext, ".txt"))
//{i=1;  mime_txt = conttxt;}
if (!intfiletype)
{mime_txt = conttxt;}
// end mime mapping

logging(mime_txt, 1, 0);
if (!editmode)
{ // if static send file
stradd (buffer_out1, hthead, &count);
stradd (buffer_out1, mime_txt, &count);
// con close
stradd (buffer_out1, connclose, &count);
stradd (buffer_out1, contlen, &count);

res = sprintf(buffer_out2, "%s%d\n\n", buffer_out1, fsize);

corkon (connfd);
write (connfd, buffer_out2, res);

// sendfile returns total sent for error checks
sendfile(connfd, localfd, 0, fsize);
close (localfd);
} // if static

if (editmode)
{
if (intfiletype <= 1)
{ // if standardfile send it
logging ("sending standard file", 1, 0);

stradd (buffer_out1, hthead, &count);
stradd (buffer_out1, mime_txt, &count);
// con close
stradd (buffer_out1, connclose, &count);
stradd (buffer_out1, contlen, &count);

res = sprintf(buffer_out2, "%s%d\n\n", buffer_out1, fsize);

corkon (connfd);
write (connfd, buffer_out2, res);

// sendfile returns total sent for error checks
sendfile(connfd, localfd, 0, fsize);
close (localfd);
}

if (intfiletype > 1)
{ // load editor
logging ("loading editor", 1, 0);
editorfd = open (editor, O_RDONLY);
if (editorfd > 0)
{
read_in = read(editorfd, buffer_out2, max_buff);
++read_in;
buffer_out2[read_in] = 0;

b = read_in; // store editor-len in b

read_in = read (localfd, buffer_file, max_buff);
++read_in;
buffer_file[read_in] = 0;

a = instr(buffer_out2, "DELIMETER", 1, 100000);
a -= 8;
i = 0;
res = 0;

while (i < a)
{
buffer_out1[res] = buffer_out2[i];
++i;
++res;
} // while


i = 0;
while (i < read_in)
{
buffer_out1[res] = buffer_file[i];
++i;
++res;
} // while

a += 9;

//i = res;
res -= 1;
while (a < b)
{
buffer_out1[res] = buffer_out2[a];
++a;
++res;
} // while

memset (buffer_out2, 0, max_buff);

write_out = sprintf (buffer_out2, "%s%d\n\n%s", closehead, res, buffer_out1);
write (connfd, buffer_out2, write_out);
logging ("editor loaded, bytes sent: ", 2, write_out);

close (localfd);
close (editorfd);
//mode = 'z';
}else{
// invalod editor
mode = 'z';
close (localfd);
}// if editor valid file
} // load editor
} // if edit mode
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
