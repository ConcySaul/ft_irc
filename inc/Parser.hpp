#ifndef PARSER_HPP
#define PARSER_HPP

#include "Tools.hpp"

class   Parser
{
    public:
        std::string                 _token;
        std::string                 _delimiter;
        std::string                 _delimiter1;
        std::vector<std::string>    _parsed_query;
        std::vector<std::string>    _command;
        size_t                      _pos;
        int                         _num_tokens;

    public:
        Parser();
        ~Parser();

    //split on " " or "\n"
    void    parse_query(std::string query);
    void    parse_command(std::string command);
    //tools
    void    get_num_tokens(void);
    int     get_command_id();
    //utils
    void    print_parsed_query(void);
    void    print_parsed_command(void);
};

#endif