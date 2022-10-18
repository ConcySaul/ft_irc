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
        Client                  *_first_op;
        std::string             _mode;
        std::string             _key;
        int                     _num_clients;
        int                     _max_clients;

    public:
        Channel();
        Channel(std::string name, Client *client);
        ~Channel();

        void add_new_client(Client *client);
        void remove_client(Client *client);
        void remove_operator(Client *client);
        void send_message_to_chan(std::string buffer, int sender);
        int  is_in_channel(std::string nick);
        int  is_op(std::string nick);
        
        void print_chan_info(void);
};

#endif