#include "../../inc/Server/Server.hpp"

int status;

void handler(int)
{
    status = 1;
}


Server::Server(int port, string server_name, string server_password, int max_client)
{
    this->_port = port;
    this->_server_name = server_name;
    this->_server_password = server_password;
    this->_max_client = max_client;
    FD_ZERO(&_current_sockets);
}

Server::~Server()
{
    //free ?
    //close ?
    close(this->_client);
    close(this->_server);
}

void Server::init(void)
{
    this->_server = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_server < 0)
    {
        cout << "Error while creating socket..." << endl;
        return ;
    }

    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->_port);

    int val = 1;
	setsockopt(this->_server, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));

    bind(this->_server, (struct sockaddr *)&server_addr, sizeof(server_addr));
    cout << "WAITING FOR CLIENT..." << endl;
    listen(this->_server, this->_max_client);
    FD_SET(this->_server, &_current_sockets);
}

void    Server::accept_new_user(void)
{
    int client;
    socklen_t size;
    //accepting new connection
    size = sizeof(this->client_addr);
    client = accept(this->_server, (struct sockaddr *)&client_addr, &size);
    //checking
    if (this->_client < 0)
        cout << "ERROR WHILE ACCEPTING" << endl;
    else
        cout << "CLIENT ACCEPTED" << endl << endl;
    //creating user and had him to a vector which contains all users
    std::string ip = inet_ntoa(this->client_addr.sin_addr);
    Client new_user(client, ip);
    this->_clients.push_back(new_user);
    FD_SET(client, &_current_sockets);
}

void    Server::handle_request(int fd)
{
    char buffer[1024];
    int bytes_read;

    bytes_read = recv(fd, buffer, 1024, 0);
    if (bytes_read <= 0)
        cout << "Error with the client";
    else
    {
        cout << buffer << endl;
    }

}

void    Server::execute(void)
{
    signal(SIGINT, handler);
    status = 0;
    while (status == 0)
    {
        this->_ready_sockets = this->_current_sockets;
        if (select(FD_SETSIZE, &_ready_sockets, NULL, NULL, NULL) < 0)
            return ;
        
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &_ready_sockets))
            {
                if (i == this->_server)
                    accept_new_user();
                else
                    handle_request(i);
            }
        }
    } 
}

void Server::printInfo(void)
{
    cout << this->_port << endl;
    cout << this->_server_name << endl;
    cout << this->_server_password << endl;
}