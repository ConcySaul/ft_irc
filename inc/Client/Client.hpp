#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Tools.hpp"
#include "../Parser.hpp"

class Client
{
    public:
        std::string _nickname;
        std::string _user_name;
        std::string _real_name;
        std::string _ip;
        int         _socket;
        
    public:
        Client(int socket, std::string ip);
        ~Client();

        void add_client_info(std::vector<std::string> parsed_query);                                                                   
};

#endif