#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Tools.hpp"
#include "Client/Client.hpp"

class Channel
{
    public:
        std::string             _channel_name;
        std::vector<Client*>    _clients;
        std::vector<Client*>    _operators;
        int                     _num_clients;

    public:
        Channel();
        Channel(std::string name, Client *client);
        ~Channel();

        void add_new_client(Client *client);
        void remove_client(Client *client);
        void send_message_to_chan(std::string buffer, int sender);
        
        void print_chan_info(void);
};

#endif