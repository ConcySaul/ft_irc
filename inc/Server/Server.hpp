#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Tools.hpp"
#include "../Client/Client.hpp"
#include "../Channel.hpp"

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
        //users
        std::vector<Client>   _clients;
        //channels
        std::vector<Channel>   _channels;
        //info
        int                   _num_clients;

	public :
		Server();
		Server(int port, string server_name, string server_password, int max_client);
		~Server();
        //
		void    init(void);
        void    accept_new_user(void);
        void    handle_request(int fd);
        void    exec_query(int fd, std::string command);
        void    execute(void);
        void    send_welcome(int fd);
        //COMMAND
        void    pass_command(Parser &parser, int fd);
        void    nick_command(Parser &parser, int fd);
        void    user_command(Parser &parser, int fd);
        void    mode_command(Parser &parser, int fd);
        void    ping_command(Parser &parser, int fd);
        void    join_command(Parser &parser, int fd);
        void    privmsg_command(Parser &parser, int fd);
        void    kick_command(Parser &parser, int fd);
        void    who_command(Parser &parser, int fd);
        void    part_command(Parser &parser, int fd);
        //utils
		int     getClient(void) { return this->_client; };
        int     getServer(void) { return this->_server; };
        void    clear_all_socks();
        std::vector<Client>::iterator get_client_by_nick(std::string nick);
        std::vector<Client>::iterator get_client_by_fd(int fd);
        std::vector<Channel>::iterator get_channel_by_name(std::string name);
		void    printInfo();
        void    printAllUser();

};

#define RPL_WELCOME(servername, nick, username, ip) (":" + servername + " 001 " + nick + " :Welcome to the " + servername + " Network " + nick + "!" + username + "@" + ip + "\r\n")
#define RPL_YOURHOST(servername, nick, version) (":" + servername + " 002 " + nick + " :Your host is " + servername + ", running version " + version + "\r\n")
#define RPL_CREATED(servername, nick, date) (":" + servername + " 003 " + nick +  " :This server was created " + date + "\r\n")
#define RPL_MYINFO(servername, nick, version, usr_modes, chann_modes) (":" + servername + " 004 " + nick  + " " + servername + " " + version + " " + usr_modes + " " + chann_modes + ":bklov\n")

#endif