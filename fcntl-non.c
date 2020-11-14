// where socketfd is the socket you want to make non-blocking
int status = fcntl(socketfd, F_SETFL, fcntl(socketfd, F_GETFL, 0) | O_NONBLOCK);

if (status == -1){
  perror("calling fcntl");
  // handle the error.  By the way, I've never seen fcntl fail in this way
}
