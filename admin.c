
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

const char *closehead = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\nContent-Length: ";

//const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 0000012\n\nHello world!";

const char *hthead = "HTTP/1.1 200 OK\n";

const char *contjava = "Content-Type: text/javascript\n";
const char *conthtml = "Content-Type: text/html; charset=utf-8\n";
const char *conttxt = "Content-Type: text/plain\n";
const char *contjpg = "Content-Type: image/jpg\n";


//const char *contjava = "Content-Type: text/javascript\n";


const char *connclose = "Connection: close\n";
const char *contlen = "Content-Length: ";

struct buffer_data
{
char *p;
size_t max;
size_t len;
size_t mid;
size_t procint;
};

typedef struct buffer_data buffer;

struct str200
{
char p[200];
char mid;
char len;
};

typedef struct str200 namespace;

enum emode
{err, hellow, root, dir, file, upload, edit};

struct req_data
{
namespace uri;
namespace path;
char method;
int keepalive;
int code;
size_t fsize;
enum emode mode;
//int localfd; used code instead
}request;

struct myset
{
int keepalive;
size_t max_read;
size_t max_buff;
char *upuri;
char *uphandle;
char *rootpath;
}settings = {0, 1000, 100000, "upload.cst", "[* UPLOAD FILE *]", "/home"};

buffer outbuff;
buffer inbuff;

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

void send_txt(char *txt)
{

outbuff.len = sprintf (outbuff.p, "%s%s%s%s%d\n\n%s", hthead, conttxt, connclose, contlen, strlen(txt), txt);


} // send_txt

void send_ntxt(char *txt, int n)
{

outbuff.len = sprintf (outbuff.p, "%s%s%s%s%d\n\n%.*s", hthead, conttxt, connclose, contlen, n, n, txt);




} // send_ntxt

void init_buffers()
{


inbuff.max = settings.max_read;
inbuff.p = malloc(settings.max_read);
if (inbuff.p == NULL)
	logging ("error malloc inbuff", 100, 0);


outbuff.max = settings.max_buff;
outbuff.p = malloc(settings.max_buff);
outbuff.procint = 0;
if (outbuff.p == NULL)
	logging ("error malloc outbuff", 100, 0);

} // init_buffers

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

size_t midstr(char *major, char *minor, int start, int end)
{
int count = 0;
while (start < end)
{
minor[count] = major[start];
++start;
++count;
}
minor[count] = 0;
return (count - 1);
} // end midstr

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

void process_request (buffer *b)
{
request.method = b->p[0];
request.code = 500;

struct stat finfo;
	
int d1, d2, cnt;
int a;
cnt = 0;
for (d1 = 0;d1 < 100; ++d1)
if (b->p[d1] == 32)
break;
++d1;
for (d2 = d1; d2 < 200;++d2)
if (b->p[d2] == 32)
{break;}

cnt = d2 - d1;
request.uri.len = cnt;

cnt = 0;
while (d1 != d2)
{
request.uri.p[cnt] =  b->p[d1];
++cnt; ++d1;
} // while
//++cnt;
request.uri.p[cnt] = 0;

d1 = instr (b->p, "Connection: keep-alive", 1, b->procint);
if (d1 > 0)
request.keepalive = 1;

// if root mode.....programming
if (request.uri.p[0] == '/' && request.uri.p[1] == 0)
{
strcpy (request.path.p, settings.rootpath);
request.path.len = strlen(settings.rootpath);
request.mode = root;
return ;
}
request.path.len = sprintf(request.path.p, "%s%s", settings.rootpath, request.uri.p);

//logging (request.path.p, 1,0);


request.code = open (request.path.p, O_RDONLY);
if (request.code > 0)
{
fstat (request.code, &finfo);

if (S_ISDIR(finfo.st_mode)) // is dir
{request.mode = dir; close (request.code);}

if (S_ISREG(finfo.st_mode)) // is file
{request.fsize = finfo.st_size;	request.mode = file;}

}else{
 // check for favico....
	//check for **upload

if (!strcmp(request.uri.p, "/favicon.ico"))
{request.mode = err; request.code = 410;}

a = instr (request.uri.p, settings.upuri, 0, request.uri.len);
if (a > 0)
{
	request.mode = upload;
} // if upload mode
} //if valid file

} // process_request

