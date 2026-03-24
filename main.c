#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>


#define print_check(fmt, ...) printf("\033[32m ✓\033[0m " fmt "\n", ##__VA_ARGS__)

// TODO: Make thread only for receiving data

typedef enum system_type{
        SERVER,
        CLIENT
}system_type;

typedef enum player_role{
        FOX   = 1,
        HOUND = 2
}player_role;

int create_server(int port){
        int sockfd;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1){
                perror("Socket creation failed.\n");
                exit(0);
        }
        print_check("Socket creation sucessful.");

        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;  // ipv4 addrs
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(port);

        if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0){
                perror("Socket bind failed.\n");
                exit(0);
        }
        char adr[INET_ADDRSTRLEN];
        print_check("Socket bind sucessful. [Addr: %s:%d]", inet_ntop(AF_INET, &(servaddr.sin_addr), adr, INET_ADDRSTRLEN), ntohs(servaddr.sin_port));

        if(listen(sockfd, 5) != 0){
                perror("Listen failed.\n");
                exit(0);
        }
        print_check("Socket listen sucessful.");
        
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
        print_check("Socket accept sucessful.");
        return connfd;
}

int create_client(){
        int sockfd;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd == -1){
                perror("Socket creation failed\n");
                exit(0);
        }
        return sockfd;
}

void connect_to_server(int sockfd, int port){
        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(port);

        char adr[INET_ADDRSTRLEN];
        printf("Trying to connect to %s:%d\n", inet_ntop(AF_INET, &(servaddr.sin_addr), adr, INET_ADDRSTRLEN), ntohs(servaddr.sin_port));
        if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0){
                perror("Connection with the server failed\n");
                exit(0);
        }
        print_check("Connected to server sucessfully.");
}

void init_board(int board[8][8]){
        for(int i = 7; i >= 0; i--){
                for(int j = 0; j < 8; j++){
                        board[i][j] = 0;
                }
        }

        board[0][4] = FOX;

        board[7][1] = HOUND;
        board[7][3] = HOUND;
        board[7][5] = HOUND;
        board[7][7] = HOUND;
}

void print_board(const int board[8][8]){
        printf("   A B C D E F G H\n");
        for(int i = 7; i >= 0; i--){
                printf(" %d ", i+1);
                for(int j = 0; j < 8; j++){
                        if(board[i][j] == 1){
                                printf("🦊");
                                continue;
                        }
                        if(board[i][j] == 2){
                                printf("🐶");
                                continue;
                        }
                        if(((i+j) % 2) == 0){
                                printf("⬛");
                        }else{
                                printf("⬜");
                        }
                }
                printf(" %d ", i+1);
                puts("");
        }
        printf("   A B C D E F G H\n");
}

int test_move(const player_role role, int board[8][8], char move[10]){
        int curr_x = move[0] - 'a';
        int curr_y = move[1] - '0' - 1;
        int new_x = move[3] - 'a';
        int new_y = move[4] - '0' - 1;
        if(role == FOX){
                new_x = curr_x;
                new_y = curr_y;
                for(int i = 0; i < 8; i++){
                        for(int j = 0; j < 8; j++){
                                if(board[i][j] == 1){
                                        curr_x = j;
                                        curr_y = i;
                                        break;
                                }
                        }
                }
        }
        if(role == HOUND){
                if(board[curr_y][curr_x] != HOUND){
                        printf("Must select a hound. Try again...\n");
                        return 0;
                }
        }

        printf("%d:%d -> %d:%d\n", curr_x, curr_y, new_x, new_y);

        if(new_y < 0 || new_y > 7 || new_x < 0 || new_x > 8){  // BOARD BOUNDS
                printf("Movement out of board. Try again...\n");
                return 0;
        }

        int delta_x = abs(new_x - curr_x);
        int delta_y = abs(new_y - curr_y);

        if((delta_x > 1) || (delta_y > 1)){                     // MOV DIST
                printf("Illegal move!\n");
                return 0;
        }
        if(delta_x + delta_y != 2){                             // DIAGONALS
                printf("Must move diagonally!\n");
                return 0;
        }
        if(role == HOUND){                                      // HOUND ONLY DOWN
                if(new_y > curr_y){
                        printf("Hound cant move up!\n");
                        return 0;
                }
        }

        return 1;
}

int move_fox(int board[8][8], char move[10]){
        int x = move[0] - 'a';
        int y = move[1] - '0' - 1;

        int curr_x, curr_y;
        for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                        if(board[i][j] == 1){
                                curr_x = j;
                                curr_y = i;
                                break;
                        }
                }
        }
        
        board[curr_y][curr_x] = 0;
        board[y][x] = 1;
        return 1;
}
void move_hound(int board[8][8], char move[10]){
        int curr_x = move[0] - 'a';
        int curr_y = move[1] - '0' - 1;
        int new_x = move[3] - 'a';
        int new_y = move[4] - '0' - 1;

        printf("(%d:%d) to (%d:%d)\n", curr_x, curr_y, new_x, new_y);

        board[curr_y][curr_x] = 0;
        board[new_y][new_x] = 2;
};

void game(player_role role, int connfd){
        int board[8][8];
        init_board(board);
        char move[10];

        while(1){
                print_board(board);
                switch(role){
                        case FOX:
                                do{
                                        printf("You: 🦊\n");
                                        printf("Next move: ");
                                        scanf("%s", move);
                                }while(test_move(FOX, board, move) == 0);
                                move_fox(board, move);
                                print_board(board);

                                write(connfd, &board, sizeof(board));
                                printf("Waiting for hound to move...\n");
                                read(connfd, &board, sizeof(board));
                                break;
                        case HOUND:
                                printf("You: 🐶\n");
                                printf("Waiting for fox to move...\n");
                                read(connfd, &board, sizeof(board));
                                print_board(board);

                                do{
                                        printf("You: 🐶\n");
                                        printf("Next move:");
                                        fgets(move, sizeof(move), stdin);
                                }while(test_move(HOUND, board, move) == 0);
                                move_hound(board, move);
                                write(connfd, &board, sizeof(board));
                                break;
                }
        }
}

int main(int argc, char* argv[]){
        if(argc != 3){
                puts("Usage: server/client <port>");
                return 0;
        }


        int sockfd;
        int connfd;
        system_type sys_type;
        player_role player_role;


        int port = atoi(argv[2]);

        if(strcmp(argv[1], "server") == 0){
                sys_type = SERVER;
        }else if(strcmp(argv[1], "client") == 0){
                sys_type = CLIENT;
        }else{
                puts("Usage: server/client <port>");
                return 0;
        }

        switch(sys_type){
                case SERVER:
                        sockfd = create_server(port);
                        connfd = accept_client(sockfd);

                        srand(time(NULL));
                        if(rand() %2 == 0){
                                player_role = FOX;
                        }else{
                                player_role = HOUND;
                        }
                        write(connfd, &player_role, sizeof(player_role));
                        break;
                case CLIENT:
                        connfd = create_client();
                        connect_to_server(connfd, port);
                        read(connfd, &player_role, sizeof(player_role));
                        if(player_role == FOX){
                                player_role = HOUND;
                        }else{
                                player_role = FOX;
                        }
                        break;
        }
                //char buffer[] = "hello client!";
                //write(connfd, buffer, sizeof(buffer));
                //
                //char buffer[14];
                //read(sockfd, buffer, sizeof(buffer));
                //printf("Received: %s\n", buffer);

// Connected ======================================
        game(player_role, connfd);

        close(sockfd);
        return 1;

}
