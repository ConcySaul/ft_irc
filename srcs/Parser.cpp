#include "../inc/Parser.hpp"

Parser::Parser()
{
    this->_token = "";
    this->_delimiter = " ";
    this->_delimiter1 = "\n";
    this->_delimiter2 = "\r";
    this->_pos = 0;
}

Parser::~Parser()
{
}

void    Parser::parse_connection_query(std::string query)
{
    while ((this->_pos = query.find(this->_delimiter)) != std::string::npos
            || (this->_pos = query.find(this->_delimiter1)) != std::string::npos
            ||(this->_pos = query.find(this->_delimiter2)) != std::string::npos)
    {
        this->_token = query.substr(0, this->_pos);
        this->_parsed_query.push_back(this->_token);
        query.erase(0, this->_pos + this->_delimiter.length());
    }
    for (std::vector<std::string>::iterator it = _parsed_query.begin(); it != _parsed_query.end(); it++)
        cout << *it << endl;
}