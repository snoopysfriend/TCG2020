#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>
#include <poll.h>


void reselect(int timeout, int& flag) {

    //printf("waiting\n");
    struct pollfd pollfds;
    pollfds.fd = 0;
    pollfds.events = POLLIN;
    int n = poll(&pollfds, 1, timeout);
    //int n = select(maxfds, fds, NULL, NULL, 0);
    if (n == -1) {
        printf("select error");
        exit(-1);
    } else if (n > 0) {
        flag = 0;
        //int c = getchar();
        //putchar(c);
        //printf("good\n");
    }
    
}


/*
int main() {
    while (1) {
        reselect(3000);
    }
    return 0;
}*/
