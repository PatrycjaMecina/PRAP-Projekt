#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc , char *argv[])
{
    int socket_desc , new_socket , c;
    struct sockaddr_in server , client;
    int read_size;
    //Utworz gniazdo
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    //Przygotuj addres gniazda
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 ); //ustawianie portu
    //Polacz deskrypor gniazda (gniazdo) z przypisanym adresem
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("Problem dolaczenia gniazda");
        //return -1;
    }
    //Nasluchiwanie na gniezdzie (serwer)
    listen(socket_desc , 3);
    puts("SERWER TCP: Czekam na polaczenie...");
    c = sizeof(struct sockaddr_in);
    int pid;
    while (1) {
        //Akceptuj polaczenie przychodzace
        new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (new_socket<0)
        {
            perror("Problem z akceptacja poloczenia");
        }
        puts("Polaczenie zaakceptowane");
        //close(socket_desc);
        pid = fork();
        if (pid < 0) {
            perror("ERROR in new process creation");
            _exit(1);
        }
        if (pid == 0) {
            char client_message[3000];  // bufor wiadomosci z poleceniami od klienta
            char message_result[3000];  // bufor odpowiedzi dla klienta
            sprintf(message_result, "Serwer odebral dane");
            while ((read_size = recv(new_socket, client_message, 3000, 0)) > 0) { //sprawdzanie czy przyszło polecenie do serwera
                puts(client_message);
                write(new_socket, message_result, strlen(message_result));  //wysylanie odpowiedzi do klienta
                memset (client_message, 0, sizeof (client_message));    // wypelnienie zerami bufora wiadomosci z polecenimi od klienta
            }
            if(read_size == 0) {
                puts("Klient sie rozlaczyl");
            }
        } else {
            close(new_socket);
        }
    }
}
