#include "../../inc/Client/Client.hpp"

Client::Client(int socket, std::string ip)
{
    this->_socket = socket;
    this->_ip = ip;
}

Client::~Client()
{
}

// void    Client::add_client_info(std::vector<std::string> parsed_query)
// {
//     std::vector<std::string>::iterator it = parsed_query.begin();
//     it++;
//     cout << endl;
//     cout << *it << endl;
//     return ;
// }
