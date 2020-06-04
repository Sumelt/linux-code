
#include "server.h"

const int PORT = 6666;

int main()
{
    Server serv( PORT );
    serv.Run();
    return 0;
}
