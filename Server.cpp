#include <iostream>
#include <vector>
#include <mutex>
#include "thread.h"
#include "socketserver.h"
#include "socket.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <stack>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

#include <random>
//you will need to install json

using json = nlohmann::json;
using namespace std;
using namespace Sync; // Assuming Sync namespace is defined in one of the included headers

// Forward declaration of Socket to be used in Player and ClientHandler
class Socket;

class Asteroid {
public:
    int positionX;
    int speed;

    Asteroid(int posX, int spd) : positionX(posX), speed(spd) {}

    json toJson() const {
        json asteroidJson;
        asteroidJson["astroid"] = positionX;
        return asteroidJson;
    }
};

Asteroid asteroid(0, 5); // Initialize with some default values

// Adjust the initialization of the asteroid to start from y = 0
void InitializeAsteroid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, 750); // Random position for X

    asteroid.positionX = disX(gen);
    asteroid.speed = 5; // Speed is set to ensure it moves down 25 pixels per second
}

class Player
{
public:
    int id;
    int positionX = 0;
    bool isAlive= true;
    Socket connection;
    Player(Socket& conn) : connection(conn) {std::cout << "Constructing player" << std::endl;}
    int GetPositionX() const { return positionX; }
    void kill() { isAlive=false;}
};

std::vector<Player *> players;

// Assuming you have a timer or loop that calls this function
void BroadcastAsteroidPositions() {
    while (true) {
        
        InitializeAsteroid();
        json message = asteroid.toJson();
        std::string asteroidMessage = message.dump() + "\n";

        for (Player* ptr : players) {
            cout << "Sending asteroid to player " << ptr->id << std::endl;
            cout << asteroidMessage <<std::endl;
            int bytesSent = ptr->connection.Write(ByteArray(asteroidMessage));
            cout << "Bytes sent: "<<bytesSent << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10000)); // Control the update rate.
    }
}

void SendPlayerPositions(int id,Socket* connectionSocket){

    json playerPositions;
    // Create an array to store player positions
    json positionsArray = json::array();

    for (const auto& p : players)
    {
        int playerID = p -> id;
        bool alive = p->isAlive;

        //if player is not local player and is alive,
        if(id!=playerID and alive){
            // Add each player's position to the array
            json playerData;
            playerData["id"] = playerID;
            playerData["positionX"] = p->GetPositionX();
            positionsArray.push_back(playerData);
        }
    }

        // Add the positions array to the main JSON object
    playerPositions["playerPositions"] = positionsArray;

        // Serialize the JSON object to string and send it
    std::string positionMessage = playerPositions.dump();
    connectionSocket->Write(ByteArray(positionMessage));
}

void ClientThreadMain(Player* connectedPlayer){
    while (true)
    {
        try{
            //ByteArray receivedData;
            /*
            if(connectedPlayer->isAlive){
                connectionSocket->Read(receivedData);
                std::string move = receivedData.ToString();
                std::cout<<"Recieved data from player: "<<move<<std::endl;
                int val = 0;
                if(move=="dead"){
                    connectedPlayer->kill();
                    //stop broadcasting position to players
                    //stop reading data from player-> stop adjusting record of player/accepting inputs
                }
                else{
                    json jsonData = json::parse(move);
                    val = jsonData["positionX"];
                    connectedPlayer -> positionX=val;
                }
            }
            */
            
            //SendPlayerPositions(connectedPlayer->id,connectionSocket);

        }
        catch (...)
        {
            break;
        }
    }
}

int main()
{
    std::cout << "Server is active. Listening on port pyt." << std::endl;
    std::cout << "Type end to shut down the server..." << std::endl;

    SocketServer server(2005);
    
    std::stack<std::thread> threads;

    // Background thread for generating asteroids and broadcasting their positions
    std::thread asteroidThread (BroadcastAsteroidPositions);

    while(true){
        Socket recvSocket = server.Accept();
        try{
            ByteArray bytes;
            int r = recvSocket.Read(bytes);
            if (r == -1)
            {
                std::cout << "Error in socket detected" << std::endl;
                // no change
            }
            else if (r == 0)
            {
                std::cout << "Socket closed at remote end" << std::endl;
                
            }
            else
            {
                std::cout<<"Received message!"<<std::endl;
                std::string theString = bytes.ToString();

                if (theString == "monkey_eating_lettuce")
                {   
                    std::cout<<"Received Secret! Creating Player!"<<std::endl;
                        
                    //Create player, pass socket to player
                    Player* newPlayer=new Player(recvSocket);
std::cout << "past constructor" << std::endl;
                    //set new player id
                    newPlayer->id = players.size()+1;

                    //push player pointer/ref to players list
                    players.push_back(newPlayer);
                    cout<<"Add to vector..."<<std::endl;
                    
                    threads.push(std::thread(ClientThreadMain,newPlayer));
                    cout<<"Added to thread stack"<<std::endl;
                }
            }
        }catch(...){
            break;
        }
    }


    std::cout << "Shutting down the server." << std::endl;
    server.Shutdown();

    for (auto *player : players)
    {
        delete player;
    }
    players.clear();

    while(!threads.empty()){
		threads.top().join();
		threads.pop();
	}
    asteroidThread.join();

    return 0;
}


//Client threads
//Asteroid thread
//Main