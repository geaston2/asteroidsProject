#include <time.h>
#include "thread.h"
#include <iostream>
#include "socket.h"
#include <stdlib.h>
#include <cstdlib>

using namespace Sync;
//YES

// Handles communication between a client and a server, managing the entire session.
class CommunicationAgent : public Thread {
private:
	bool activeConnection = false; // Indicates the current state of the network connection.
	bool& terminateSignal; // Flag used to indicate when the session should be terminated.

    Socket& link; // Manages the network connection to the server.

    std::string command; // Stores the latest command from the user.

	ByteArray payload; // Holds data being sent to or received from the server.
    
    

public:
    // Sets up the communication channel and control flag.
    CommunicationAgent(Socket& link, bool& signal) : link(link), terminateSignal(signal) {}

    // Main loop for handling communication after establishing a connection.
    virtual long ThreadMain() {
        // Attempt to establish a network connection.
        try {
            link.Open();
            activeConnection = true;
            std::cout << "Successfully connected to the server." << std::endl;
        } catch(...) {
            std::cerr << "Failed to connect to the server." << std::endl;
        }

        // Process commands from the user until instructed to terminate.
        while (!terminateSignal && activeConnection) {
            std::cout << "Enter some text and see! ('exit' to end): ";
            std::getline(std::cin, command);
			//exit server clean
            if (command == "exit") {

                terminateSignal = true;
				//closing message

                std::cout << "Now closing session." << std::endl;
                break;
            }

            // Sending user input to the server.
            payload = ByteArray(command);

            if (link.Write(payload) > 0) {
				//failed server

                if (link.Read(payload) > 0) {

                    std::cout << "From Server: " << payload.ToString() << std::endl;
                } else {
                    std::cerr << "Server did not respond." << std::endl;
                    terminateSignal = true;
                }
            } else {
                std::cerr << "Message failed to send." << std::endl;
                terminateSignal = true;
            }
        }

        return 0; // Thread completes its execution.
    }
};

// Entry point: Establishes a server connection and handles user interaction.
int main() {
    

    

   
std::system("python client_view.py");


return 0; // Exit the application.

    
    
    
    

    
    

    
}
