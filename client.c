//#include <stdio.h>
//#include <stdlib.h>
//#include <errno.h>
//#include <string.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <unistd.h>
//
//const char* REQUEST_FIFO = "/home/cristi/CLionProjects/tema1/fifoReq";
//const char* RESPONSE_FIFO = "/home/cristi/CLionProjects/tema1/fifoResp";
//#define BUFFER_SIZE 2048
//
//int main() {
//    int req, resp;
//    char buffer[BUFFER_SIZE] = {};
//
////    mkfifo(REQUEST_FIFO, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
////    mkfifo(RESPONSE_FIFO, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
//
//    if((req = open(REQUEST_FIFO, O_WRONLY))==-1){
//        perror("[client]: eroare conectare la server\n");
//        exit(-1);
//    }
//
//    if((resp = open(RESPONSE_FIFO, O_RDONLY))==-1){
//        perror("[client]: eroare conectare la server\n");
//        exit(-1);
//    }
//
//    printf("[client]: conectat la server cu succes.\n");
//
//    while(1){
//        printf(">");
//        fgets(buffer, BUFFER_SIZE, stdin);
//        buffer[strlen(buffer)-1] = '\0';
//        printf("[c]: %s\n", buffer);
//        write(req, buffer, strlen(buffer));
//        bzero(buffer, strlen(buffer));
//        read(resp, buffer, BUFFER_SIZE);
//        printf("[client]: %s\n", buffer);
//        if(strstr(buffer, "parasit")!=NULL){
//            break;
//        }
//        bzero(buffer, BUFFER_SIZE);
//        fflush(stdin);
//    }
//    close(req);
//    close(resp);
//}