int init_page (buffer *buf, int keepalive, int css)
{
char *filler = "000000";
	int count = 0;

if (keepalive)
buf->procint = sprintf (buf->p, "%s%s%s%s\n\n", hthead, conthtml, contlen, filler);

if (!keepalive)
buf->procint = sprintf (buf->p, "%s%s%s%s%s\n\n", hthead, conthtml, connclose, contlen, filler);

buf->len = buf->procint;

//logging (buf->p, 100, 0);

if (css)
{
stradd (buf->p, "<!DOCTYPE html>\n<html>\n<head>\n", &buf->len);

stradd (buf->p, "<style>\n", &buf->len);
stradd (buf->p, "body\n{\ntext-align:center;\nbackground-color:aqua;\nfont-size:52px;\n}\n", &buf->len);
stradd (buf->p, "a:link\n{\ncolor:midnightblue;\ntext-decoration:none;\n}\n", &buf->len);
stradd (buf->p, "</style>\n</head>\n<body>\n", &buf->len);

//return count;
} // if css
} // init_page

void fix_len(buffer *buf)
{

int numstart = buf->procint - 8;
char cnum[7];
int cnumlen;

size_t doclen = buf->len - buf->procint;

int a, i;
//printf("wjwjs\n");

a = sprintf(cnum, "%.*d", 6, doclen);

for (i = 0; i < 7; ++i)
{
if (cnum[i] == 0)
	break;
buf->p[i + numstart] = cnum[i];
} // for
//exit (0);	
} // fix_len


void serv_dir_static()
{
DIR *dp;
struct dirent *ep;

 init_page (&outbuff, 0, 1);

dp = opendir (request.path.p);
if (dp != NULL)
{

stradd (outbuff.p, "<a href=\"", &outbuff.len);

if (request.mode == dir)
{
// if dir mode, add full path
stradd (outbuff.p, request.uri.p, &outbuff.len);
outbuff.p[outbuff.len] = '/'; ++outbuff.len;
} // if dir mode add full path
stradd (outbuff.p, settings.upuri, &outbuff.len);
stradd (outbuff.p, "\">", &outbuff.len);
stradd (outbuff.p, settings.uphandle, &outbuff.len);
stradd (outbuff.p, "</a><br>\n", &outbuff.len);
// end add upload handle

while (ep = readdir (dp))
{
//if (ep->d_type == 4)
// 4 is dir 8 is file
//ep->d_name
if (ep->d_name[0] == '.')
	continue;

stradd (outbuff.p, "<a href=\"", &outbuff.len);
if (request.mode == dir)
{
// if dir mode, add full path
stradd (outbuff.p, request.uri.p, &outbuff.len);
outbuff.p[outbuff.len] = '/'; ++outbuff.len;
} // if dir mode add full path
stradd (outbuff.p, ep->d_name, &outbuff.len);
stradd (outbuff.p, "\">", &outbuff.len);
stradd (outbuff.p, ep->d_name, &outbuff.len);

if (ep -> d_type == 4)
{
// if directory add following /
outbuff.p[outbuff.len] = '/';
++outbuff.len;
} // if dir
stradd (outbuff.p, "</a><br>\n", &outbuff.len);

} // while ep ! NULL
} // if != dp null

stradd (outbuff.p, "</body>\n</html>", &outbuff.len);
 closedir (dp);

 fix_len(&outbuff);


} // serv_dir_static

