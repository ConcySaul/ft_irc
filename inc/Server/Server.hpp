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
		std::string 		_operator_password;
        std::string 		_allowed_chan_mode;
        std::string 		_allowed_client_mode;
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
        void    user_mode(Parser &parser, int fd);
        void    channel_mode(Parser &parser, int fd);
        void    ping_command(Parser &parser, int fd);
        void    join_command(Parser &parser, int fd);
        void    privmsg_command(Parser &parser, int fd);
        void    kick_command(Parser &parser, int fd);
        void    who_command(Parser &parser, int fd);
        void    part_command(Parser &parser, int fd);
        void    quit_command(Parser &parser, int fd);
        void    notice_command(Parser &parser, int fd);
        void    oper_command(Parser &parser, int fd);
        void    kill_command(Parser &parser, int fd);
        void    die_command(Parser &parser, int fd);
        void    wallops_command(Parser &parser, int fd);
        //utils
        void    send_to_client(std::string buffer, int fd);
		int     getClient(void) { return this->_client; };
        int     getServer(void) { return this->_server; };
        void    removeClient(int fd);
        void    clear_all_socks();
        std::vector<Client>::iterator get_client_by_nick(std::string nick);
        std::vector<Client>::iterator get_client_by_fd(int fd);
        std::vector<Channel>::iterator get_channel_by_name(std::string name);
		void    printInfo();
        void    printAllUser();

};
//RPL_
#define RPL_WELCOME(servername, nick, username, ip) (":" + servername + " 001 " + nick + " :Welcome to the " + servername + " Network " + nick + "!" + username + "@" + ip + "\r\n")
#define RPL_YOURHOST(servername, nick, version) (":" + servername + " 002 " + nick + " :Your host is " + servername + ", running version " + version + "\r\n")
#define RPL_CREATED(servername, nick, date) (":" + servername + " 003 " + nick +  " :This server was created " + date + "\r\n")
#define RPL_MYINFO(servername, nick, version, usr_modes, chan_modes) (":" + servername + " 004 " + nick  + " " + servername + " " + version + " " + usr_modes + " " + chan_modes + " :n\r\n")
#define RPL_YOUREOPER(servername, nick) (":" + servername + " 381 " + nick + " :You are now an operator\r\n")

//ERR_
#define ERR_NOSUCHNICK(servername, nick, nickname) (":" + servername + " 401 " + nick + " " + nickname + " :No such nickname\r\n")
#define ERR_NOSUCHSERVER(servername, nick, server) (":" + servername + " 402 " + nick + " " + server + " :No such server\r\n")
#define ERR_NOSUCHCHAN(servername, nick, chan) (":" + servername + " 403 " + nick + " " + chan + " :No such channel\r\n")
#define ERR_CANNOTSENDTOCHAN(servername, nick, chan) (":" + servername + " 404 " + nick + " " + chan + " :You must be in the room to send a message (+n enabled)\r\n")
#define ERR_NOORIGIN(servername, nick) (":" + servername + " 409 " + nick + " :No origin\r\n")
#define ERR_NONICKNAMEGIVEN(servername, nick) (":" + servername + " 431 " + nick + " :Nickname not given\r\n")
#define ERR_ERRONEUSNICKNAME(servername, nick) (":" + servername + " 432 " + nick + " " + nick + " :Nickname invalid\r\n")
#define ERR_NICKNAMEINUSE(servername, nick) (":" + servername + " 433 " + nick + " " + nick + " :Nickname already used\r\n")
#define ERR_NOTONCHANNEL(servername, nick, chan) (":" + servername + " 442 " + nick + " " + chan + " :You must be in the room to modify it\r\n")
#define ERR_NOTREGISTERED(servername, nick) (":" + servername + " 451 " + nick + " :Not registered\r\n")
#define ERR_NEEDMOREPARAMS(servername, nick, command) (":" + servername + " 461 " + nick + " " + command + " :Need more parameters\r\n")
#define ERR_ALREADYREGISTERED(servername, nick) (":" + servername + " 462 " + nick + " :You are already registered\r\n")
#define ERR_NOPERMFORHOST(servername, nick) (":" + servername + " 463 " + nick + " :Permission not granted\r\n")
#define ERR_PASSWDMISMATCH(servername, nick) (":" + servername + " 464 " + nick + " :Wrong password... Try again\r\n")
#define ERR_NOPRIVILEGES(servername, nick) (":" + servername + " 481 " + nick + " :You must be an operator\r\n")
#define ERR_USERSDONTMATCH(servername, nick) (":" + servername + " 502 " + nick + " :You can't change \r\n")
#define ERR_UNKNOWNMODE(servername, nick, char, modes) (":" + servername + " 572 " + nick + " " + char + " :Unknown user mode. (Valid mode " + modes +")\r\n")
#define ERR_CHANOPRIVSNEEDED(servername, nick, chan) (":" + servername + " 582 " + nick + " " + chan + " :You must be an operator\r\n")
#endif