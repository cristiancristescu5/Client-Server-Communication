#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "user.h"
#include <sys/socket.h>
#include <sys/wait.h>
#include <utmp.h>

const char* FIFO_REQUEST = "/home/cristi/CLionProjects/tema1/fifoReq";
const char* FIFO_RESPONSE = "/home/cristi/CLionProjects/tema1/fifoResp";
const char* USER_DB = "/home/cristi/CLionProjects/tema1/users.txt";

#define BUFFER_SIZE 2048
#define NUM_USERS 100
int numUsers = 0;

int isLogged;

User users[NUM_USERS];


int loginUser(char user[]){//logarea unui user prin compararea cu userDB
    FILE *f = fopen(USER_DB, "r");
    char usr[100];
    while(fgets(usr, 100, f) != NULL){
        usr[strcspn(usr, "\n")]='\0';
        if(strcmp(usr, user)==0 && numUsers < NUM_USERS){
            User user1;
            strcpy(user1.name, usr);
            users[numUsers] = user1;
            numUsers++;
            fclose(f);
            isLogged = 1;
            return 1;
        }
    }
    fclose(f);
    isLogged = 0;
    return 0;
}

char* getLoggedUsers(){
    char *buffer = (char *)malloc(BUFFER_SIZE);
    struct utmp *ut;
    setutent();
    while ((ut = getutent())!= NULL){
        if(ut->ut_type == USER_PROCESS){
            char info[100] = {'\0'};
            sprintf(info, "user: %s, host: %s, time: %d",
                    ut->ut_user, ut->ut_host, ut->ut_tv.tv_sec);
            strcat(buffer, info);
        }
    }
    endutent();
    return buffer;
}

char* getProcInfo(char* pid){
    int proc;
    proc = atoi(pid);
    char filePath[30] = {'\0'};
    sprintf(filePath, "/proc/%d/status", proc);
    char* buff = (char*) malloc(BUFFER_SIZE);
    FILE *f = fopen(filePath, "r");
    char line[100] = {'\0'};
    while(fgets(line, 100, f)!=NULL){
        if(strstr(line, "Name")!= NULL ||
        strstr(line, "State")!= NULL ||
        strstr(line , "PPid")!= NULL ||
        strstr(line, "Uid")!= NULL ||
        strstr(line, "VmSize") != NULL) {

            line[strcspn(line, "\n")] = '\0';
            strcat(buff, line);
            strcat(buff, "\n");

        }
    }
    buff[strlen(buff)-1]='\0';
    fclose(f);
    bzero(line, 100);
    return buff;
}

char** parseRequest(char* request){
    char** parsed = (char**)malloc(5 * sizeof(char *));
    if(parsed == NULL){
        perror("[server]: eroare parse\n");
        exit(-1);
    }
    char *p = strtok(request, " ");
    int i = 0;
    while(p != NULL && i != 3){
        parsed[i]= strdup(p);
        p = strtok(NULL, " ");
        i++;
    }
    parsed[i] = NULL;
    return parsed;
}

int getRequestSize(char** req){
    int num = 0;
    for(int i = 0 ; req[i] != NULL; i++){
        num ++;
    }
    return num;
}

