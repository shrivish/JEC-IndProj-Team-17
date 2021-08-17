#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#define MAX_CLIENT 10
#define Port 8001

using namespace std;
class Temp
{
public:
    int socket;
};

void *connection_handler(void *arg)
{

    Temp *num = (Temp *)arg;
    int clientSocket = num->socket;
    char data[4096];
    while (true)
    {
        memset(data, 0, 2048);
        char Hello[2048] = "Hello ";
        int dataRecv = recv(clientSocket, data, 2048, 0);
        if (dataRecv < 0)
        {

            cout << "Connection issue\n";
            break;
            if (dataRecv == 0)
            {

                cout << "The client dissconnected \n";
                break;
            }
        }

        cout << "Recieved : " << data << endl;
        strcat(Hello, data);
        send(clientSocket, Hello, dataRecv + 10, 0);
    }
    close(clientSocket);
    pthread_exit(NULL);
}

int main()
{
    ofstream serverFile("server.txt");

    // Creating socket

    int servSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (servSocket == -1)
    {
        serverFile << "Cannot create a socket \n";

        cout << "Cannot create a socket \n";
        return -1;
    }

    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(Port);
    inet_pton(AF_INET, "0.0.0.0", &servAddr.sin_addr);

    //Binding the server with the port

    if (bind(servSocket, (sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        serverFile << "Error in Binding \n";

        cout << "Error in Binding \n";
        return -1;
    }

    //Listening
    listen(servSocket, SOMAXCONN);
    serverFile << "Started listening to : " << Port << endl;

    cout << "Started listening to : " << Port << endl;

    sockaddr_in clientAddr;
    socklen_t clientSize = sizeof(clientAddr);

    pthread_t threads[MAX_CLIENT];
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        int clientSocket = accept(servSocket, (sockaddr *)&clientAddr, &clientSize);
        if (clientSocket < 0)
        {
            serverFile << "Problem with client connection\n";

            cout << "Problem with client connection\n";
            return -1;
        }

        Temp *a = new Temp;
        a->socket = clientSocket;
        pthread_create(&threads[i], NULL, connection_handler, (void *)a);
    }
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(servSocket);

    return 0;