#include "../../inc/Server/Server.hpp"

int status;

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
    this->_max_client = max_client;
    this->_num_clients = 0;
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
		return ;
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
        case 6 : //JOIN COMMAND
            join_command(parser, fd);
            break;
        case 7 : //PRIVMSG COMMAND
            privmsg_command(parser, fd);
            break;
        case 8 : //KICK COMMAND
            kick_command(parser, fd);
            break;
        // case 9 : //WHO COMMAND
        //     who_command(parser, fd);
        //     break;
        case 10 : //KICK COMMAND
            part_command(parser, fd);
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
        return;
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
	buffer.append(RPL_YOURHOST(this->_server_name, client->_nickname, "InspIRCd-3"));
	buffer.append(RPL_CREATED(this->_server_name, client->_nickname, "08:04:20 Oct 10 2022"));
	buffer.append(RPL_MYINFO(this->_server_name, client->_nickname, "InspIRCd-3", "iosw", "biklmnopstv "));
    // cout << "SEND : " << endl;
    // cout << buffer << endl;
    send(fd, buffer.c_str(), buffer.size(), 0);
	buffer.clear();
}

//COMMAND
// void    Server::pass_command(Parser &parser, int fd)
// {   
    // parser.print_parsed_command();
//     std::vector<Client>::iterator client = get_client_by_fd(fd);
// 	if (this->_server_password != parser._command[1])
// 	{
// 		std::string buffer("WRONG PASSWORD");
// 		return ;
// 	}

// }

void    Server::nick_command(Parser &parser, int fd)
{   
    // parser.print_parsed_command();
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client == this->_clients.end())
		return ;
	client->_nickname = parser._command[1];
}

void    Server::user_command(Parser &parser, int fd)
{
    // parser.print_parsed_command();
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client == this->_clients.end())
		return ;
    client->_user_name = parser._command[1];
    client->_real_name = parser._command[4].erase(0, 1);
    this->send_welcome(fd);
    // this->printAllUser();
}

void    Server::mode_command(Parser &parser, int fd)
{
    //check if the client exist
    // parser.print_parsed_command();
    std::vector<Client>::iterator client = get_client_by_fd(fd);
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
        std::string buffer("");
        buffer.append(":").append(client->_nickname).append("!").append(client->_user_name).append("@").append(client->_ip).append(" MODE ").append(client->_nickname).append(" :").append(parser._command[2]).append("\r\n");
        // cout << "SEND : " << endl;
        // cout << buffer << endl;
        send(fd, buffer.c_str(), buffer.size(), 0);
		buffer.clear();
}

void    Server::ping_command(Parser &parser, int fd)
{
    // parser.print_parsed_command();
    std::string buffer("");
    buffer.append(":").append(parser._command[1]).append(" PONG ").append(parser._command[1]).append(" :").append(parser._command[1]).append("\r\n");
    // cout << "SEND : " << endl;
    // cout << buffer << endl;
    send(fd, buffer.c_str(), buffer.size(), 0);
    buffer.clear();
}

void    Server::join_command(Parser &parser, int fd)
{
    // parser.print_parsed_command();
    std::string chan_name = parser._command[1];
    chan_name.erase(0, 1);
    std::vector<Client>::iterator client = get_client_by_fd(fd);
    std::vector<Channel>::iterator channel = this->get_channel_by_name(chan_name);
    if (channel == this->_channels.end()) // channel doesn't exist, so we create it
    {
        cout << "CREATING NEW CHANNEL" << endl;
        Channel new_channel(chan_name, client.base());
        this->_channels.push_back(new_channel);
        // new_channel.print_chan_info();
    }
    else
    { // channel already exists, so we add the client to it
        channel->add_new_client(client.base());
        // channel->print_chan_info();
    }
    std::string buffer("");
    buffer.append(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip + " JOIN :" + parser._command[1]).append("\r\n");
    send(fd, buffer.c_str(), buffer.size(), 0);
    buffer.clear();
    
}

void    Server::privmsg_command(Parser &parser, int fd)
{
    //check if the chan exist
    std::string chan_name = parser._command[1];
    chan_name.erase(0, 1);
    std::vector<Channel>::iterator channel = this->get_channel_by_name(chan_name);
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
    size_t i = 3;
    std::string buffer(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip + " PRIVMSG #" + chan_name + " :");
    buffer.append(parser._command[2].erase(0, 1));
    for (; i < parser._command.size(); i++)
        buffer.append(" " + parser._command[i]);
    buffer.append("\r\n");
    cout << buffer << endl;
    channel->send_message_to_chan(buffer, fd);
    buffer.clear();
}

void    Server::kick_command(Parser &parser, int fd)
{
    //check if the client is operator
    //check if the client is in the chan
    //check if the client to kick is in the chan and exists
    //check if the chan exists
    //kick multiple client : /KICK user1,user2 chan :message
    std::string chan_name = parser._command[1];
    chan_name.erase(0, 1);
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
    std::vector<Client>::iterator client_to_kick = this->get_client_by_nick(parser._command[2]);
    std::vector<Channel>::iterator channel = this->get_channel_by_name(chan_name);
    std::vector<std::string>::iterator token = parser._command.begin();
    channel->remove_client(client_to_kick.base());
    std::string buffer(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip);
    for (; token != parser._command.end(); token++)
        buffer.append(" " + *token);
    buffer.append("\r\n");
    channel->send_message_to_chan(buffer, fd);
    // channel->print_chan_info();
}

void Server::part_command(Parser &parser, int fd)
{
    //check if client exists
    //check if the channel exist
    //check if the client is in the channel
    std::string chan_name = parser._command[1];
    chan_name.erase(0, 1);
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
    std::vector<Channel>::iterator channel = this->get_channel_by_name(chan_name);
    channel->remove_client(client.base());
    std::string buffer(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip);
    buffer.append(" PART").append(" :" + parser._command[1]).append("\r\n");
    cout << buffer << endl;
    send(fd, buffer.c_str(), buffer.size(), 0);
}

////////////////////////////////////////////UTILS
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