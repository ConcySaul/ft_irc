#include "../inc/Parser.hpp"

Parser::Parser()
{
    this->_token = "";
    this->_delimiter = " ";
    this->_delimiter1 = "\n";
    this->_pos = 0;
    this->_num_tokens = 0;
}

Parser::~Parser()
{
}

void    Parser::print_parsed_query(void)
{
    std::vector<std::string>::iterator it = this->_parsed_query.begin();
    for (; it != this->_parsed_query.end(); it++)
        cout << *it << " ";
    cout << endl << endl;
}

void    Parser::print_parsed_command(void)
{
    std::vector<std::string>::iterator it = this->_command.begin();
    for (; it != this->_command.end(); it++)
        cout << *it << " ";
    cout << endl << endl;
}

void    Parser::parse_query(std::string query)
{
    this->_parsed_query.clear();
    while ((this->_pos = query.find(this->_delimiter1)) != std::string::npos)
    {
        this->_token = query.substr(0, this->_pos);
        this->_token.resize(this->_token.size());
        this->_parsed_query.push_back(this->_token);
        query.erase(0, this->_pos + this->_delimiter1.length());
    }
}

void    Parser::parse_command(std::string command)
{
    this->_command.clear();
    while ((this->_pos = command.find(this->_delimiter)) != std::string::npos)
    {
        this->_token = command.substr(0, this->_pos);
        this->_command.push_back(this->_token);
        command.erase(0, this->_pos + this->_delimiter.length());
    }
    this->_command.push_back(command);
}

void     Parser::get_num_tokens(void)
{
    std::vector<std::string>::iterator it = this->_command.begin();
    for (; it != this->_command.begin(); it++)
        this->_num_tokens++;
}

int    Parser::get_command_id(void)
{
    std::string commands[] = {"CAP", "PASS", "NICK", "USER", "MODE", "PING", "JOIN", "KICK", "WHO", "PRIVMSG",
                                "PART", "QUIT", "NOTICE", "TOPIC", "INVITE"};
    int i = 0;
    std::vector<std::string>::iterator it = this->_command.begin();
    for (; i < 14; i++)
    {
        if (*it == commands[i])
            return (i);
    }
    return (-1);
}