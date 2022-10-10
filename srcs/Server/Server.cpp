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
	this->_max_client++;
    FD_SET(client, &_current_sockets);
}

void    Server::exec_query(int fd, std::string command)
{
    Parser parser;
    parser.parse_command(command);
    int command_id = parser.get_command_id();
	std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client->_nickname.empty() && command_id > 2)
	{
		return ;
	}
    //!!!!!MUST CHECK IF USER IS REGISTERED!!!!!
    switch (command_id)
    {
        case 0 :
            break;
        // // case 1 : //PASS COMMAND;
        //     pass_command(parser, fd);					  
        //     break;
        case 2 : //NICK COMMAND;
            nick_command(parser, fd);					  
            break;
        case 3 : //USER COMMAND;
            user_command(parser, fd);
            break;
        case 4 : //MODE COMMAND;
            mode_command(parser, fd);
            break;
        case 5 : //PING COMMAND
            ping_command(parser, fd);
            break;
    }
}

void    Server::handle_request(int fd)
{
    char buffer[1024];
    int bytes_read;
    bytes_read = recv(fd, buffer, 1024, 0);
    if (bytes_read <= 0)
        return;
    else
    {
        std::string query(buffer);
		size_t b = 0;
		size_t c = 0;
		while ((b = query.find("\n")) != std::string::npos)
		{
			if ((c = query.find("\r\n")) != std::string::npos)
			{
				std::string	command(query.substr(0, c));
				query = query.substr(c + 2);				
				this->exec_query(fd, command);
				command.clear();
			}
			else
			{
				std::string	command2(query.substr(0, b));
				query = query.substr(b + 1);
				this->exec_query(fd, command2);
				command2.clear();
			}
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
                {
                    accept_new_user();
                    continue;
                }
                else
                {
                    handle_request(i);
                    break;
                }
            }
        }
    } 
}

void    Server::send_welcome(int fd)
{
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client == this->_clients.end())
		return ;
    std::string buffer(RPL_WELCOME(this->_server_name, client->_nickname, client->_user_name, client->_ip));
	// buffer.append(RPL_YOURHOST(this->_server_name, client->_nickname, "InspIRCd-3"));
	// buffer.append(RPL_CREATED(this->_server_name, client->_nickname, "08:04:20 Oct 10 2022"));
	// buffer.append(RPL_MYINFO(this->_server_name, client->_nickname, "InspIRCd-3", "iosw", "biklmnopstv "));
    send(fd, buffer.c_str(), buffer.size(), 0);
	buffer.clear();
}

//COMMAND
// void    Server::pass_command(Parser &parser, int fd)
// {   
//     parser.print_parsed_command();
//     std::vector<Client>::iterator client = get_client_by_fd(fd);
// 	if (this->_server_password != parser._command[1])
// 	{
// 		std::string buffer("WRONG PASSWORD");
// 		return ;
// 	}

// }

void    Server::nick_command(Parser &parser, int fd)
{   
    parser.print_parsed_command();
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client == this->_clients.end())
		return ;
	client->_nickname = parser._command[1];
}

void    Server::user_command(Parser &parser, int fd)
{
    parser.print_parsed_command();
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client == this->_clients.end())
		return ;
    client->_user_name = parser._command[1];
    client->_real_name = parser._command[4].erase(0, 1);
    this->send_welcome(fd);
}

void    Server::mode_command(Parser &parser, int fd)
{
    parser.print_parsed_command();
    std::vector<Client>::iterator client = this->_clients.begin();
    for (; client != this->_clients.end(); client++)
    {
        if (client->_nickname == parser._command[1])
        {
            std::string mode = parser._command[2];
            int flag = 0;
            size_t pos;
            for (size_t i = 0; i < mode.length(); i++)
            {
                if (mode[i] == '-')
                    flag = 0;
                else if (mode[i] == '+')
                    flag = 1;
                else
                {
                    if (flag == 1)
                        client->_mode.push_back(mode[i]);
                    else if (flag == 0)
                    {
                        while ((pos = client->_mode.find(mode[i])) != std::string::npos)
                        client->_mode.erase(pos, 1);
                    }
                }
            }
        }
        std::string buffer("");
        buffer.append(":").append(client->_nickname).append("!").append(client->_user_name).append("@").append(client->_ip).append(" MODE ").append(client->_nickname).append(" :").append(parser._command[2]).append("\r\n");
        send(fd, buffer.c_str(), buffer.size(), 0);
		buffer.clear();
        break;
    }
}

void    Server::ping_command(Parser &parser, int fd)
{
    parser.print_parsed_command();
    std::string buffer("");
    buffer.append(":").append(parser._command[1]).append(" PONG ").append(parser._command[1]).append(" :").append(parser._command[1]).append("\r\n");
    send(fd, buffer.c_str(), buffer.size(), 0);
}

//UTILS
void Server::printInfo(void)
{
    cout << this->_port << endl;
    cout << this->_server_name << endl;
    cout << this->_server_password << endl;
}

std::vector<Client>::iterator Server::get_client_by_fd(int fd)
{
    std::vector<Client>::iterator client = this->_clients.begin();
    for (; client != this->_clients.end(); client++)
	{
        if (client->_socket == fd)
			return client;
	}
	return this->_clients.end();
}