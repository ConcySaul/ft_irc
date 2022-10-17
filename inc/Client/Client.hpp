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
        std::string _mode;
        int         _socket;
        int         _passed;
        
    public:
        Client(int socket, std::string ip);
        ~Client();

        //getter
        int get_user_socket(void) { return (this->_socket); };
        //utils
        void    print_client_info(void);
                                                                      
};

#endif