/*----------------------------------------------------------------

* @Author: Su

* @Description:

* @Creath Date: 2019-05-25-21.48.18

----------------------------------------------------------------*/

#include "client.h"

int main(int argc, char *argv[])
{
    Client client( "127.0.0.1", 6666 );
    client.Connect();
    client.Run();
    while(1)
    {
        string msg;
        getline(cin, msg);
        if(msg == "exit")
            break;
        cout << "msg\n";
    }
    return 0;
}
