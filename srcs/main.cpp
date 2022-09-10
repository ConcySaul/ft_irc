#include "../inc/Server/Server.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;

int status;

void handler(int)
{
    status = 1;
}

int main()
{
    int     socketServer = socket(AF_INET, SOCK_STREAM, 0);

    struct  sockaddr_in addrServer;
    struct  sockaddr_in addrClient;

    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(10000);

    bind(socketServer, (struct sockaddr *)&addrServer, sizeof(addrServer));

    cout << "WAITING FOR CLIENT..." << endl;
    listen(socketServer, 2);

    socklen_t size = sizeof(addrClient);
    int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &size);
    if (socketClient < 0)
        cout << "ERROR WHILE ACCEPTING" << endl;
    else
        cout << "CLIENT ACCEPTED" << endl;

    while (socketClient)
    {
        char buf[1024];
        recv(socketClient, buf, 1024, 0);
        send(socketClient, buf, 1024, 0);
        cout << buf << endl;
    }
    close(socketClient);
    close(socketServer);
    return 0;
}