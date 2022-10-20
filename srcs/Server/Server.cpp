#include "../../inc/Server/Server.hpp"

void handler(int)
{
    status = 1;
}

void ft_bzero(char *ptr, int len)
{
    for (int i = 0 ; i < len ; i++)
        ptr[i] = 0;
}

Server::Server(int port, string server_name, string server_password, int max_client)
{
    this->_server = 0;
    this->_client = 0;
    this->_port = port;
    this->_server_name = server_name;
    this->_server_password = server_password;
    this->_operator_password = "GiveMePowerBitch!";
    this->_max_client = max_client;
    this->_num_clients = 0;
    this->_allowed_chan_mode = "n";
    this->_allowed_client_mode = "ow";
    FD_ZERO(&_current_sockets);
    FD_ZERO(&_ready_sockets);
}

Server::~Server()
{
    clear_all_socks();
}

void Server::init(void)
{
    this->_server = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_server < 0)
    {
        cout << "Error while creating socket..." << endl;
        return ;
    }

    struct sockaddr_in	server_addr;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->_port);

    int val = 1;
	setsockopt(this->_server, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));

    if(bind(this->_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cout << "ERROR WHILE BINDING" << endl;
        return ;
    }
    cout << "WAITING FOR CLIENT..." << endl;
    listen(this->_server, 5);
    FD_SET(this->_server, &_current_sockets);
}

void    Server::accept_new_user(void)
{
    int client;
    socklen_t size;
    struct sockaddr_in	client_addr;
    //accepting new connection
    size = sizeof(client_addr);
    client = accept(this->_server, (struct sockaddr *)&client_addr, &size);
    //checking
    if (this->_client < 0)
        cout << "ERROR WHILE ACCEPTING" << endl;
    else
        cout << "CLIENT ACCEPTED" << endl << endl;
    //creating user and had him to a vector which contains all users
    std::string ip = inet_ntoa(client_addr.sin_addr);
    Client new_user(client, ip);
    this->_clients.push_back(new_user);
	this->_num_clients++;
    FD_SET(client, &_current_sockets);
    // this->printInfo();
}

void    Server::exec_query(int fd, std::string command)
{
    Parser parser;
    parser.parse_command(command);
    int command_id = parser.get_command_id();
	std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client->_nickname.empty() && command_id > 2)
    {
        std::string buffer(ERR_NOTREGISTERED(this->_server_name, ""));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    //!!!!!MUST CHECK IF USER IS REGISTERED!!!!!
    switch (command_id)
    {
        case 0 :
            break;
        case 1 : //PASS COMMAND;
            pass_command(parser, fd);					  
            break;
        case 2 : //NICK COMMAND; (set a nickname)
            nick_command(parser, fd);					  
            break;
        case 3 : //USER COMMAND; (ask for a connection, set the username, realname a IP)
            user_command(parser, fd);
            break;
        case 4 : //MODE COMMAND;
            mode_command(parser, fd);
            break;
        case 5 : //PING COMMAND
            ping_command(parser, fd);
            break;
        case 6 : //JOIN COMMAND (join a channel)
            join_command(parser, fd);
            break;
        case 7 : //PRIVMSG COMMAND (send a message to a channel)
            privmsg_command(parser, fd);
            break;
        case 8 : //KICK COMMAND //!!!!OPERATOR COMMAND!!!!!
            kick_command(parser, fd);
            break;
        case 9 : //PART COMMAND (leave a channel)
            part_command(parser, fd);
            break;
        case 10 : //QUIT COMMAND (quit the server)
            quit_command(parser, fd);
            break;
        case 11 : //NOTICE COMMAND (notice a single client)
            notice_command(parser, fd);
            break;
        case 12 : //OPER_COMMAND (Upgrade a client to operator)                         
            oper_command(parser, fd);                                          
            break;
        case 13 : //KILL_COMMAND (remove a client from server)                            __
            kill_command(parser, fd);                        //                            |    SERV OPERTOR ONLY
            break;                                           //                            |
        case 14 : //DIE_COMMAND (turn off server)                                          |
            die_command(parser, fd);                         //                            |
            break;                                           //
        default :
            unknown_command(parser, fd);
            break;
    }
}

void    Server::handle_request(int fd)
{
    char buffer[1024];
    ft_bzero(buffer, 1024);
    int bytes_read;
    bytes_read = recv(fd, buffer, 1024, 0);
    if (bytes_read <= 0)
    {
        // this->get_client_by_fd(fd);
        this->removeClient(fd);
        return ;
    }
    else
    {
        std::string query(buffer);
        cout << "QUERY :" << endl << query << endl;
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
        
        for (int i = 0; i < FD_SETSIZE + 1; i++)
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
        // this->printInfo();
    }
}

void    Server::send_welcome(int fd)
{
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client == this->_clients.end())
		return ;
    std::string buffer(RPL_WELCOME(this->_server_name, client->_nickname, client->_user_name, client->_ip));
	buffer.append(RPL_YOURHOST(this->_server_name, client->_nickname, "IRCOKE.V1"));
	buffer.append(RPL_CREATED(this->_server_name, client->_nickname, "27:04:27 AUG 38 2158"));
	buffer.append(RPL_MYINFO(this->_server_name, client->_nickname, "IRCOKE.V1", "oi", "nblok"));
    // cout << "SEND : " << endl;
    // cout << buffer << endl;
    send(fd, buffer.c_str(), buffer.size(), 0);
	buffer.clear();
}

//COMMAND


//UTILS

void Server::printInfo(void)
{
    cout << "SERVER PORT : " << this->_port << endl;
    cout << "SERVER NAME : " << this->_server_name << endl;
    cout << "SERVER PASS : " << this->_server_password << endl;
    cout << "CONNECTED CLIENTS : " << this->_num_clients << endl << endl;
}

std::vector<Client>::iterator Server::get_client_by_nick(std::string nick)
{
    std::vector<Client>::iterator client = this->_clients.begin();
    for (; client != this->_clients.end(); client++)
	{
        if (client->_nickname == nick)
			return client;
	}
	return this->_clients.end();
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

std::vector<Channel>::iterator Server::get_channel_by_name(std::string name)
{
    std::vector<Channel>::iterator channel = this->_channels.begin();
    for (; channel != this->_channels.end(); channel++)
	{
        if (channel->_channel_name == name)
			return channel;
	}
	return this->_channels.end();
}

void    Server::removeClient(int fd)
{
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
    if (client == this->_clients.end())
        return ;
    FD_CLR(fd, &_current_sockets);
    close(fd);
    this->_clients.erase(client);
    this->_num_clients--;
}

void Server::printAllUser()
{
    std::vector<Client>::iterator client = this->_clients.begin();
    for (; client != this->_clients.end(); client++)
        client->print_client_info();
}

void Server::clear_all_socks()
{
    close(this->_server);
	for (int i = 0; i < 1024; i++)
	{
		if (FD_ISSET(i, &_current_sockets))
		{
			FD_CLR(i, &_current_sockets);
			close(i);
		}
	}
	FD_ZERO(&_current_sockets);
}

void    Server::send_to_chan(std::string chan, std::string buffer, int fd)
{
    std::vector<Client>::iterator client = get_client_by_fd(fd);
    std::vector<Channel>::iterator channel = get_channel_by_name(chan);
    std::vector<std::string>::iterator start = channel->_clients.begin();
    for (; start != channel->_clients.end(); start++)
    {
        std::vector<Client>::iterator client2 = get_client_by_nick((*start));
        if ( client->_socket != client2->_socket)
            send(client2->_socket, buffer.c_str(), buffer.size(), 0);
    }
}