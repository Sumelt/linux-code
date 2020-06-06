
#include "server.h"

const int PORT = 6666;

int main()
{
    Server serv( PORT );
    serv.Run();
    //serv.daemon_run();
    return 0;
}