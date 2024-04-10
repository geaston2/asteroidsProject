#include "SharedObject.h"
#include "Semaphore.h"
#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    try
    {
        //Socket theSocket("10.0.2.15", 2000);
        Socket theSocket("127.0.0.1", 2000);
        theSocket.Open();
        std::string entry = "";
        while (entry != "done")
        {
            std::cout << "Enter a string to send: ";
            std::cin >> entry;
            ByteArray ba(entry);
            int written = theSocket.Write(ba);
            if ( written != ba.v.size())
            {
                std::cout << "Wrote: " << written << std::endl;
                std::cout << "The socket appears to have been closed" << std::endl;
                break;
            }
            else
            {
                std::cout << "Sent: " << entry << std::endl;
            }
            if (theSocket.Read(ba) <=0)
            {
                std::cout << "The socket appears to have been closed" << std::endl;
                break;
            }
            std::cout << "Received: " << ba.ToString() << std::endl;
        }
        std::cout << "Sleep now" << std::endl;
        theSocket.Close();
    }
    catch(std::string s)
    {
        std::cout << s << std::endl;
    }
    catch(...)
    {
        std::cout << "Caught unexpected exception" << std::endl;
    }

}
