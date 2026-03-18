#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <string.h>
#include <arpa/inet.h>


#define PORT 8080

// TODO: Make thread only for receiving data

int create_server(){
        int sockfd;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1){
                perror("Socket creation failed.\n");
                exit(0);
        }
        puts("Socket creation sucessful.");

        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;  // ipv4 addrs
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);

        if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0){
                perror("Socket bind failed.\n");
                exit(0);
        }
        //printf("Socket bind %s sucessful.", inet_ntop(AF_INET, &servaddr));

        if(listen(sockfd, 5) != 0){
                perror("Listen failed.\n");
                exit(0);
        }
        puts("Socket listen sucessful.");
        
        return sockfd;
}

int accept_client(int sockfd){
        int connfd;
        struct sockaddr_in cli;
        unsigned int len;

        len = sizeof(cli);

        puts("Waiting for connection...");
        connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
        if(connfd < 0){
                perror("Server accept failed.");
                exit(0);
        }
        puts("Socket accept sucessful.");
        return connfd;
}

int create_client(){
        int sockfd;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd == -1){
                perror("socket creation failed\n");
                exit(0);
        }
        return sockfd;
}

void connect_to_server(int sockfd){
        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(PORT);

        if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0){
                perror("connection with the server failed\n");
                exit(0);
        }
}

int main(int argc, char* argv[]){
        int sockfd;
        int connfd;

        if(argc != 3){
                puts("Usage: server/client <port>");
                return 0;
        }

        if(strcmp(argv[1], "server") == 0){
                sockfd = create_server();
                connfd = accept_client(sockfd);

                char buffer[] = "hello client!";
                write(connfd, buffer, sizeof(buffer));
        }else if(strcmp(argv[1], "client") == 0){
                sockfd = create_client();
                connect_to_server(sockfd);

                char buffer[14];
                read(sockfd, buffer, sizeof(buffer));
                printf("Received: %s\n", buffer);
        }else{
                puts("Usage: server/client <port>");
                return 0;
        }

// Connected ======================================

        close(sockfd);
        return 1;

}
