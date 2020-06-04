/*----------------------------------------------------------------

* @Author: Su

* @Description:

* @Creath Date: 2019-05-25-16.05.49

----------------------------------------------------------------*/

#include "server.h"
int main(int argc, char *argv[])
{
    Server ser( 6666 );
    ser.Run();
    return 0;
}
