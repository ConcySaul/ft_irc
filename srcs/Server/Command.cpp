#include "../../inc/Server/Server.hpp"

int status;

int     bad_char(std::string test)
{
    std::string allowed("#qwertyuiopasdfghjklzxcvbnm0123456789");
    size_t i = 0;
    size_t pos;

    for (; i < test.size(); i++)
    {
        if ((pos = allowed.find(test[i])) == std::string::npos)
        {
            return (1);
        }
    }
    return (0);
}

void    Server::pass_command(Parser &parser, int fd)
{   
    // parser.print_parsed_command();
    std::vector<Client>::iterator client = get_client_by_fd(fd);
    if (client == this->_clients.end()) //if the client isn't connected
    {
        std::string buffer(ERR_NOTREGISTERED(this->_server_name, "*"));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    else if (parser._command.size() != 2) //if the client isn't connected
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    else if (!client->_nickname.empty()) //if the user is already registered
    {
        std::string buffer(ERR_ALREADYREGISTERED(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    else if (parser._command[1] != this->_server_password) // if the password isn't correct
    {
        std::string buffer(ERR_PASSWDMISMATCH(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    client->_passed = 1;

}

void    Server::nick_command(Parser &parser, int fd)
{   
    // parser.print_parsed_command();
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client == this->_clients.end())//if the client isn't connected
    {
        std::string buffer(ERR_NOTREGISTERED(this->_server_name, ""));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    else if (client->_passed == 0)//if the password isn't set
    {
        std::string buffer(ERR_NOTREGISTERED(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    else if (parser._command.size() != 2) // if the nickname param is missing
    {
        std::string buffer(ERR_NONICKNAMEGIVEN(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    std::vector<Client>::iterator client_to_test = this->get_client_by_nick(parser._command[1]);

    if (client_to_test != this->_clients.end()) //if nick is already used
    {
        std::string buffer(ERR_NICKNAMEINUSE(this->_server_name, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;     
    }
	client->_nickname = parser._command[1];
}

void    Server::user_command(Parser &parser, int fd)
{
    // parser.print_parsed_command();
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);

	if (client == this->_clients.end())//if the client isn't connected
    {
        std::string buffer(ERR_NOTREGISTERED(this->_server_name, ""));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    else if (parser._command.size() != 5)//if some params are missing
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    else if (client->_user_name.empty() == 0)//the user already send the USER cmd, and so, is connected
    {
        std::string buffer(ERR_ALREADYREGISTERED(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

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
    if (parser._command.size() < 3)// if missing params
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    std::string chan = parser._command[1];

    if (chan[0] == '#')//CHANNEL MODE
    {
        this->channel_mode(parser, fd);
        return ;
    }
    if (chan[0] != '#')//USER MODE
    {
        std::string buffer(ERR_NOPERMFORHOST(this->_server_name, client->_nickname));//not required in the subject, so... fuck it
        send(fd, buffer.c_str(), buffer.size(), 0);
        return ;
    }
}

void Server::user_mode(Parser &parser, int fd)
{
    std::vector<Client>::iterator client = get_client_by_fd(fd);
    int flag = 0;
    size_t pos;

    if (parser._command.size() < 3)//missing params
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    std::vector<Client>::iterator client_to_change = get_client_by_nick(parser._command[1]);
    std::string mode = parser._command[2];

    if (client_to_change == this->_clients.end())//client to change doesn't exist
    {
        std::string buffer(ERR_NOSUCHNICK(this->_server_name, client->_user_name, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    else if (client->_nickname != parser._command[1]) // if the client try to change another client's mode
    {
        std::string buffer(ERR_USERSDONTMATCH(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    for (size_t i = 0; i < mode.length(); i++)
    {
        if (mode[i] == '-')
            flag = 0;
        else if (mode[i] == '+')
            flag = 1;
        else
        {
            if (flag == 1)// if mode is +
                client->_mode.push_back(mode[i]);
            else if (flag == 0)//if mode is -
            {
                if ((pos = client->_mode.find(mode[i])) != std::string::npos)
                    client->_mode.erase(pos, 1);
            }
        }
    }

    std::string buffer("");
    buffer.append(":").append(client->_nickname).append("!").append(client->_user_name).append("@").append(client->_ip).append(" MODE ").append(client->_nickname).append(" :").append(mode).append("\r\n");
    // cout << "SEND : " << endl;
    // cout << buffer << endl;
    send(fd, buffer.c_str(), buffer.size(), 0);
	buffer.clear();
}

void Server::channel_mode(Parser &parser, int fd)
{
    std::vector<Client>::iterator client = get_client_by_fd(fd);

    if (parser._command.size() < 3)//missing params
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    std::string chan_name = parser._command[1];
    chan_name.erase(0, 1);
    std::vector<Channel>::iterator channel = get_channel_by_name(chan_name);
    std::string mode = parser._command[2];
    int flag = 0;
    size_t pos;

    if (channel == this->_channels.end())//channel doesn't exist
    {
        std::string buffer(ERR_NOSUCHCHAN(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return; 
    }
    if (!channel->is_in_channel(client->_nickname))//client isn't in the chan
    {
        std::string buffer(ERR_CANNOTSENDTOCHAN(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;   
    }
    if (!channel->is_op(client->_nickname))//client isn't an chan operator
    {
        std::string buffer(ERR_CHANOPRIVSNEEDED(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;     
    }

    if (mode[0] != '-' && mode[0] != '+')
    {
        std::string buffer(ERR_BADMASK(this->_server_name, client->_nickname, mode));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;     
    }

    long unsigned int token = 3; // long unsigned int, so we can compare it with size_t 
    for (size_t i = 0; i < mode.length(); i++)
    {
        if (mode[i] == '-')
            flag = 0;
        else if (mode[i] == '+')
            flag = 1;
        else
        {
            if (flag == 1)// if mode is +
            {
                if (mode[i] == 'k' && token <= parser._command.size())//set a key for channel
                {
                    channel->_key = parser._command[token];
                    token++;
                }
                if (mode[i] == 'l' && token <= parser._command.size())//set a max amount of client for channel
                {
                    channel->_max_clients = std::atoi(parser._command[token].c_str());
                    cout << channel->_max_clients << endl;
                    token++;
                }
                if (mode[i] == 'o' && token <= parser._command.size())//add new operator
                {
                    std::vector<Client>::iterator client = this->get_client_by_nick(parser._command[token]);
                    if (client == this->_clients.end())//if the client doesn't exist
                    {
                        token++;
                        continue ;
                    }
                    channel->_operators.push_back(client->_nickname);
                    token++;
                }
                if (mode[i] == 'b' && token <= parser._command.size())//add new ban
                {
                    std::vector<Client>::iterator client = this->get_client_by_nick(parser._command[token]);
                    if (client == this->_clients.end())//if the client doesn't exist
                    {
                        token++;
                        continue ;
                    }
                    channel->_banned.push_back(client->_nickname);
                    token++;
                }
                if ((pos = channel->_mode.find(mode[i])) == std::string::npos)
                    channel->_mode.push_back(mode[i]);
            }
            else if (flag == 0)//if mode is -
            {
               if (mode[i] == 'o' && token <= parser._command.size())//delete operator
                {
                    std::vector<Client>::iterator client = this->get_client_by_nick(parser._command[token]);
                    if (client == this->_clients.end())
                    {
                        token++;
                        continue ;
                    }
                    channel->remove_operator(client->_nickname);
                    token++;
                }
               if (mode[i] == 'b' && token <= parser._command.size())//delete ban
                {
                    std::vector<Client>::iterator client = this->get_client_by_nick(parser._command[token]);
                    if (client == this->_clients.end())
                    {
                        token++;
                        continue ;
                    }
                    channel->remove_ban(parser._command[token]);
                    token++;
                }
                if ((pos = channel->_mode.find(mode[i])) != std::string::npos)
                    channel->_mode.erase(pos, 1);
            }
        }
    }
    std::string buffer("");
    buffer.append(":").append(client->_nickname).append("!").append(client->_user_name).append("@").append(client->_ip).append(" MODE ").append(parser._command[1]).append(" :").append(mode).append("\r\n");
    // cout << "SEND : " << endl;
    // cout << buffer << endl;
    send(fd, buffer.c_str(), buffer.size(), 0);
	buffer.clear();
}

void    Server::ping_command(Parser &parser, int fd)
{
    // parser.print_parsed_command();
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
	if (client == this->_clients.end())//if the client isn't connected
    {
        std::string buffer(ERR_NOTREGISTERED(this->_server_name, ""));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    else if (parser._command.size() < 2) // if the server to ping param is missing
    {
        std::string buffer(ERR_NOORIGIN(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    else if (parser._command[1] != this->_server_name)//if the server ping is not our server
    {
        std::string buffer(ERR_NOSUCHSERVER(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    std::string buffer("");
    buffer.append(":").append(parser._command[1]).append(" PONG ").append(parser._command[1]).append(" :").append(parser._command[1]).append("\r\n");
    send(fd, buffer.c_str(), buffer.size(), 0);
    buffer.clear();
}

void    Server::join_command(Parser &parser, int fd)
{
    // parser.print_parsed_command();
    std::vector<Client>::iterator client = get_client_by_fd(fd);
    size_t pos;
    if (parser._command.size() < 2)//not enough parameters
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    std::string chan_name = parser._command[1];

    if (chan_name[0] != '#' || bad_char(chan_name))
    {
        std::string buffer(ERR_BADMASK(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    chan_name.erase(0, 1);
    std::vector<Channel>::iterator channel = this->get_channel_by_name(chan_name);

    if (channel == this->_channels.end())//channel doesn't exist
    {
        cout << "CREATING NEW CHANNEL" << endl;
        Channel new_channel(chan_name, client->_nickname);
        // channel->_operators.push_back(client->_nickname);
        this->_channels.push_back(new_channel);
        std::string buffer("");
        buffer.append(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip + " JOIN :" + parser._command[1]).append("\r\n");
        buffer.append(RPL_TOPIC(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        buffer.clear();
        return ;
    }
    if (channel->is_banned(client->_nickname))
    {
        std::string buffer(ERR_BANNEDFROMCHAN(this->_server_name, client->_nickname, channel->_channel_name));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;   
    }
    if (channel->_mode.find('k') != std::string::npos)//if key is needed to join
    {
        if (parser._command.size() < 3)
        {
            std::string buffer(ERR_KEYSET(this->_server_name, client->_nickname, channel->_channel_name));
            send(fd, buffer.c_str(), buffer.size(), 0);
            return;  
        }
        else if (parser._command[2] != channel->_key)
        {
            std::string buffer(ERR_KEYSET(this->_server_name, client->_nickname, channel->_channel_name));
            send(fd, buffer.c_str(), buffer.size(), 0);
            return;  
        }
    }
    if ((pos = channel->_mode.find('l') != std::string::npos) && (channel->_max_clients == channel->_num_clients)) //channel is full
    {
        std::string buffer(ERR_CHANNELISFULL(this->_server_name, client->_nickname, channel->_channel_name));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;  
    }
    else 
    { // channel already exists, so we add the client to it
        channel->add_new_client(client->_nickname);
        std::string buffer("");
        buffer.append(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip + " JOIN :" + parser._command[1]).append("\r\n");
        buffer.append(RPL_TOPIC(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        buffer.clear();
        return ;
    }
    
}

void    Server::privmsg_command(Parser &parser, int fd)
{
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);

    if (parser._command.size() < 3) // missing param
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    std::string chan_name = parser._command[1];

    if (chan_name[0] != '#')
    {
        std::string buffer(ERR_BADMASK(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    chan_name.erase(0, 1);
    std::vector<Channel>::iterator channel = this->get_channel_by_name(chan_name);

    if (channel == this->_channels.end())//if channel doesn't exist
    {
        std::string buffer(ERR_NOSUCHCHAN(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    else if (!channel->is_in_channel(client->_nickname) && (channel->_mode.find("n") != std::string::npos))//the client isnt't in the channel and the n mode is enabled
    {
        std::string buffer(ERR_CANNOTSENDTOCHAN(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return; 
    }

    size_t i = 3;
    std::string buffer(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip + " PRIVMSG #" + chan_name + " :");
    buffer.append(parser._command[2].erase(0, 1));
    for (; i < parser._command.size(); i++)
        buffer.append(" " + parser._command[i]);
    buffer.append("\r\n");
    // cout << buffer << endl;
    this->send_to_chan(channel->_channel_name, buffer, fd);
    buffer.clear();
}

void    Server::kick_command(Parser &parser, int fd)
{
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);

    if (parser._command.size() < 3)//if missing param(s)
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    std::vector<Client>::iterator client_to_kick = this->get_client_by_nick(parser._command[2]);
    std::string chan_name = parser._command[1];
    chan_name.erase(0, 1);
    std::vector<Channel>::iterator channel = this->get_channel_by_name(chan_name);

    if ((client->_mode.find("o")) == std::string::npos) //the client is not a operator
    {
        std::string buffer(ERR_NOPRIVILEGES(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    if (client_to_kick == this->_clients.end())//client to kick doesn't exist
    {
        std::string buffer(ERR_NOSUCHNICK(this->_server_name, client->_nickname, parser._command[2]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    if (channel == this->_channels.end())//channel to kick doesn't exist
    {
        std::string buffer(ERR_NOSUCHCHAN(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    if (!channel->is_in_channel(client->_nickname))//the client isn't in the channel
    {
        std::string buffer(ERR_NOTONCHANNEL(this->_server_name, client->_nickname, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;  
    }
    if (!channel->is_in_channel(client_to_kick->_nickname))//the client to kick isn't in the channel
    {
        std::string buffer(ERR_NOTONCHANNEL(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;  
    }
    
    std::vector<std::string>::iterator token = parser._command.begin();
    std::string buffer(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip);
    for (; token != parser._command.end(); token++)
        buffer.append(" " + *token);
    buffer.append(" :" + client->_nickname);
    buffer.append("\r\n");
    this->send_to_chan(channel->_channel_name, buffer, fd);
    channel->remove_client(client_to_kick->_nickname);
    // channel->print!_chan_info();
}

void Server::part_command(Parser &parser, int fd)
{
    //check if the client is in the channel
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
    if (parser._command.size() < 2)
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    std::string chan_name = parser._command[1];

    if (chan_name[0] != '#')
    {
        std::string buffer(ERR_BADMASK(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }

    chan_name.erase(0, 1);
    std::vector<Channel>::iterator channel = this->get_channel_by_name(chan_name);

    if (channel == this->_channels.end())
    {
        std::string buffer(ERR_NOSUCHCHAN(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return;
    }
    else if (!channel->is_in_channel(client->_nickname))
    {
        std::string buffer(ERR_NOTONCHANNEL(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
        return; 
    }
    channel->remove_client(client->_nickname);
    std::string buffer(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip);
    buffer.append(" PART").append(" :" + parser._command[1]).append("\r\n");
    send(fd, buffer.c_str(), buffer.size(), 0);
}

void Server::quit_command(Parser &parser, int fd)
{
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
    std::string buffer("ERROR :Closing link: (" + client->_user_name + "@" + client->_ip + ") " + "[Quit: ");
    if (parser._command.size() == 2)
        buffer.append(parser._command[1] + "]");
    else
        buffer.append("leaving]");
    this->removeClient(fd);
    send(fd, buffer.c_str(), buffer.size(), 0);
	FD_CLR(fd, &_current_sockets);//clear the fd from de the fd_set and close it
	close(fd);
}

void Server::notice_command(Parser &parser, int fd)
{
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
    if (parser._command.size() < 3)// if missing param(s)
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }

    std::vector<Client>::iterator client_to_notice = this->get_client_by_nick(parser._command[1]);
    
    if (client_to_notice == this->_clients.end())//if the client to notice doesn't exist
    {
        std::string buffer(ERR_NOSUCHNICK(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    size_t i = 3;
    std::string buffer(":" + client->_nickname + "!" + client->_user_name + "@" + client->_ip + " NOTICE " + parser._command[1] + " :");
    buffer.append(parser._command[2].erase(0, 1));
    for (; i < parser._command.size(); i++)
        buffer.append(" " + parser._command[i]);
    buffer.append("\r\n");
    send(client_to_notice->_socket, buffer.c_str(), buffer.size(), 0);
}

void Server::oper_command(Parser &parser, int fd)
{
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
    std::vector<Client>::iterator client_to_upgrade = this->get_client_by_nick(parser._command[1]);
    if (parser._command.size() < 3)
    {
        std::string buffer(ERR_NEEDMOREPARAMS(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    else if (parser._command[2] != this->_operator_password)//password doesn't match
    {
        std::string buffer(ERR_PASSWDMISMATCH(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    else if (client_to_upgrade == this->_clients.end())//client to upgrade doesn't exist
    {
        std::string buffer(ERR_NOSUCHNICK(this->_server_name, client->_nickname, parser._command[1]));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    client->_mode.push_back('o');
    std::string buffer(RPL_YOUREOPER(this->_server_name, client->_nickname));
    send(fd, buffer.c_str(), buffer.size(), 0);
}

void Server::die_command(Parser &parser, int fd)
{
    (void)parser;
    std::vector<Client>::iterator client = this->get_client_by_fd(fd);
    if ((client->_mode.find("o")) == std::string::npos)//client us not an operator
    {
        std::string buffer(ERR_NOPRIVILEGES(this->_server_name, client->_nickname));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
    }
    else
        status = 1;
}

void Server::unknown_command(Parser &parser, int fd)
{
        std::vector<Client>::iterator client = this->get_client_by_fd(fd);
        std::string buffer(ERR_UNKNOWNCOMMAND(this->_server_name, client->_nickname, parser._command[0]));
        send(fd, buffer.c_str(), buffer.size(), 0);
		return ;
}