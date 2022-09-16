#include "../../inc/Client/Client.hpp"

Client::Client(int socket, std::string ip)
{
    this->_socket = socket;
    this->_ip = ip;
}

Client::~Client()
{
}

void    Client::print_client_info(void)
{
    cout << "NICK " << this->_nickname << endl;
    cout << "USER_NAME " << this->_user_name << endl;
    cout << "REAL_NAME " << this->_real_name << endl;
    cout << "IP " << this->_ip << endl;
    cout << "SOCKET " << this->_socket << endl;
}

// void    Client::add_client_info(std::vector<std::string> parsed_query)
// {
//     std::vector<std::string>::iterator it = parsed_query.begin();
//     it++;
//     cout << endl;
//     cout << *it << endl;
//     return ;
// }
