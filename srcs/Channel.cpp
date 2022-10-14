#include "../inc/Channel.hpp"

Channel::Channel()
{
    this->_channel_name = "";
    this->_num_clients = 0;
    std::memset(&this->_clients, 0, sizeof(this->_clients));
    std::memset(&this->_operators, 0, sizeof(this->_operators));
}

Channel::Channel(std::string name, int fd)
{
    this->_channel_name = name;
    this->_clients.push_back(fd);
    this->_operators.push_back(fd);
    this->_num_clients = 1;
}

Channel::~Channel(){}

void Channel::add_new_client(int fd)
{
    this->_clients.push_back(fd);
    this->_num_clients++;
}

void Channel::send_message_to_chan(std::string buffer, int sender)
{
    std::vector<int>::iterator start = this->_clients.begin();
    for (; start != this->_clients.end(); start++)
    {
        if (*start == sender)
            continue;
        send(*start, buffer.c_str(), buffer.size(), 0);
    }
}