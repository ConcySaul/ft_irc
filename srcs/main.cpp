#include "../inc/Server/Server.hpp"

using namespace std;

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        cout << "TRY ./ircserv PORT PASSWORD" << endl;
        return(1);
    }
    else
    {
        Server serv(atoi(argv[1]), "IRCoke", argv[2], 3);
        serv.init();
        serv.execute();
    }
    return (0);
}