char* handleRequests(char* request){
    int fd[2];
    char buff[BUFFER_SIZE] = {'\0'};

    if(pipe(fd)==-1){
        perror("Eroare pipe");
        exit(1);
    }

    pid_t pid = fork();

    if(pid == -1) {
        perror("Eroare fork\n");
        exit(1);
    }

    if(pid > 0){
        close(fd[1]);
        wait(NULL);
        read(fd[0], buff, BUFFER_SIZE);
        close(fd[0]);
        if(strstr(buff, "logat")!= NULL && strstr(buff, "succes")!= NULL){
            isLogged = 1;
        }
        if(strstr(buff, "delogat") != NULL){
            isLogged = 0;
        }
        char size[20] = {'\0'};
        char newBuff[BUFFER_SIZE] = {'\0'};
        sprintf(size, "%lu", strlen(buff));
        strcat(newBuff, "[");
        strcat(newBuff, size);
        strcat(newBuff, "] ");
        strcat(newBuff, buff);
        bzero(buff, BUFFER_SIZE);
        bzero(size, 20);
        return strdup(newBuff);

    }else{
        close(fd[0]);
        int executedCommand = 0;
        char** req = parseRequest(request);
        int size = getRequestSize(req);
        if((size > 3 || size == 0)){
            strcat(buff, "Comanda invalid.");
            executedCommand = 1;
        }
        if(strcmp(req[0], "login") == 0 && executedCommand == 0 && size == 3 && strcmp(req[1], ":")==0){
            printf("isLogged: %d\n", isLogged);
            if(isLogged==0){
                if(loginUser(req[2])==1){
                    strcpy(buff, "Utilizator logat cu succes.");
                    isLogged = 1;
                }else{
                    strcpy(buff, "Utilizator inexistent.");
                }
            }else{
                strcpy(buff, "Utilizator deja logat.");
            }
            executedCommand = 1;
        }
        if(strcmp(req[0], "get-proc-info")==0 && executedCommand == 0 && size == 3 && strcmp(req[1], ":") == 0) {
            if (isLogged == 1) {
                strcpy(buff, getProcInfo(req[2]));
            }else{
                strcpy(buff, "Trebuie sa te loghezi inainte de a putea executa comanda get-proc-info.");
            }
            executedCommand = 1;

        }

        if(strcmp(req[0], "logout") == 0 && size == 1 && executedCommand == 0){
            if(isLogged == 1){
                strcpy(buff, "Utilizator delogat cu succes");
            }else{
                strcpy(buff, "Utilizatorul nu este logat pe server");
            }
            executedCommand = 1;
        }

        if(strcmp(req[0], "quit") == 0 && size == 1 && executedCommand == 0){
            strcpy(buff ,"Ai parasit serverul");
            executedCommand = 1;
        }
        if(strcmp(req[0], "get-logged-users") == 0 && size == 1 && executedCommand == 0){
            if(isLogged == 1){
                strcpy(buff, getLoggedUsers());
            }else{
                strcpy(buff, "Trebuie sa te loghezi inainte de a putea executa comanda get-logged-users.");
            }
            executedCommand = 1;
        }

        if(executedCommand == 0){
            strcpy(buff, "Comanda invalida.");
            executedCommand = 1;
        }
        printf("response: %s\n", buff);

        write(fd[1], buff, strlen(buff));

        for(int i = 0 ; i < size ; i++){
            free(req[i]);
        }
        free(req);

        close(fd[1]);

        exit(0);
    }
}

int main(){
    char buffer[BUFFER_SIZE] = {'\0'};
    int resp, req;
    int sockets[2];
    isLogged = 0;
    pid_t pid;

    mkfifo(FIFO_REQUEST, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    mkfifo(FIFO_RESPONSE, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    printf("[server]: am creat fifo\n");

    if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets)==-1){
        perror("[server] eroare creare socket\n");
        exit(-1);
    }

    if((req = open(FIFO_REQUEST, O_RDONLY))==-1){
        perror("[server]: eroare la deschidere FIFO.\n");
        exit(-1);
    }

    if((resp = open(FIFO_RESPONSE, O_WRONLY))==-1){
        perror("[server]: eroare la deschidere fifo.\n");
        exit(-1);
    }

    printf("[server]: server pornit\n");
    pid = fork();
    if(pid == -1){
        perror("[server]: eroare fork\n");
        exit(1);
    }else if(pid == 0){
        close(sockets[1]);
    }else {
        close(sockets[0]);
    }

    while(1){
        if(pid == 0){

            read(sockets[0], buffer, BUFFER_SIZE);//read din socket

            char* response = handleRequests(buffer);

            write(sockets[0], response, strlen(response));//write in socket

            memset(buffer, '\0',BUFFER_SIZE);

            free(response);
        }else{
            // citire request
            read(req, buffer, BUFFER_SIZE);

            printf("[server]: de la client: %s\n", buffer);

            // transmitere request catre copil
            write(sockets[1], buffer, BUFFER_SIZE);


            //citire response
            memset(buffer, '\0', BUFFER_SIZE);
            read(sockets[1], buffer, BUFFER_SIZE);

            printf("[server]: de la copil : %s\n", buffer);

            //transmitere response la client
            write(resp, buffer, strlen(buffer));

            memset(buffer, '\0', BUFFER_SIZE);
        }

    }
}