#include "../inc/Server/Server.hpp"

using namespace std;

int check_ip(char *ip)
{
    int i = 0;
    int len = 0;

    while (ip[len])
        len++;
    for (; i < len; i++)
    {
        if (!std::isdigit(ip[i]))
        {
            cout << "IP must be digit only" << endl;
            return (0);
        }
    }
    return (1);
}

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        cout << "TRY ./ircserv PORT PASSWORD" << endl;
        return(1);
    }
    else
    {
        if (!check_ip(argv[1]))
            return (1);
        Server serv(atoi(argv[1]), "IRCoke", argv[2], 2);
        serv.init();
        serv.execute();
    }
    return (0);
}