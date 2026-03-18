#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <string.h>


#define PORT 8080

// TODO: Make thread only for receiving data

void create_server(){

}
void create_client(){
}

int main(int argc, char* argv[]){

        if(argc != 3){
                puts("Usage: server/client <port>");
                return 0;
        }

        if(strcmp(argv[1], "server") == 0){
                create_server();
        }else if(strcmp(argv[1], "client") == 0){
                create_client();
        }else{
                puts("Usage: server/client <port>");
                return 0;
        }

// -----------------------------------

        int sockfd;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1){
                perror("Socket creation failed.\n");
                return 0;
        }
        puts("Socket creation sucessful.");

        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;  // ipv4 addrs
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);

        if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0){
                perror("Socket bind failed.\n");
                return 0;
        }
        //printf("Socket bind %s sucessful.", inet_ntop(AF_INET, &servaddr));

        if(listen(sockfd, 5) != 0){
                perror("Listen failed.\n");
                return 0;
        }
        puts("Socket listen sucessful.");


        int len;
        struct sockaddr_in cli;
        len = sizeof(cli);

        int connfd;
        puts("Waiting for connection...");
        connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
        if(connfd < 0){
                perror("Server accept failed.");
                return 0;
        }
        puts("Socket accept sucessful.");

        // Connected ======================================
        char buffer[] = "hello client!";
        write(connfd, buffer, sizeof(buffer));

        close(sockfd);
        return 1;

}
