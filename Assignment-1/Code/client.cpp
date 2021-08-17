#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include<fstream>

#define Port 8002

using namespace std;

int main()
{   ofstream clientFile("client.txt");

    sockaddr_in servAddr = {
        .sin_family = AF_INET,
        .sin_port = htons(Port),

    };

    int servSocket = socket(AF_INET, SOCK_STREAM, 0);
    inet_pton(AF_INET,"0.0.0.0",&servAddr.sin_addr);

    connect(servSocket, (sockaddr *)&servAddr, sizeof(servAddr));

    char data[2048];
    char recieve[2048];

    clientFile << "Connetced to the server ! You can send message ! " << endl<< endl;

    cout << "Connetced to the server ! You can send message ! " << endl<< endl;

    while (true)
    {
        memset(data, 0, 2048);
        memset(recieve, 0, 2048);

        cin.getline(data,2048);

        if (data[0] == 'q' && data[1] == '\0')
        {
            break;
        }

        send(servSocket, data, strlen(data), 0);

        int dataRecv = recv(servSocket, recieve, 2048, 0);
        if (dataRecv < 0)
        { clientFile << "Check connection at your end \n ";
            
            cout << "Check connection at your end \n ";
            break;
        }
        if (dataRecv == 0)
        {
            clientFile<< "The server disconnected \n";
            
            cout << "The server disconnected \n";
            break;
        }
        clientFile << recieve << endl;
        
        cout << recieve << endl<< endl;
    }
    close(servSocket);
    return 0;
}