void serv_dir_edit()
{

send_txt ("blahbafkjklngklgn");
return ;


DIR *dp;
struct dirent *ep;

 init_page (&outbuff, 0, 1);

dp = opendir (request.path.p);
if (dp != NULL)
{

stradd (outbuff.p, "<a href=\"", &outbuff.len);

if (request.mode == dir)
{
// if dir mode, add full path
stradd (outbuff.p, request.uri.p, &outbuff.len);
outbuff.p[outbuff.len] = '/'; ++outbuff.len;
} // if dir mode add full path
stradd (outbuff.p, settings.upuri, &outbuff.len);
stradd (outbuff.p, "\">", &outbuff.len);
stradd (outbuff.p, settings.uphandle, &outbuff.len);
stradd (outbuff.p, "</a><br>\n", &outbuff.len);
// end add upload handle

while (ep = readdir (dp))
{
//if (ep->d_type == 4)
// 4 is dir 8 is file
//ep->d_name
if (ep->d_name[0] == '.')
	continue;

stradd (outbuff.p, "<a href=\"", &outbuff.len);
if (request.mode == dir)
{
// if dir mode, add full path
stradd (outbuff.p, request.uri.p, &outbuff.len);
outbuff.p[outbuff.len] = '/'; ++outbuff.len;
} // if dir mode add full path
stradd (outbuff.p, ep->d_name, &outbuff.len);
stradd (outbuff.p, "\">", &outbuff.len);
stradd (outbuff.p, ep->d_name, &outbuff.len);

if (ep -> d_type == 4)
{
// if directory add following /
outbuff.p[outbuff.len] = '/';
++outbuff.len;
} // if dir
stradd (outbuff.p, "</a><br>\n", &outbuff.len);

} // while ep ! NULL
} // if != dp null

stradd (outbuff.p, "</body>\n</html>", &outbuff.len);
 closedir (dp);

 fix_len(&outbuff);


} // serv_dir_edit


void get_upload(int connfd)
{
//logging ("get upload", 1, 0);



 init_page (&outbuff, 0, 1);

stradd (outbuff.p, "<form enctype=\"multipart/form-data\" action=\"", &outbuff.len);
//if (request.mode == dir)
stradd (outbuff.p, settings.upuri, &outbuff.len);
stradd (outbuff.p, "\" method=\"post\">\n", &outbuff.len);

stradd (outbuff.p, "<input type=\"file\" name=\"myFile\">\n", &outbuff.len);

stradd (outbuff.p, "<input type=\"submit\" value=\"upload\">\n", &outbuff.len);

stradd (outbuff.p, "</form>\n</body>\n</html>", &outbuff.len);

 fix_len(&outbuff);


} // get upload

