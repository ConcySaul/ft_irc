#ifndef PARSER_HPP
#define PARSER_HPP

#include "Tools.hpp"

class   Parser
{
    public:
        std::string                 _token;
        std::string                 _delimiter;
        std::string                 _delimiter1;
        std::string                 _delimiter2;
        std::vector<std::string>    _parsed_query;
        size_t                      _pos;

    public:
        Parser();
        ~Parser();

    void    parse_connection_query(std::string query);
};

#endif