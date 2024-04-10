#include "SharedObject.h"
#include "Semaphore.h"
#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <pthread.h>
#include <vector>

class CommThread : public Thread
{
private:
    Socket theSocket;
public:
    CommThread(Socket const & p)
        : Thread(true),theSocket(p)
    {
        ;
    }
    int ThreadMain(void)
    {
        ByteArray bytes;
        std::cout << "Created a socket thread!" << std::endl;
        for(;;)
        {
            int read = theSocket.Read(bytes);
            if (read == -1)
            {
                std::cout << "Error in socket detected" << std::endl;
                break;
            }
            else if (read == 0)
            {
                std::cout << "Socket closed at remote end" << std::endl;
                break;
            }
            else
            {
                std::string theString = bytes.ToString();
                std::cout << "Received: " << theString << std::endl;
                bytes.v[0]='R';
                theSocket.Write(bytes);
            }
        }
        std::cout << "Thread is gracefully ending" << std::endl;
    }
};

class KillThread  : public Thread
{
private:
    SocketServer & theServer;
public:
    KillThread(SocketServer & t)
        : Thread(true),theServer(t)
    {
        ;
    }
    int ThreadMain(void)
    {
        std::cout << "Type 'quit' when you want to close the server" << std::endl;
        for (;;)
        {
            std::string s;
            std::cin >> s;
            if(s=="quit")
            {
                theServer.Shutdown();
                break;
            }
        }
    }
};

int main(void)
{
    std::cout << "I am a socket server" << std::endl;
    SocketServer theServer(2000);
    KillThread killer(theServer);
    std::vector<CommThread *> threads;

    for(;;)
    {
        try
        {
            Socket newSocket = theServer.Accept();
            std::cout << "Received a socket connection!" << std::endl;
            threads.push_back(new CommThread(newSocket));
        }
        catch(TerminationException e)
        {
            std::cout << "The socket server is no longer listening. Exiting now." << std::endl;
            break;
        }
        catch(std::string s)
        {
            std::cout << "thrown " << s << std::endl;
            break;
        }
        catch(...)
        {
            std::cout << "caught  unknown exception" << std::endl;
            break;
        }
    }
    std::cout << "End of main" << std::endl;
    for (int i=0;i<threads.size();i++)
        delete threads[i];
}
