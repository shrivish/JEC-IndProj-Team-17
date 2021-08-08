#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>

#define Port 8000

using namespace std;

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

    int clientSocket = accept(servSocket, (sockaddr *)&clientAddr, &clientSize);
    if (clientSocket < 0)
    {
        serverFile << "Problem with client connection\n";
        
        cout << "Problem with client connection\n";
        return -1;
    }

    //Exchanging data with the client

    char data[4096];
    while (true)
    {
        memset(data, 0, 2048);
        char Hello[2048] = "Hello ";
        int dataRecv = recv(clientSocket, data, 2048, 0);
        if (dataRecv < 0)
        {
            serverFile << "Connection issue\n";
            
            cout << "Connection issue\n";
            break;
            if (dataRecv == 0)
            {
                serverFile << "The client dissconnected \n";
                
                cout << "The client dissconnected \n";
                break;
            }
        }
        serverFile << "Recieved : " << data << endl;
        
        cout << "Recieved : " << data << endl;
        strcat(Hello, data);
        send(clientSocket, Hello, dataRecv + 10, 0);
    }
    close(clientSocket);
    close(servSocket);

    return 0;
}