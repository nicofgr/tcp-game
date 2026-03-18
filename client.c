#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/unistd.h>

#define PORT 8080

int main(){
        int sockfd;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd == -1){
                perror("socket creation failed\n");
                return 0;
        }

// ====================================

        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(PORT);

        if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0){
                perror("connection with the server failed\n");
                return 0;
        }
        
// Connected ==========================
        char buffer[14];
        read(sockfd, buffer, sizeof(buffer));
        printf("Received: %s\n", buffer);

        close(sockfd);
        return 1;
}
