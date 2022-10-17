#include "../inc/Channel.hpp"

Channel::Channel()
{
    this->_channel_name = "";
    this->_num_clients = 0;
}

Channel::Channel(std::string name, Client *client)
{
    this->_channel_name = name;
    this->_clients.push_back(client);
    this->_operators.push_back(client);
    this->_num_clients = 1;
    this->_first_op = client;
    this->_mode = "";
}

Channel::~Channel(){}

void Channel::add_new_client(Client *client)
{
    this->_clients.push_back(client);
    this->_num_clients++;
}

void Channel::remove_client(Client *client)
{
    std::vector<Client*>::iterator start = this->_clients.begin();
    for (; start != this->_clients.end(); start++)
    {
        if ((*start)->_nickname == client->_nickname)
        {
            this->_clients.erase(start);
            break;
        }
    }
}

void Channel::send_message_to_chan(std::string buffer, int sender)
{
    (void)sender;
    std::vector<Client*>::iterator start = this->_clients.begin();
    for (; start != this->_clients.end(); start++)
    {
        if (sender != (*start)->_socket)
            send((*start)->_socket, buffer.c_str(), buffer.size(), 0);
    }
}

int Channel::is_in_channel(std::string nick)
{
    std::vector<Client*>::iterator client = this->_clients.begin();
    for (; client != this->_clients.end(); client++)
    {
        if ((*client)->_nickname == nick)
            return (1);
    }
    return (0);
}

int Channel::is_op(std::string nick)
{
    std::vector<Client*>::iterator op = this->_operators.begin();
    for (; op != this->_operators.end(); op++)
    {
        if ((*op)->_nickname == nick)
            return (1);
    }
    return (0);
}

void Channel::print_chan_info()
{
    std::vector<Client*>::iterator client = this->_clients.begin();
    std::vector<Client*>::iterator op = this->_operators.begin();

    cout << "CHANNEL NAME\t:\t" << this->_channel_name << endl;
    cout << "CLIENT CONNECTED\t:" << endl;
    for (; client != this->_clients.end(); client++)
        cout << (*client)->_nickname << endl;
    cout << endl;
    cout << "OPERATOR\t:" << endl;
    for (; op != this->_operators.end(); op++)
        cout << (*op)->_nickname << endl;
    cout << endl;
}