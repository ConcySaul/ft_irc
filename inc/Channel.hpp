#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Tools.hpp"
#include "Client/Client.hpp"

class Channel
{
    public:
        std::string         _channel_name;
        std::vector<int>    _clients;
        std::vector<int>    _operators;
        int                 _num_clients;

    public:
        Channel();
        Channel(std::string name, int fd);
        ~Channel();

        void add_new_client(int fd);
        void send_message_to_chan(std::string buffer, int sender);
};

#endif