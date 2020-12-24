#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <fcntl.h>
#include <signal.h>
#include <time.h>

//#include <stdio.h>

#define out_max 100000
#define max_out 100000
#define str_max 1000
#define max_str 1000

#define timeout 5

const char *closehead = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\nContent-Length: ";
//const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 0000012\n\nHello world!";
const char *hthead = "HTTP/1.1 200 OK\n";
const char *contjava = "Content-Type: text/javascript\n";
const char *conthtml = "Content-Type: text/html; charset=utf-8\n";
const char *conttxt = "Content-Type: text/plain\n";
const char *contjpg = "Content-Type: image/jpg\n";
const char *contcss = "Content-Type: text/css\n";
const char *connclose = "Connection: close\n";
const char *contlen = "Content-Length: ";

const char *rootpath = "/home";
const char *editoruri = "/editor.cst";
const char *upuri = "/upload.cst";
const char *uphandle = "[* UPLOAD FILE *]";

struct buffer_data
{
char data [max_out];
int procint;
int len;
};
struct string_data
{
char data [str_max];
int procint;
int len;
};
typedef struct buffer_data bfr;
typedef struct string_data str;

struct req_data 
{
str uri;
str path;
str referer;
char method;
int code;
int localfd;
int keepalive;
size_t fsize;
enum emode
{none = 0, err, root, dir, file, get_upload, post_upload, multi_part_recv, get_edit, post_edit, ka} mode;

struct boundary_info
{
char info[100];
int b1;
int b2;
int b3;
}boundary; // boundary
}; // request data

int int_chr (char *base, const int num)
{
int r;
int len = 0;
int iter = num;
while (iter)
{
iter /= 10;
++len;
}
// get length
int spos = len - 1;
base[len] = 0;
iter = num;
while (spos > -1)
{
r = iter % 10;
base[spos] = r + 48;
iter /= 10;
--spos;
}

return len;
} // int_chr

int chrlen (const char *str)
{
int i = 0;
while (1)
{
if (str[i] == 0)
	return i;
++i;
} // while
} // strlen

void logging (const char *msg)
{
int len = chrlen (msg);
write (1, msg, len);
}

void nlogging (const char *msg, const int num)
{
char cnum[10];
int len = chrlen (msg);
int cnumlen = int_chr (cnum, num);
write (1, msg, len);
write (1, cnum, cnumlen);
write (1, "\n", 1);
}

void term (const char *msg)
{
logging (msg);
exit (0);
}

int sgetnext (const str buf, int cnt, const char term)
{
while (cnt < buf.len)
{
if (buf.data[cnt] == term)
	return cnt;
++cnt;
} // while
return -1;
} // sgetnext


int chrcmp (const char *s1, const char *s2)
{
int i = 0;
while (1)
{
if (s1[i] == 0 && s2[i] == 0)
	return 1;

if (s1[i] == s2[i])
{
++i;
continue;
}else{
return 0;
}

} // while
} // chrcmp


int searchS (const str buf, const char *term, int cnt)
{
int sc = 0;
while (cnt < buf.len)
{
//printf ("%c, %c\n", buf.data[cnt], term[sc]);
if (term[sc] == 0)
return cnt;
if (buf.data[cnt] == term[sc])
{

++cnt;
++sc;
continue;
}else{ // if
sc = 0;
++cnt;
} // if
} // while
return -1;
} // searchS


int midbfr (bfr *dest, const char *src, const int start, int end, const int offs)
{
if (end == 0)
	end = chrlen (src);

int sc = offs;
int cnt = start;
while (cnt < end)
{
dest->data[sc] = src[cnt];
++cnt;
++sc;
} // while
dest->data[sc] = 0;
int len = end - start + offs;
dest->len = len;
return len;
} // midbfr


void appbfr (bfr *dest, const char *src)
{
int end = chrlen (src);
int offs = dest->len;
int sc = offs;
int cnt = 0;
while (cnt < end)
{
dest->data[sc] = src[cnt];
++cnt;
++sc;
} // while
dest->data[sc] = 0;
//int len = end + offs;
dest->len = end + offs;
//return len;
} // midbfr

