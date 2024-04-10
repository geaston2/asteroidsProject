from socket import socket, AF_INET, SOCK_STREAM
import json

address =('localhost', 2005)
sock = socket(AF_INET,SOCK_STREAM)
isAlive = True

def sendPlayerData(x):

    message = json.dumps({'positionX':x}) if isAlive else "dead"
    try:
        sock.sendall(message.encode('utf-8'))
    except ConnectionResetError:
        print("Connection was reset by the server.")
    except Exception as e:
        print(f"An error occurred in sending player data: {e}")

def recieveData():
    buffer = ""
    try:
        while True:
            try:
                data = sock.recv(4096).decode('utf-8')
                if not data:
                    break
                buffer += data

                while '\n' in buffer:  # Assuming '\n' is the delimiter
                    # Split the buffer at the first delimiter
                    message, buffer = buffer.split('\n', 1)
                            
                    # Now process the complete message
                    data = json.loads(message)
                    print("Received message:", data)
            except BlockingIOError:
                # No data available at the moment, continue loop
                break
            

    except ConnectionResetError:
        print("Connection was reset by the server.")
    except Exception as e:
        print(f"An error occurred in receiving data: {e}")

try:
    sock.connect(address)

    secret = "monkey_eating_lettuce"
    print("Secret: monkey_Eating_lettuce")
    sock.sendall(secret.encode('utf-8'))
    print("Sent secret")
except ConnectionResetError:
    print("Connection was reset by the server.")
except Exception as e:
    print(f"An error occurred in set up: {e}")

print("Sending data")
for i in range(31):
    recieveData()

isAlive= False
sendPlayerData(0)
recieveData()