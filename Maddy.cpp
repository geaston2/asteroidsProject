



#include <vector>
#include <mutex>
#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <chrono>
#include <thread>
#include <random>

class GameServer:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.clients = []
        self.player_positions = {}

    def broadcast(self, message):
        for client in self.clients:
            try:
                client.sendall(message.encode('utf-8'))
            except:
                self.clients.remove(client)

    def handle_client(self, client, address):
        print(f"New connection: {address}")
        self.clients.append(client)
        while True:
            try:
                data = client.recv(1024).decode('utf-8')
                if not data:
                    break

                # Here you'll need to parse the data according to your game's protocol
                update = json.loads(data)
                self.player_positions[update['id']] = update['position']
                
                # Broadcast updated positions to all clients
                self.broadcast(json.dumps(self.player_positions))

            except Exception as e:
                print(f"Error handling client {address}: {e}")
                break
        
        client.close()
        self.clients.remove(client)
        print(f"Connection closed: {address}")

    def start_server(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((self.host, self.port))
        server_socket.listen()
        print(f"Game server listening on {self.host}:{self.port}")

        while True:
            client, address = server_socket.accept()
            client_thread = threading.Thread(target=self.handle_client, args=(client, address))
            client_thread.start()

if __name__ == "__main__":
    server = GameServer('0.0.0.0', 3000)
    server.start_server()