int post_upload (int connfd)
{
int a, b, c, d, e, i;
char bnd[200];
char filename[200];
char path[200];
char fpath[200];
char filebuffer[settings.max_read];
int loopcount = 0;
int boundset = 0;
int boundlen = 0;
int fbound = 0;
int lbound = 0;
int fileset = 0;
int newfd = 0;

 while (1)
 {

if (loopcount)
{
memset (inbuff.p, 0, inbuff.max);	

inbuff.len = read (connfd, inbuff.p, inbuff.max);
if (inbuff.len == -1)
{
usleep (1000); 
//logging ("reading blocked",1,0); 
continue;
}



//logging ("loop iteration ", 2, loopcount);
//printf("%s\n", inb->p);
} // do subsequest reads

if (!boundset)
{
a = instr(inbuff.p, "boundary=", 1, inbuff.len);
if (a < 0)
	{request.mode = err; request.code = 500; return 0;}

// check for newline
for (i = a + 1; i < inbuff.len; ++i)
{
if (inbuff.p[i] == 10)
{b = i; break;}
}
++a;
--b;

// set boundary info
memset (bnd, 0, 200);
midstr(inbuff.p, bnd, a, b);
boundlen = strlen (bnd);
boundset = 1;
logging ("boundary set", 1, 0);
}// set boundary info

if (!fbound)
{	

if (!loopcount)
a = instr (inbuff.p, bnd, b, inbuff.len);

if (loopcount)
a = instr (inbuff.p, bnd, 0, inbuff.len);

if (a < 0) // if first boundary is null (chrome), continue to next read
{++loopcount; printf("no bound, chrome\n");  continue;}

logging ("continued after 1st boundary found", 1, 0);
fbound = a;
} // if !fbound

if (!fileset)
{
a = instr (inbuff.p, "filename=\"", fbound, inbuff.len);
//logging ("value of a is: ", 2, a);
//send_txt (connfd, "blah");

for (i = a; i < inbuff.len; ++i)
{
if (inbuff.p[i] == 10)
{b = i; break;}

}// for
//++a;
//b -= 2;

midstr (inbuff.p, filename, a + 1, b - 2);
logging (filename, 1, 0);
// set filename

fbound = b;
 //b set after to filename=**\n

b = instr (inbuff.p, "\n\n", fbound, inbuff.len);

fbound = b;
 
fbound += 3;
 // reset proper fboundary

for (i = request.path.len; i > 0; --i)
	if (request.path.p[i] == '/')
	{b = i; break;}

if (i > 0)
{
midstr (request.path.p, path, 0, i);
//logging (path, 1, 0);
sprintf (fpath, "%s/%s", path, filename);
}else{
//logging ("no path", 1, 0);
strcpy(fpath, filename);
} // set new filename path
fileset = 1;
} // if !fileset
// set full filename path

//printf ("new file path set: %s\n", fpath);
//logging (fpath, 1, 0);

if (!newfd)
{
logging ("new file not open", 1, 0);

newfd = open (fpath, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);

lbound = instr (inbuff.p, bnd, fbound, inbuff.len);
	
if (lbound < 0)
{
logging ("file not open, no rear boundary", 1, 0);
a = inbuff.len - fbound;

for (i = 0; i < a; ++i)
filebuffer[i] = inbuff.p[i + fbound];

//write new file contents
write (newfd, filebuffer, a);
++loopcount;
continue;
// if no rear boundary
}else{ 
logging ("file not open rear boundary found",1,0);

a = inbuff.len - fbound - boundlen;

//for (i = inb->procint; i > 0; --i)
//if (inb->p[i] == 10)
//{--i; a = i;}

for (i = 0; i < a; ++i)
filebuffer[i] = inbuff.p[i + fbound];

for (i = a; i > 0; --i)
if (filebuffer[i] == 10)
	{b = i; break;}

//write new file contents
write (newfd, filebuffer, b);
send_txt ("file recieved succesfully");
break;
} //if rear boundary
} // if file not open


if (newfd)
{

lbound = instr (inbuff.p, bnd, 0, inbuff.len);

if (lbound < 0)
{
logging ("new file created, no rear boundary",1, 0);
	for (i = 0; i < inbuff.len; ++i)
	filebuffer[i] = inbuff.p[i];

	//write new file content
	write (newfd, filebuffer, inbuff.len);
	++loopcount;
	continue;
	// if no rear boundary
}else{ 

logging ("new file created, rear boundary found",1, 0);
a = inbuff.len - boundlen;

for (i = 0; i < a; ++i)
	filebuffer[i] = inbuff.p[i];

for (i = a; i > 0; --i)
if (filebuffer[i] == 10)
	{b = i; break;}

// write file contents
write (newfd, filebuffer, b - 1);

break;
} //if rear boundary



}// if file already open



//exit (0);


// a is first boundary

} // loop

send_txt ("success, check for file completion");

close (newfd);
} //post_upload

int getlast(char *buf, int start, char last)
{
int i;
int d = 0;
for (i = start; i > 0; --i)
{
if (d)
printf("%C, %d\n", buf[i], buf[i]);

if (buf[i] == last)
return i;

}
return -1;
}

