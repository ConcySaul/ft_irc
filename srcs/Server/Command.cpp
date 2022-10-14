// #include "../../inc/Server/Server.hpp"

// void    Server::send_welcome(int fd)
// {
//     std::vector<Client>::iterator client = this->get_client_by_fd(fd);
// 	if (client == this->_clients.end())
// 		return ;
//     std::string buffer(RPL_WELCOME(this->_server_name, client->_nickname, client->_user_name, client->_ip));
// 	buffer.append(RPL_YOURHOST(this->_server_name, client->_nickname, "InspIRCd-3"));
// 	buffer.append(RPL_CREATED(this->_server_name, client->_nickname, "08:04:20 Oct 10 2022"));
// 	buffer.append(RPL_MYINFO(this->_server_name, client->_nickname, "InspIRCd-3", "iosw", "biklmnopstv "));
//     send(fd, buffer.c_str(), buffer.size(), 0);
// 	buffer.clear();
// }

// //COMMAND
// // void    Server::pass_command(Parser &parser, int fd)
// // {   
// //     parser.print_parsed_command();
// //     std::vector<Client>::iterator client = get_client_by_fd(fd);
// // 	if (this->_server_password != parser._command[1])
// // 	{
// // 		std::string buffer("WRONG PASSWORD");
// // 		return ;
// // 	}

// // }

// void    Server::nick_command(Parser &parser, int fd)
// {   
//     parser.print_parsed_command();
//     std::vector<Client>::iterator client = this->get_client_by_fd(fd);
// 	if (client == this->_clients.end())
// 		return ;
// 	client->_nickname = parser._command[1];
// }

// void    Server::user_command(Parser &parser, int fd)
// {
//     parser.print_parsed_command();
//     std::vector<Client>::iterator client = this->get_client_by_fd(fd);
// 	if (client == this->_clients.end())
// 		return ;
//     client->_user_name = parser._command[1];
//     client->_real_name = parser._command[4].erase(0, 1);
//     this->send_welcome(fd);
// }

// void    Server::mode_command(Parser &parser, int fd)
// {
//     parser.print_parsed_command();
//     std::vector<Client>::iterator client = this->_clients.begin();
//     for (; client != this->_clients.end(); client++)
//     {
//         if (client->_nickname == parser._command[1])
//         {
//             std::string mode = parser._command[2];
//             int flag = 0;
//             size_t pos;
//             for (size_t i = 0; i < mode.length(); i++)
//             {
//                 if (mode[i] == '-')
//                     flag = 0;
//                 else if (mode[i] == '+')
//                     flag = 1;
//                 else
//                 {
//                     if (flag == 1)
//                         client->_mode.push_back(mode[i]);
//                     else if (flag == 0)
//                     {
//                         while ((pos = client->_mode.find(mode[i])) != std::string::npos)
//                         client->_mode.erase(pos, 1);
//                     }
//                 }
//             }
//         }
//         std::string buffer("");
//         buffer.append(":").append(client->_nickname).append("!").append(client->_user_name).append("@").append(client->_ip).append(" MODE ").append(client->_nickname).append(" :").append(parser._command[2]).append("\r\n");
//         send(fd, buffer.c_str(), buffer.size(), 0);
// 		buffer.clear();
//         break;
//     }
// }

// void    Server::ping_command(Parser &parser, int fd)
// {
//     parser.print_parsed_command();
//     std::string buffer("");
//     buffer.append(":").append(parser._command[1]).append(" PONG ").append(parser._command[1]).append(" :").append(parser._command[1]).append("\r\n");
//     send(fd, buffer.c_str(), buffer.size(), 0);
// }

// // void    Server::join_command(Parser &parser, int fd)
// // {
// //     parser.print_parsed_command();

// // }