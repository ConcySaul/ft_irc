#include "../../inc/Client/Client.hpp"

Client::Client(int socket, std::string ip)
{
    this->_socket = socket;
    this->_passed = 0;
    this->_ip = ip;
}

Client::~Client()
{
}

void    Client::print_client_info(void)
{
    cout << "NICK\t:\t" << this->_nickname << endl;
    cout << "USER_NAME\t:\t" << this->_user_name << endl;
    cout << "REAL_NAME\t:\t" << this->_real_name << endl;
    cout << "IP\t:\t" << this->_ip << endl;
    cout << "SOCKET\t:\t" << this->_socket << endl << endl;
}
