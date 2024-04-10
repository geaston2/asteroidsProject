# Import necessary libraries
import pygame  # For game development (graphics, events, etc.)
import sys  # System-specific parameters and functions (not used in this code)
from socket import socket, AF_INET, SOCK_STREAM  # For networking (socket programming)
import json  # For encoding and decoding JSON strings

# Initialize Pygame and the clock
pygame.init()
clock = pygame.time.Clock()

# Screen setup
size = (800, 600)  # Window size
screen = pygame.display.set_mode(size)  # Create a window of the specified size
pygame.display.set_caption("Client View")  # Set the window title

# Character setup
character = pygame.Rect(400, 300, 50, 50)  # Create a rectangle for the character (starting position and size)
character_speed = 100  # Speed of character movement

# Asteroid setup
asteroid = pygame.Rect(0, 0, 50, 50)  # Initialized but will be updated by the server

# Player alive flag
isAlive = True  # Flag to keep track of the player's life status

# Connection secret
secret = "monkey_eating_lettuce"  # Secret string for connection (authentication purpose)

# Function to reset the game
def reset_game():
    global character, asteroid, isAlive
    character.x = 400  # Reset character's X position
    asteroid.y = 0  # Reset asteroid's Y position to the top
    asteroid.x = 0  # 0 is asteroid's X position
    isAlive = True  # Set the player's status to alive




#things from maddy for updating player positions:
# New structure to hold other players' positions
##   other_players = {}
##   
##   def update_other_players(data):
##       global other_players
##       if "players" in data:
##           other_players = data["players"]
##   
##   def draw_other_players():
##       for player_id, player_info in other_players.items():
##           player_pos = pygame.Rect(player_info["positionX"], 300, 50, 50)  # Assuming all players are on the same Y level for simplicity
##           pygame.draw.rect(screen, (0, 255, 0), player_pos)  # Draw other players in green

################














def main():
    global isAlive
    buffer = ''
    server_address = ('localhost', 2005)  # Server address and port
    sock = socket(AF_INET, SOCK_STREAM)  # Create a TCP socket
    try:
        sock.connect(server_address)  # Connect to the server
        sock.setblocking(0)  # Set the socket to non-blocking mode
        sock.sendall(secret.encode('utf-8'))  # Send the secret for authentication

        done = False
        while not done:  # Game loop
            for event in pygame.event.get():  # Event handling
                if event.type == pygame.QUIT:
                    done = True  # Exit the game loop if window is closed

            current_frame_time = pygame.time.get_ticks()  # Current time (unused)
            delta_time = clock.tick(30) / 1000.0  # Time passed since last frame (to ensure consistent movement speed)

            keys = pygame.key.get_pressed()  # Get state of all keyboard keys
            if isAlive:  # Only allow movement if the player is alive
                # Move character left or right based on key presses
                if keys[pygame.K_LEFT] and character.x >= 0:
                    character.x -= character_speed * delta_time
                if keys[pygame.K_RIGHT] and character.x <= (size[0] - character.width):
                    character.x += character_speed * delta_time

            # Send character's position to the server if alive
            if isAlive:
                message = json.dumps({"positionX": character.x})
                sock.sendall(message.encode('utf-8'))

            # Attempt to receive and parse messages from the server
            try:
                # Receive data and append to buffer
                data = sock.recv(4096).decode('utf-8')  # Adjust buffer size as needed
                buffer += data
                print("Buffer before processing:", buffer)  # Debug print
                
                # Process each message in buffer
                while '\n' in buffer:
                    message, buffer = buffer.split('\n', 1)
                    print("Processing message:", message)  # Debug print
                    data = json.loads(message)
                    
                    if "asteroid" in data:
                        asteroid_info = data["asteroid"]
                        asteroid.x = asteroid_info["assX"]
                        asteroid.y = asteroid_info["assY"]
                        #render() - draw background, asteroid, player

                    #MADDY's
                    ##if "players" in data:  # Check if the message contains players data
                    ##    update_other_players(data)  # Update other players' positions

            except BlockingIOError:
                pass  # No data received, continue
            
            # Check for collision between character and asteroid
            if isAlive and character.colliderect(asteroid):
                isAlive = False  # Set isAlive to False on collision

            # Drawing
            screen.fill((0, 0, 128))  # Fill screen with dark blue
            #Maddys
            #draw_other_players()  # Draw other players
            
            pygame.draw.rect(screen, (255, 0, 0), asteroid)  # Draw the asteroid (red)
            if isAlive:
                pygame.draw.rect(screen, (255, 255, 255), character)  # Draw the character (white) if alive
            pygame.display.flip()  # Update the full display Surface to the screen
            
    except ConnectionResetError:
        print("Connection was reset by the server.")
    except Exception as e:
        print(f"An error occurred: {e}")
        print(sock.recv(4096).decode('utf-8'))
    finally:
        sock.close()  # Ensure the socket is closed on exit
        pygame.quit()  # Quit Pygame

if __name__ == "__main__":
    main()