int midstr (str *dest, const char *src, const int start, int end, const int offs)
{
if (end == 0)
	end = chrlen (src);
int sc = offs;
int cnt = start;
while (cnt < end)
{
dest->data[sc] = src[cnt];
//printf ("%c\n", src[cnt]);
++cnt;
++sc;
} // while
dest->data[sc] = 0;
int len = end - start + offs;
dest->len = len;
return len;
} // midstr




int prepsocket (const int port)
{
int result = 0;
int optval = 1;
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
struct sockaddr_in address;
int addrlen = sizeof(address);
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons (port);
result = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval , sizeof(int));
if (result == -1)
	term("error, reuse addr");
result = bind(server_fd, (struct sockaddr *)&address,(socklen_t) sizeof(address));
if (result == -1)
	term("error, bind");
result = listen(server_fd, 10);
if (result == -1)
	term("error, reuse listen");

result = fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);
if (result == -1)
  	term ("calling fcntl");
return (server_fd);
}// end prep socket



void send_ntxt(bfr *out, char *txt, int n)
{

if (n == 0)
n = chrlen (txt);

char cnum[10];
int_chr (cnum, n);

midbfr (out, hthead, 0, 0, 0);
appbfr (out, conttxt);
appbfr (out, connclose);
appbfr (out, contlen);
appbfr (out, cnum);
appbfr (out, "\n\n");
appbfr (out, txt);

} // send_ntxt


int poll_serv (const int fd)
{
struct sockaddr_in address;
socklen_t addrlen = sizeof(address);

time_t basetime;
time (&basetime);

while (1)
{
int connfd = accept(fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
if (connfd > 0)
{
int result = fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFL, 0) | O_NONBLOCK);
if (result == -1)
 	term ("calling fcntl poll serv\n");

	return connfd;
} // connection

if (connfd == -1)
{
usleep (1000);
time_t deadtime;
time (&deadtime);

deadtime -= basetime;
if (deadtime >= timeout)
	return -1;

} // if connfd = -1

} // while
} // poll_serv


int sock_read (str *inb, int connfd)
{
time_t basetime;
time (&basetime);

int len = -1;
//read (connfd, inb->data, in_size);
while (len < 0)
{
len = read (connfd, inb->data, str_max);

if (len == -1)
{
usleep (1000);
time_t deadtime;
time (&deadtime);

deadtime -= basetime;
if (deadtime >= timeout)
	{logging ("error timeout read"); return -1;}

} // if -1
} // while

inb->len = len;

return 1;
} // sock_read


int sock_term (const int connfd)
{
char temp [1000];
shutdown (connfd,  SHUT_WR);
while (1)
{
int res = read(connfd, temp, 1000);
if(res < 0)
{
usleep (1000); 
//logging ("waiting for connection to close\n");
}
if(!res)
{
logging ("Connection closed by client\n");
break;
} //if
} // for
close(connfd);
}

struct req_data process_request (const str inb)
{
//write (1, inb.data, inb.len);
struct req_data r;
r.method = inb.data[0];
r.code = 500;
r.mode = err;
// set method, and default err 500 if nothing changes it

//term (inb.data);


int a = sgetnext (inb, 0, (char) 32);
++a;
int b = sgetnext (inb, a, (char) 32);
midstr (&r.uri, inb.data, a, b, 0);
// set uri
midstr (&r.path, rootpath, 0, 0, 0); 
// set path with root dir

//a = searchS (

if (r.uri.data[0] == '/' && r.uri.data[1] == 0)
	{r.mode = root; return r;}
// if root request return

a = chrcmp (r.uri.data, "/favicon.ico");
if (a)
{r.code = 410; return r;}

//a = searchS (inb, , 0);

//a = searchS (inb, upuri, 0);
//a = searchS (inb, editoruri, 0);

//const char *editoruri = "/editor.cst";
//const char *upuri = "/upload.cst";

midstr (&r.path, r.uri.data, 0, 0, r.path.len);
// set path with uri added

r.localfd = open (r.path.data, O_RDONLY);
if (r.localfd > 0)
{
struct stat finfo;
fstat (r.localfd, &finfo);

if (S_ISDIR(finfo.st_mode)) // is dir
{r.mode = dir; close (r.localfd);}

if (S_ISREG(finfo.st_mode)) // is file
{r.fsize = finfo.st_size; r.mode = file;}

} // if valid file

return r;
} // process_request

