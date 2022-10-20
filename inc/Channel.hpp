#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Tools.hpp"
#include "Client/Client.hpp"

class Channel
{
    public:
        std::string                 _channel_name;
        std::vector<std::string>    _clients;
        std::vector<std::string>    _operators;
        std::vector<std::string>    _banned;
        std::string                 _mode;
        std::string                 _key;
        int                         _num_clients;
        int                         _max_clients;

    public:
        Channel();
        Channel(std::string name, std::string client_nick);
        ~Channel();

        void add_new_client(std::string client_nick);
        void remove_client(std::string client_nick);
        void remove_operator(std::string client_nick);
        void remove_ban(std::string client_nick);
        void send_message_to_chan(std::string buffer, int sender);
        int  is_in_channel(std::string nick);
        int  is_op(std::string nick);
        int  is_banned(std::string nick);
        
        void print_chan_info(void);
};

#endif