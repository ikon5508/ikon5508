
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

int main (int argc, char **argv)
{
struct sockaddr_in address;
int editmode = 1;
int localfd;
int connfd;
int sv_fd = prepsocket(9999);
socklen_t addrlen = sizeof(address);
ssize_t max_read = 10000;
ssize_t max_buff = 100000;
ssize_t res;
ssize_t read_in = max_read;
ssize_t write_out;
char  buffer_in[max_read];
char buffer_out1[max_buff];
char buffer_out2[max_buff];
char buffer_file[max_read];
char dep_buffer[500];
char method;
char uri[200];
char mime_ext[10];
const char *mime_txt;
char path[200];
char mode;
size_t count;
DIR *dp;
struct dirent *ep;
struct stat finfo;
size_t fsize;
int a, b, i;
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

stradd (buffer_out1, "<meta name=\"viewport\" content=\"width=device-width, intitial-scale=1\">", &count);

stradd (buffer_out1, "<style>\n", &count);
stradd (buffer_out1, "body\n{\ntext-align:center;\nbackground-color:aqua;\nfont-size:52px;\n}\n", &count);
stradd (buffer_out1, "a:link\n{\ncolor:midnightblue;\ntext-decoration:none;\n}\n", &count);

if (editmode)
{// if edit mode add overlay css
stradd (buffer_out1, "#overlay\n{\nposition: fixed;\ndisplay: none;\nwidth: 100%;\nheight: 100%;\ntop: 0;\nleft: 0;\nright: 0;\nbottom: 0;\nbackground-color: rgba(0,0,0,0.5);\nz-index: 2;\ncursor: pointer;\n}\n", &count);
stradd (buffer_out1, "#text\n{\nposition: absolute;\ntop: 50%;\nleft: 50%;\nfont-size: 50px;\ncolor: white;\ntransform: translate(-50%,-50%);\n-ms-transform: translate(-50%,-50%);\n", &count);
stradd (buffer_out1, ".btn\n{\nfont-size:36px;\n}", &count);

} // if edit mode add overlay css
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


if (ep -> d_type == 8)// if file
{
if (!editmode)
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
} // if ! edit mode

if (editmode)
{
stradd (buffer_out1, "\n<br><a href=\"\" onclick=\"on(\'", &count);

if (mode == 'd')
{
stradd (buffer_out1, path, &count);
} // if ddir, not /
if (mode == 'r')
{
stradd (buffer_out1, "root", &count);
} // if root

stradd (buffer_out1, "\' ,\'", &count);
stradd (buffer_out1, ep->d_name, &count);
stradd (buffer_out1, "\')\">", &count);
stradd (buffer_out1, ep->d_name, &count);
stradd (buffer_out1, "</a>\n", &count);

//  build editmode page

} // if editmode
} // if file
} // while dir list
 closedir (dp);
}else{
// if dir null here
} // done with dir entries if dir  null

// if editmode add overlay java

if (editmode)
{

stradd (buffer_out1, "<div id=\"overlay\" onclick=\"off()\">", &count);

stradd (buffer_out1, "<div id=\"text\"><input type=\"button\"> Overlay text</input> </div>\n</div>", &count);

stradd (buffer_out1, "<script>\nfunction on(path, obj)\n{\ndocument.getElementById(\"overlay\").style.display = \"block\";}\n", &count);

stradd (buffer_out1, "function off()\n{\ndocument.getElementById(\"overlay\").style.display = \"none\";\n}\n</script>\n", &count);

} // if edit mode add java and overlay
/*
stradd (buffer_out1, "<script>\nfunction on(path, obj)\n{\n", &count);
stradd (buffer_out1, "window.alert(path);\n window.alert(obj)\n}\n</script>\n", &count);
*/

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
