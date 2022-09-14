#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Tools.hpp"
#include "../Client/Client.hpp"

#define MAX_CLIENT 5

class Server
{
    public:
        //socket
		int 				_server;
		int					_client;
        fd_set              _current_sockets;
        fd_set              _ready_sockets;
        //server config
        int  		        _port;
        std::string 		_server_name;
		std::string 		_server_password;
        int                 _max_client;
        //addr part
		struct sockaddr_in	server_addr;
        struct sockaddr_in	client_addr;
        //user
        std::vector<Client>   _clients;

	public :
		Server();
		Server(int port, string server_name, string server_password, int max_client);
		~Server();

		void    init(void);
        void    accept_new_user(void);
        void    handle_request(int fd);
        void    execute(void);

		int     getClient(void) { return this->_client; };
        int     getServer(void) { return this->_server; };
		void    printInfo();

};

#endif