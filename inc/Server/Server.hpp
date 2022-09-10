#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Tools.hpp"

#define MAX_CLIENT 5

class Server
{
    public:
		int 				_server;
        int 				_port;
        string				_ip_address;
        string 				_server_name;
		string 				_server_password;
		int					_socketfd; 
		struct sockaddr_in	server_addr;//for single client

	public :
		Server();
		Server(int port, string ip_adress, string server_name, string server_password);
		~Server();

		int init(void);

		int getSocketfd(void) { return this->_socketfd; };
        int getServer(void) { return this->_server; };
		void printInfo();

};

#endif