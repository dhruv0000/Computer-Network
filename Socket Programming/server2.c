// Dhruv Patel
// B18CSE012
// IITJ- CS321
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 256

// Give ansver to string arethmatic. Donst follow BDMAS
int eval(char *str)
{
    char *p;
    int lhs, rhs;
    char op;
    int r = 0;
    lhs = strtol(str, &p, 10);
    while(*p == ' ') p++;
    op = *p++;
    switch(op) {
        case '/': r = lhs / eval(p); break;
        case '*': r = lhs * eval(p); break;
        case '+': r = lhs + eval(p); break;
        case '-': r = lhs - eval(p); break;
        default: r = lhs;
    }
    return r;
}

int main(int argc, char *argv[]) {

    int sockfd, portnum;
    int pid;
    struct sockaddr_in server_addr;

    // Check input arguments
    if(argc < 2) {
        fprintf(stderr,"usage %s <server-port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Creating a file descriptor for the lstening socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        fprintf(stderr, "ERROR opening socket\n");
        exit(EXIT_FAILURE);
    }
    printf("Socket created: %d\n", sockfd);

    //Filling the structure describing the Internet socket address
    portnum = atoi(argv[1]);
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(portnum);

    // Binding the socket to the described Internet socket address
    if(bind(sockfd, (struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        fprintf(stderr, "Bind Error PORT: %d", portnum);
        exit(EXIT_FAILURE);
    }

    // Listening to accept connections on socket FD
    if(listen(sockfd, 5) < 0) {
        fprintf(stderr, "Listeing Error");
        exit(EXIT_FAILURE);
    }

    int server_addr_length = sizeof(server_addr);
    
    while(1){
        // When a connection arrives, open a new socket to communicate with it
        int clientfd = accept(sockfd, (struct sockaddr *)&server_addr, (socklen_t*)&server_addr_length);
        if(clientfd<0){
            fprintf(stderr, "Accept Err");
            exit(EXIT_FAILURE);
        }

        //Clone a seperate process
        pid = fork();
        if(pid<0){
            fprintf(stderr, "Fork Err");
            exit(EXIT_FAILURE);
        }

        //Child Process
        if(pid==0) {
            close(sockfd);
            int n;
            char inputbuf[MAX_INPUT_SIZE];
            while(read(clientfd, inputbuf, MAX_INPUT_SIZE) > 0)
            {
                printf("Input from Client %d: %s\n",clientfd, inputbuf);
                char outputbuf[MAX_INPUT_SIZE];
                sprintf(outputbuf, "%d", eval(inputbuf));
                printf("ans: %s\n", outputbuf);
                send(clientfd, outputbuf, strlen(outputbuf), 0);
                bzero(inputbuf,MAX_INPUT_SIZE);
            }
            exit(0);
        }
        //Parent Process
        else
        {
            close(clientfd);
        }

    }

}