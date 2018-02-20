#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

struct lista_wiazana {
    struct lista_wiazana *nastepny_element;
    int liczba;
};

int suma(struct lista_wiazana *lista);

int roznica(struct lista_wiazana *lista);

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
    struct lista_wiazana *listaWiazana;
    struct lista_wiazana *staraListaWiazana;
    staraListaWiazana = NULL;
    while (1) {
        //Akceptuj polaczenie przychodzace
        new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (new_socket<0)
        {
            perror("Problem z akceptacja poloczenia");
        }
        puts("Polaczenie zaakceptowane");
        //close(socket_desc);
        pid = fork(); //tworzenie nowego procesu, jezeli funkcja fork() zwróci 0 to jestesmy w procesie dziecka, jezeli zwróci wartosc > 0 to jestesmy w procesie macierzystym
        if (pid < 0) { // warunek sprawdzajacy czy udalo sie stworzyc nowy proces
            perror("ERROR in new process creation");
            _exit(1);
        }
        if (pid == 0) { // warunek sprawdzajacy czy jestesmy w procesie dziecka
            char client_message[3000];  // bufor wiadomosci z poleceniami od klienta
            char message_result[3000];  // bufor odpowiedzi dla klienta
            int liczba = 0;
            int wynik = 0;
            sprintf(message_result, "Serwer odebral dane");
            while ((read_size = recv(new_socket, client_message, 3000, 0)) > 0) { //sprawdzanie czy przyszło polecenie do serwera
                puts(client_message);
                liczba = atoi(client_message);  // zamiana odebranych danych na liczbe
                if(liczba != 0) {
                    listaWiazana = malloc (sizeof(struct lista_wiazana)); // zaalokowanie pamieci o rozmiarze listy wiazanej dla wskaznika
                    listaWiazana->liczba = liczba;  // przypisanie liczby do zmiennej w liscie wiazanej
                    listaWiazana->nastepny_element = staraListaWiazana; // wskazanie na kolejny element w liscie
                    staraListaWiazana = listaWiazana;
                }
                if (strstr(client_message, "suma") != NULL){    //sprawdzanie czy klient wyslal komende do zsumowania liczb
                    wynik = suma(listaWiazana);
                    snprintf(message_result, 3000, "Wynik sumowania: %d\n", wynik);
                    write(new_socket, message_result, strlen(message_result)); //wysylanie odpowiedzi do klienta
                } else if (strstr(client_message, "roznica") != NULL){  //sprawdzanie czy klient wyslal komende do odejmowania liczb
                    wynik = roznica(listaWiazana);
                    snprintf(message_result, 3000, "Wynik odejmowania: %d\n", wynik);
                    write(new_socket, message_result, strlen(message_result)); //wysylanie odpowiedzi do klienta
                } else if (client_message[0] == 't') {  //sprawdzanie czy klient wyslal litere potwierdzajaca zakonczenie dzialania
                    kill(getpid(), SIGKILL);    //zabicie procesu
                } else {
                    char message[] = "Potwierdzenie odebrania danych przez serwer\n";
                    write(new_socket, message, strlen(message));    //wysylanie odpowiedzi do klienta
                }
                //write(new_socket, message_result, strlen(message_result));  //wysylanie odpowiedzi do klienta
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

int suma(struct lista_wiazana *lista) {
    struct lista_wiazana *wskaznik = lista;
    int suma = 0;
    while (wskaznik != NULL) {
        suma = suma + wskaznik->liczba;
        wskaznik = wskaznik->nastepny_element;
    }
    return suma;
}

int roznica(struct lista_wiazana *lista) {
    struct lista_wiazana *wskaznik = lista;
    int suma = 0;
    while (wskaznik != NULL) {
        suma = suma - wskaznik->liczba;
        wskaznik = wskaznik->nastepny_element;
        if(wskaznik->nastepny_element == NULL) {
            suma = wskaznik->liczba + suma;
            break;
        }
    }
    return suma;
}
