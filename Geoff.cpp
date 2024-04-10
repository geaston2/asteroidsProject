#include "SharedObject.h"
#include "Semaphore.h"
#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <pthread.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

using namespace Sync;

class Player {
public:
    std::string playerName;
    int positionX = 0; // Example player position property
    int playerSpeed = 5;
    bool isAlive = true;
    Socket connection;

    void MoveLeft() { positionX -= playerSpeed; } // Move the player left
    void MoveRight() { positionX += playerSpeed; } // Move the player right
    int GetPositionX() const { return positionX; } // Get player's X position
    void kill() {isAlive=false;}

    Player(Socket playerConnection) : connection(playerConnection) {}
};

std::vector<Player*> players; // Global player list



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
    std::vector<std::thread> threads;

    Semaphore socketSem("Player",0,true);

    //infinite loop
    for(;;)
    {
        try
        {
            //wait for connection
            Socket newSocket = theServer.Accept();
            std::cout << "Received a socket connection!" << std::endl;
            //start thread
            Player newPlayer(newSocket);

            threads.push_back(new CommThread(newSocket));
            players.push_back(newPlayer);
            
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

/*Broadcast Thread*/
void broadcastThreadMain(){

    while(true){
        //interval; 1 second
        std::chrono::seconds interval(1);

        //semWait on players
        socketSem.wait();

        std::vector<int> positions;
        //iterate through players, retrieve position data. Store in thread
        for(const auto& player: players){
            if(player.isAlive)
        }

        //semSignal on players

        //build message;
            //position, followed by a delimiter, position, ...
    }
}



/*Player Thread*/
//pass reference to Player object (will affect original object)
void playerThreadMain(Player &player){
    ByteArray incoming;
    
    while(player.isAlive){
        //read incoming message from player at other end of player.connection Socket
        int r = player.connection.Read(incoming);
        if (r==-1){
            std::cout << "Error in socket detected" << std::endl;
            //no change
        }
        else if (r==0){
            std::cout << "Socket closed at remote end" << std::endl;
            break;
        }
        else{
            std::string theString = incoming.ToString();

            //check type of message (movement or death message)
            if(theString=="death"){
                player.isAlive=false;
            }
            else if(theString=="left"){
                player.MoveLeft();
            }
            else if(theString=="right"){
                player.MoveRight();
            }
        }
    }
}



/*Asteroid Thread*/
void asteroidsThreadMain(){

    int astroidSpeed =2;
    int counter = 0;
    
    std::chrono::seconds interval(1);

    while(true){
        std::this_thread::sleep_for(interval);
        counter=counter+2;
        if(counter%10==0) astroidSpeed++;


        //random position
        int position = randomInRange(0,800);

        ByteArray astroidData;

        //build ByteArray (position and speed)
        ByteArray astroidData = encodeAsteroids(position,astroidSpeed);

        //iterate through Player sockets, send astroidData
        //socketSem.wait();

        for(const auto& connection: players){
            connection->connection.Write(astroidData);
        }

        //socketSem.signal();

    }
}

int randomInRange(int min, int max){

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(min,max);

    return distr(gen);
}

ByteArray encodeAsteroids(int pos, int speed) {
    ByteArray byteArray;

    //identifier for asteroid message
    byteArray.v.push_back('A');

    //2 bytes each for each number
    byteArray.v.push_back(static_cast<char>(pos >> 8)); // Most significant byte
    byteArray.v.push_back(static_cast<char>(pos));      // Least significant byte
    byteArray.v.push_back(static_cast<char>(speed >> 8)); 
    byteArray.v.push_back(static_cast<char>(speed));   

    return byteArray;
}

std::pair<int, int> decodeAsteroids(ByteArray payload) {

    //if asteroid message incorrect format
    if (payload.v.size() < 5) { 
        throw std::runtime_error("Not enough bytes to decode integers");
    }
    if (payload.v[0] != 'A') {
        throw std::runtime_error("Invalid identifier for asteroids data");
    }
    // Decode num1
    int num1 = (static_cast<unsigned char>(payload.v[1]) << 8) | static_cast<unsigned char>(payload.v[2]);

    // Decode num2
    int num2 = (static_cast<unsigned char>(payload.v[3]) << 8) | static_cast<unsigned char>(payload.v[4]);

    return std::make_pair(num1, num2);
}





/**
 * 
 * class CommThread : public Thread
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
*/
