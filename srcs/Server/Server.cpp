#include "../../inc/Server/Server.hpp"

Server::Server()
{
    this->_port = 0;
    this->_server = 0;
    this->_ip_address = "";
    this->_server_name = "";
    this->_server_password = "";
    this->_socketfd = 0;
}

Server::Server(int port, string ip_adress, string server_name, string server_password)
{
    this->_port = port;
    this->_ip_address = ip_adress;
    this->_server_name = server_name;
    this->_server_password = server_password;
    this->_socketfd = 0;
}

Server::~Server()
{
    //free ?
    //close ?
}

int Server::init(void)
{
    //socket creation
    this->_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socketfd < 0)
    {
        cout << "Error while connecting..." << endl;
        return 0;
    }
    cout << "Socket connection created..." << endl;;
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    this->server_addr.sin_port = htons(this->_port);
    //binding socket
    if (bind(this->_socketfd, (struct sockaddr*)&server_addr, sizeof(this->server_addr)) < 0)
    {
        cout << "Error while binding socket..." << endl;
        return 0;
    }
    return (1);
}

void Server::printInfo(void)
{
    cout << this->_port << endl;
    cout << this->_ip_address << endl;
    cout << this->_server_name << endl;
    cout << this->_server_password << endl;
}