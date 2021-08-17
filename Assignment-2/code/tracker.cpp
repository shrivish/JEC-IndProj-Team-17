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
#include <vector>
#include <sstream>
#define MAX_CLIENT 10
#define Port 8000

using namespace std;
vector<string> onlineClients;
string getonlineClients(){
    string temp="";
    for(int i=0;i<onlineClients.size();i++){
        temp+=onlineClients[i]+":";  
    }
    temp[temp.size()-1]='\0';
    return temp;
}
vector<string> split(const char str[], char a)
{
    vector<string> res;
    stringstream ss(str);
    string temp;
    while (getline(ss, temp, a))
    {
        res.push_back(temp);
    }
    return res;
}
class clientdata
{
public:
    int socket, id, portNum;
    string name, uID;
    string password;
    string Getdetails(){
        return (this->uID+":"+to_string(this->portNum));

    }
    void registerClient()
    {

        //Name:Password:uId

        char msg[2048] = {0};
        recv(socket, msg, 2048, 0);
        vector<string> data = split(msg, ':');
        ofstream tracker("tracker.txt", ios::app);
        tracker << string(msg) + ":" + to_string(portNum) << endl;
        tracker.close();
        name = data[0];
        password = data[1];
        uID = data[2];
        cout<<name<<endl;
    }
    void loginClient(){
        char msgdata[2048]={0};
        recv(socket,msgdata,2048,0);
        vector<string> data = split(msgdata, ':');
        ifstream tracker("tracker.txt");
        stringstream trac;
        trac<<tracker.rdbuf();
        string temp,fulldata=trac.str();
        while(getline(trac,temp)){
            vector<string> rec = split(temp.c_str(),':');
            if(rec[2].compare(data[0])==0&&rec[1].compare(data[1])==0){
                this->name = rec[0];
                this->password = rec[1];
                this->uID = rec[2];
                tracker.close();
                ofstream tracker("tracker.txt");
                int pos=fulldata.find(rec[2],0)+rec[2].size()+1;
                fulldata.replace(pos,rec[3].size(),to_string(this->portNum));
                tracker<<fulldata;
                cout<<this->name;
                tracker.close();
            
            }
        }
    }
};
void auth_client(clientdata *client)
{
    int clientsocket = client->socket;
    char mssg[2048];
    send(clientsocket,"Register/Login",15,0);
    while (true)
    {   memset(mssg, 0, 2048);

        recv(clientsocket, mssg, 2048, 0);
        
        if (strcmp(mssg, "Register") == 0)
        {
            send(clientsocket, "format(Name:Password:uID)", 26, 0);
            client->registerClient();
            break;
        }
        else if(strcmp(mssg,"Login")==0){
            send(clientsocket,"Login Format(uId:Password)",27,0);
            client->loginClient();
            break;
        }
        else
        {
            send(clientsocket, "Retry......", 12, 0);
        }
    }
}

void *connection_handler(void *arg)
{

    clientdata *client = (clientdata *)arg;
    int clientSocket = client->socket;
    char data[4096];
    auth_client(client);
    onlineClients.push_back(client->uID);
    string str = "Welcome " + client->name + ",from the server";
    send(clientSocket, str.c_str(), str.size(), 0);

    while (true)
    {
        memset(data, 0, 2048);
        char Hello[2048] = "Hello ";
        int dataRecv = recv(clientSocket, data, 2048, 0);
        if (dataRecv < 0)
        {

            cout << "Connection issue\n";
            break;
        }
            if (dataRecv == 0)
            {
                cout << "The client dissconnected \n";
                break;
            }
        

        cout << "Recieved : " << data << endl;
        if(strcmp(data,"get online clients")==0){
            string store=getonlineClients();
            send(clientSocket,store.c_str(),store.size(),0);
        }else if(strcmp(data,"Get my details")==0){
            string store=client->Getdetails();
            send(clientSocket,store.c_str(),store.size(),0);
        }
        else{
        strcat(Hello, data);
        send(clientSocket, Hello, dataRecv + 10, 0);
        }
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

    pthread_t threads[MAX_CLIENT];
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(servSocket, (sockaddr *)&clientAddr, &clientSize);
        if (clientSocket < 0)
        {
            serverFile << "Problem with client connection\n";

            cout << "Problem with client connection\n";
            return -1;
        }
        // To get the user data
        char host[NI_MAXHOST] = {0};
        char serv[NI_MAXSERV] = {0};

        int result = getnameinfo((sockaddr *)&clientAddr, clientSize, host, NI_MAXHOST, serv, NI_MAXSERV, 0);

        if (result)
        {
            std::cout << host << " connecting on " << serv << std::endl
                      << std::endl;
        }
        else
        {
            inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
            std::cout << host << " connecting on " << ntohs(clientAddr.sin_port) << std::endl
                      << std::endl;
        }

        clientdata *a = new clientdata;
        a->socket = clientSocket;
        a->portNum = stoi(string(serv));
        pthread_create(&threads[i], NULL, connection_handler, (void *)a);
    }
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(servSocket);

    return 0;
}