void init_htm (bfr *bf, int keepalive, int css)
{
const char *filler = "000000";
midbfr (bf, hthead, 0, 0, 0);
appbfr (bf, conthtml);
if (!keepalive)
	appbfr (bf, connclose);
appbfr (bf, contlen);

appbfr (bf, filler);
appbfr (bf, "\n\n");
bf->procint = bf->len;
// mark length pos to rewrite later
appbfr (bf, "<!DOCTYPE html>\n<html>\n");

if (css)
{
appbfr (bf, "<head>\n");
//appbfr (bf , "<meta name=\"viewport\" intitial-scale=\"1\">");
appbfr (bf, "<style>\n");
appbfr (bf, "body\n{\ntext-align:left;\nmargin-left:70px;\nbackground-color:aqua;\nfont-size:52px;\n}\n");
appbfr (bf, "a:link\n{\ncolor:midnightblue;\ntext-decoration:none;\n}\n");

} // if css
} // init_htm (bf, keepalive, css)

void fix_len(bfr *buf)
{
char cnum[10];
int doclen = buf->len - buf->procint;

int gap = 6 - int_chr (cnum, doclen);
int numstart = buf->procint + gap - 8;

int sc = 0;
while (1)
{
if (cnum[sc] == 0)
break;
buf->data[numstart] = cnum[sc];
++sc;
++numstart;
}
} // fix_len

int sock_write (const bfr out, int connfd)
{
time_t basetime;
time (&basetime);

//void fixit()

int len = -1;
//nlogging ("bytes queued: ", out.len);

while (len < 0)
{
len = write (connfd, out.data, out.len);

if (len == -1)
{
usleep (1000);
time_t deadtime;
time (&deadtime);
deadtime -= basetime;

if (deadtime >= timeout)
	{logging ("error timeout write"); return 0;}

} // if -1

//if (len < out.len

} // while
//nlogging ("bytes written: ", len);
if (len == out.len)
{
if (len < out_max)
	nlogging ("bytes written: ", len);
}else{
nlogging ("bytes queued: ", out.len);
nlogging ("bytes written: ", len);

return 0;
} // logging if
return len;
} // sock_write

void serv_dir_static (const struct req_data req, const int connfd)
{
logging ("static directory request\n");

bfr out;
bfr *bf = &out;

init_htm (bf, 0, 1);
DIR *dp;
struct dirent *ep;

appbfr (bf, "</style>\n</head>\n<body>\n");
 //add time stamp
time_t rawtime;
struct tm * timeinfo;
char buffer [80];
time (&rawtime);
timeinfo = localtime (&rawtime);
strftime (buffer,80,"Current Time: %I:%M:%S%p.\n<br>",timeinfo);
appbfr (bf, buffer);

// add upload handle
appbfr (bf, "<a href=\"");

if (req.mode == dir)
{
// if dir mode, add full path
appbfr (bf, req.uri.data);
//bf->data[bf->len] = '/'; ++bf->len;
} // if dir mode add full path
appbfr (bf, upuri);
appbfr (bf, "\">");
appbfr (bf, uphandle);
appbfr (bf, "</a><br>\n");
// end add upload handle

dp = opendir (req.path.data);
while (ep = readdir (dp))
{
//if (ep->d_type == 4)
// 4 is dir 8 is file
//ep->d_name
if (ep->d_name[0] == '.')
	continue;

appbfr (bf, "<a href=\"");
if (req.mode == dir)
{
// if dir mode, add full path
appbfr (bf, req.uri.data);
bf->data[bf->len] = '/'; ++bf->len;
} // if dir mode add full path
appbfr (bf, ep->d_name);
appbfr (bf, "\">");
appbfr (bf, ep->d_name);

if (ep -> d_type == 4)
{
// if directory add following /
bf->data[bf->len] = '/';
++bf->len;
} // if dir
appbfr (bf, "</a><br>\n");

} // while ep ! NULL

appbfr (bf, "</body>\n</html>");
 closedir (dp);

fix_len (bf);

sock_write (out, connfd);

} // serv_dir_static

