#include "../inc/Channel.hpp"

Channel::Channel()
{
    this->_channel_name = "";
    this->_num_clients = 0;
}

Channel::Channel(std::string name, std::string client_nick)
{
    this->_channel_name = name;
    this->_clients.push_back(client_nick);
    this->_operators.push_back(client_nick);
    this->_num_clients = 1;
    this->_mode = "";
    this->_key = "";
    this->_max_clients = 99;
}

Channel::~Channel(){}

void Channel::add_new_client(std::string client_nick)
{
    this->_clients.push_back(client_nick);
    this->_num_clients++;
}

void Channel::remove_client(std::string client_nick)
{
    std::vector<std::string>::iterator start = this->_clients.begin();
    for (; start != this->_clients.end(); start++)
    {
        if (*start == client_nick)
        {
            this->_clients.erase(start);
            break;
        }
    }
    this->_num_clients--;
}

void Channel::remove_operator(std::string client_nick)
{
    std::vector<std::string>::iterator start = this->_operators.begin();
    for (; start != this->_operators.end(); start++)
    {
        if (*start == client_nick)
        {
            this->_operators.erase(start);
            break;
        }
    }
}

int Channel::is_in_channel(std::string nick)
{
    std::vector<std::string>::iterator client = this->_clients.begin();
    for (; client != this->_clients.end(); client++)
    {
        if (*client == nick)
            return (1);
    }
    return (0);
}

int Channel::is_op(std::string nick)
{
    this->print_chan_info();
    std::vector<std::string>::iterator op = this->_operators.begin();
    for (; op != this->_operators.end(); op++)
    {
        cout << *op << endl;
        if (*op == nick)
            return (1);
    }
    return (0);
}

int Channel::is_banned(std::string nick)
{
    std::vector<std::string>::iterator banned = this->_banned.begin();
    for (; banned != this->_banned.end(); banned++)
    {
        if (*banned == nick)
            return (1);
    }
    return (0);
}

void Channel::remove_ban(std::string client_nick)
{
    std::vector<std::string>::iterator banned = this->_banned.begin();
    for (; banned != this->_banned.end(); banned++)
    {
        if (*banned == client_nick)
        {
            this->_operators.erase(banned);
            break;
        }
    }
}

void Channel::print_chan_info()
{
    std::vector<std::string>::iterator client = this->_clients.begin();
    std::vector<std::string>::iterator op = this->_operators.begin();

    cout << "CHANNEL NAME\t:\t" << this->_channel_name << endl;
    cout << "CLIENT CONNECTED\t:" << endl;
    for (; client != this->_clients.end(); client++)
        cout << "1 " << *client << endl;
    cout << endl;
    cout << "OPERATOR\t:" << endl;
    for (; op != this->_operators.end(); op++)
        cout << " 2" <<*op << endl;
    cout << endl;
}