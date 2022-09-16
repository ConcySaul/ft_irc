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

void    Server::exec_query(Parser &parser, int fd)
{
    int command_id = parser.get_command_id();
    //!!!!!MUST CHECK IF USER IS REGISTERED!!!!!
    switch (command_id)
    {
        case 0 :
            break;
        case 2 : //NICK COMMAND;
            nick_command(parser, fd);
            break;
        case 3 : //USER COMMAND;
            user_command(parser, fd);
            break;
    }
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
        std::string query(buffer);
        Parser parser;
        parser.parse_query(query);
        std::vector<std::string>::iterator  it = parser._parsed_query.begin();
        for (; it != parser._parsed_query.end(); it++)
        {
            parser.parse_command(*it);
            this->exec_query(parser, fd);
            parser._command.clear();
        }
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
                if (i == this->_server) //new connection
                    accept_new_user();
                else
                    handle_request(i);
            }
        }
    } 
}

void    Server::send_welcome(int fd)
{
    std::string nick_name;
    std::string user_name;
    std::string ip;
    std::vector<Client>::iterator it = this->_clients.begin();
    for(; it != this->_clients.end(); it++)
    {
        if (it->_socket == fd)
        {
            nick_name = it->_nickname;
            user_name = it->_user_name;
            ip = it->_ip;
            break;
        }

    }
}

//COMMAND
void    Server::nick_command(Parser &parser, int fd)
{   
    std::vector<Client>::iterator client = this->_clients.begin();
    for (; client != this->_clients.end(); client++)
    {
        if (client->_socket == fd)
        {
            client->_nickname = parser._command[1];
            break;
        }
    }
}

void    Server::user_command(Parser &parser, int fd)
{
    std::vector<Client>::iterator client = this->_clients.begin();
    for (; client != this->_clients.end(); client++)
    {
        if (client->_socket == fd)
        {
            client->_user_name = parser._command[1];
            client->_real_name = parser._command[4].erase(0, 1);
            this->send_welcome(fd);
            break;
        }
    }
}

//UTILS
void Server::printInfo(void)
{
    cout << this->_port << endl;
    cout << this->_server_name << endl;
    cout << this->_server_password << endl;
}

// Client Server::get_client_by_fd(int fd)
// {
 
// }