int getlast(const str in, char last)
{
int i;

for (i = in.len; i > 0; --i)
{
	if (in.data[i] == last)
	return i;
}
return -1;
}

int send_file (const struct req_data req, const int connfd)
{

int a = getlast (req.uri, '.');

char mime_ext[10];
const char *mime_txt;
int sc = 0;

while (a < req.uri.len)
{
mime_ext[sc] = req.uri.data[a];
++a;
++sc;
}
mime_ext[sc] = 0;

a = 0;
if (chrcmp(mime_ext, ".txt"))
{a=1;  mime_txt = conttxt;}

if (chrcmp(mime_ext, ".htm"))
{a=1;  mime_txt = conthtml;}

if (chrcmp(mime_ext, ".html"))
{a=1;  mime_txt = conthtml;}

if (chrcmp(mime_ext, ".js"))
{a=1;  mime_txt = contjava;}

if (chrcmp(mime_ext, ".jpg"))
{a=1;  mime_txt = contjpg;}

if (chrcmp(mime_ext, ".jpeg"))
{a=1;  mime_txt = contjpg;}

if (chrcmp(mime_ext, ".css"))
{a=1;  mime_txt = contcss;}

if (!a)
{mime_txt = conttxt;}

logging(mime_txt);
logging("\n");

bfr out;
bfr *bf = &out;

midbfr (bf, hthead, 0, 0, 0);
appbfr (bf, mime_txt);

if (!req.keepalive)
	appbfr (bf, connclose);
appbfr (bf, contlen);

char chr_contlen[10];
int_chr (chr_contlen, req.fsize);

appbfr (bf, chr_contlen);
appbfr (bf, "\n\n");
bf->procint = bf->len;

sock_write (out, connfd);

size_t progress = 0;

bfr in;

while (progress < req.fsize)
{
in.len = read (req.localfd, in.data, max_out);

int rtn = sock_write (in, connfd);

progress += rtn;
}


return 0;
}



int main (int argc, char **argv)
{
logging ("hello world\n");

int connfd = -1;
int servfd = prepsocket (9999);

str inbuff;
struct req_data request;
request.mode = 0;

int loopint = 0;
while (1) // server loop
{
++loopint;
nlogging ("loop int: ", loopint);

//if (request.mode == none)
//{
	while (connfd < 0)
	connfd = poll_serv (servfd);
	 // accept loop
//} // if request mode == 0

//if (request.mode == none || request.mode == ka)
//{
sock_read (&inbuff, connfd);

request = process_request (inbuff);
request.keepalive = 0;

logging ("requested: ");
logging (request.uri.data);
logging ("\n");
//} // if new read / request process

if (request.mode == root || request.mode == dir)
	serv_dir_static (request, connfd);

if (request.mode == file)
	send_file (request, connfd);



//if (request.mode == multi_part_send)
//{
//int a = multi_send (&outbuff, request)
//if (a) {request.progress = a;}

//if (!a) request.mode == 0) 
//}

//enum emode
//{none = 0, err, root, dir, file, get_upload, post_upload, multi_part_recv, get_edit, post_edit, ka} mode;

if (request.mode == ka)
	continue;

sock_term (connfd);
// reset connection to -1
request.mode == none;
connfd = -1;
} // server loop
} // main