int file_mode(int connfd)
{
int a, b, i;
char mime_ext[10] = "";
const char *mime_txt;

char header[settings.max_read];

//--request.path.len;

//logging (request.path.p, 1, 0);

a = getlast (request.path.p, request.path.len, '.');

//printf ("a is: %d, len is: %d\n", a, request.path.len);
midstr (request.path.p, mime_ext, a, request.path.len);
//logging (mime_ext, 1,a);
i = 0;

//exit (0);

if (!strcmp(mime_ext, ".txt"))
{i=1;  mime_txt = conttxt;}


if (!strcmp(mime_ext, ".htm"))
{i=1;  mime_txt = conthtml;}


if (!strcmp(mime_ext, ".html"))
{i=1;  mime_txt = conthtml;}

if (!strcmp(mime_ext, ".js"))
{i=1;  mime_txt = contjava;}

if (!strcmp(mime_ext, ".jpg"))
{i=1;  mime_txt = contjpg;}

if (!strcmp(mime_ext, ".jpeg"))
{i=1;  mime_txt = contjpg;}


if (!i)
{mime_txt = conttxt;}
// lone printf
//printf("%s\n", mime_txt);

logging(mime_txt, 1, 0);
//if (!strcmp(mime_ext, ".txt"))
//{i=1;  mime_txt = conttxt;}

//mode = 'z';



if (request.keepalive == 1 && settings.keepalive == 1)
{
a = sprintf(header, "%s%s%s%d\n\n", hthead, mime_txt, contlen, request.fsize);
}else{
a = sprintf(header, "%s%s%s%s%d\n\n", hthead, mime_txt, connclose, contlen, request.fsize);
} // if keep alive

write (connfd, header, a);

// sendfile returns total sent for error checks
sendfile(connfd, request.code, 0, request.fsize);
close (request.code);

} // file_mode

void send_err(char *errtext)
{
outbuff.len = strlen(errtext) + 1;
strcpy(outbuff.p, errtext);

}


int main (int argc, char **argv)
{
struct sockaddr_in address;
int connfd;
int sv_fd = prepsocket(9999);
socklen_t addrlen = sizeof(address);

int a, b, c, d, e, i;
int editorfd = -1;

init_buffers();

void (*serv_dir)() = serv_dir_static;

if (argc == 2)
editorfd = open (argv[1], O_RDONLY);

if (editorfd > 0)
{logging ("editor mode init", 1, 0); serv_dir = serv_dir_edit;}

while (1)
{
memset (&request, 0, sizeof(request));
reset:

logging ("waiting", 1, 0);
connfd = accept(sv_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

a = fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFL, 0) | O_NONBLOCK);
if (a == -1)
  perror("calling fcntl");


inbuff.len = -1;
while (inbuff.len == -1)
{
inbuff.len = read (connfd, inbuff.p, inbuff.max);
if (inbuff.len == -1)
{
usleep (1000);
//logging ("reading blocked main read!",1,0);
continue;
}
}// while read

process_request (&inbuff);

logging ("resource requested", 1, 0);
logging (request.uri.p, 1, 0);

// for diagnostics testing
//send_ntxt ("test worked", 10);
//send_txt ("test is not working");
//request.mode = hellow;

if (request.mode == root || request.mode == dir)
serv_dir ();


if (request.mode == upload && request.method == 'G')
	get_upload (connfd);


if (request.mode == upload && request.method == 'P')
	post_upload (connfd);


if (request.mode == file && request.method == 'G')
	file_mode(connfd);

if (request.mode == file && request.method == 'P')
	send_ntxt (inbuff.p, inbuff.len);


if (request.mode == err && request.code == 410)
	send_err ("HTTP/1.1 410 GONE\n");


if (request.mode == err && request.code == 500)
	send_err ("HTTP/1.1 500 ERROR\n");



if (outbuff.len > 0)
{
logging ("bytes queued: ", 2, outbuff.len);
write (connfd, outbuff.p, outbuff.len);
outbuff.len = 0;
}



shutdown (connfd,  SHUT_WR);

//wait for client to close connection

while (1) 
{
a=read(connfd, inbuff.p, 500);

if (a > 0)
perror("reading \"success\" in kill mode");

if(a < 0)
{
{
usleep (1000);
//logging ("closing, remote, not ready",1,0);
}
//exit(1);
}
if(!a) 
{
logging ("Connection closed by client", 1, 0);
break;
} // if socket non blocked
} // for wait for close bit.

close(connfd);


}// server loop
}// end main
