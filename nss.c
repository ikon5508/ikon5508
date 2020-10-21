#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>


/* Global SSL context */
SSL_CTX *ctx;


// globals used during client init

SSL *ssl;

BIO *rbio; /* SSL reads from, we write to. */
BIO *wbio; /* SSL writes to, we read from. */

enum sslstatus { SSLSTATUS_OK, SSLSTATUS_WANT_IO, SSLSTATUS_FAIL};
int die (char *str)
{
printf("%s", str);
exit (0);
}

int socklisten(int port)
{
  char str[INET_ADDRSTRLEN];

  int clientfd;
int servfd = socket(AF_INET, SOCK_STREAM, 0);
  if (servfd < 0)
    die("socket()");

  struct sockaddr_in servaddr;
  int enable = 1;
  if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    die("setsockopt(SO_REUSEADDR)");

  /* Specify socket address */
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if (bind(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    die("bind()");

  if (listen(servfd, 128) < 0)
    die("listen()");

 return (servfd);

}

void ssl_init(const char * certfile, const char* keyfile)
{
  printf("initialising SSL\n");

  /* SSL library initialisation */
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();

  /* create the SSL server context */
  ctx = SSL_CTX_new(SSLv23_method());
  if (!ctx)
    die("SSL_CTX_new()");

  /* Load certificate and private key files, and check consistency */
  if (certfile && keyfile) {
    if (SSL_CTX_use_certificate_file(ctx, certfile,  SSL_FILETYPE_PEM) != 1)
      die("SSL_CTX_use_certificate_file failed");

    if (SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM) != 1)
      die("SSL_CTX_use_PrivateKey_file failed");

    /* Make sure the key and certificate file match. */
    if (SSL_CTX_check_private_key(ctx) != 1)
      die("SSL_CTX_check_private_key failed");
    else
      printf("certificate and private key loaded and verified\n");
  }

  /* Recommended to avoid SSLv2 & SSLv3 */
  SSL_CTX_set_options(ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);
}

void ssl_client_init()
{
rbio = BIO_new(BIO_s_mem());
wbio = BIO_new(BIO_s_mem());
ssl = SSL_new(ctx);

//  if (mode == SSLMODE_SERVER)
    SSL_set_accept_state(ssl);  /* ssl server mode */
 // else if (mode == SSLMODE_CLIENT)
   // SSL_set_connect_state(p->ssl); /* ssl client mode */

  SSL_set_bio(ssl,rbio,wbio);

}


void print_unencrypted_data(char *buf, size_t len) {
  printf("%.*s", (int)len, buf);
}

int check_stat(int rtn)
{
int errcode, n;
char buf[1000];
errcode = SSL_get_error(ssl, rtn);
switch (errcode)
{  
case SSL_ERROR_NONE:
printf ("SSLSTATUS_OK;");
return 1;

case SSL_ERROR_WANT_WRITE:
printf ("SSL_ERROR_WANT_WRITE:\n");
}

if (errcode == SSL_ERROR_WANT_READ)
{
printf ("SSL_ERROR_WANT_READ:\n");
   
}


} // end check_stat  
int main ()
{
int servfd = socklisten(9999);
//printf ("d is : %d\n", servfd);
struct sockaddr_in peeraddr;
socklen_t peeraddr_len = sizeof(peeraddr);
int clientfd;
int buffmax = 100;


ssl_init("server.crt", "server.key"); // see README to create these files
int n;
int bytesread;
int errcode;
int bytesout;
int bytesin; 

char buffin[buffmax];
char decbuf[buffmax];

while (1)
{ // server loop

clientfd = accept (servfd, (struct sockaddr *)&peeraddr, &peeraddr_len);

//clientfd = accept4 (servfd, (struct sockaddr *)&peeraddr, &peeraddr_len, SOCK_NONBLOCK);
ssl_client_init();

bytesin = 1;
while (bytesin > 0)
{ // while reading from socket

bytesin = read (clientfd, buffin,  buffmax);
printf ("bytes from sock: %d\n", bytesin);

// clear ssl error que before any operation
//ERR_clear_error();
n = BIO_write(rbio, buffin, bytesin);
printf ("bytes to bio: %d\n", n);
if (n<=0)
break; /* assume bio write failure is unrecoverable */

// imported
if (!SSL_is_init_finished(ssl))
{
errcode = SSL_do_handshake(ssl);

if (errcode < 0)
	{printf ("fatal error\n"); break;}

if (errcode == 0)
	{printf ("recoverable error\n"); continue;}

if (!SSL_is_init_finished(ssl))
	{printf("fail on final handshake line\n"); continue;}
} // if ! init_finished


do {
n = SSL_read(ssl, decbuf, sizeof(decbuf));
if (n > 0)
print_unencrypted_data(decbuf ,(size_t) n);
//client.io_on_read(buf, (size_t)n);
} while (n > 0);

} // read loop



} // server loop
} // end main
