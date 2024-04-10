# import pygame
# import random

# # Initialize Pygame
# pygame.init()
# import socket
# import json

# # Networking setup
# server_address = ('localhost', 12345)  # Server IP and port
# client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# try:
#     client_socket.connect(server_address)
# except ConnectionRefusedError:
#     print('Server connection failed.')
#     quit()

# def send_player_update(x, visible):
#     """Sends player's current x-coordinate and visibility status to the server."""
#     message = json.dumps({'x': x, 'visible': visible})
#     client_socket.sendall(message.encode('utf-8'))


# # Set up display dimensions
# WIDTH, HEIGHT = 600, 400
# WIN = pygame.display.set_mode((WIDTH, HEIGHT))
# pygame.display.set_caption("Dodging Asteroids")

# # Colors
# WHITE = (255, 255, 255)
# BLACK = (0, 0, 0)
# RED = (255, 0, 0)

# # Player settings
# player_width = 50
# player_height = 50
# player_x = (WIDTH - player_width) // 2
# player_y = HEIGHT - player_height - 20
# player_speed = 5

# # Asteroid settings
# asteroid_width = 50
# asteroid_height = 50
# asteroid_speed = 3
# asteroid_frequency = 25
# asteroids = []

# # Clock
# clock = pygame.time.Clock()

# # Player class
# class Player:
#     def __init__(self, x, y, width, height, color=RED):
#         self.rect = pygame.Rect(x, y, width, height)
#         self.color = color

#     def draw(self):
#         pygame.draw.rect(WIN, self.color, self.rect)

#     def move(self, direction):
#         if direction == "left":
#             self.rect.x -= player_speed
#         elif direction == "right":
#             self.rect.x += player_speed

#         # Keep player within screen boundaries
#         if self.rect.left < 0:
#             self.rect.left = 0
#         elif self.rect.right > WIDTH:
#             self.rect.right = WIDTH

# # Function to create asteroids
# def create_asteroid():
#     asteroid_x = random.randrange(0, WIDTH - asteroid_width)
#     asteroid_y = -asteroid_height
#     asteroids.append(pygame.Rect(asteroid_x, asteroid_y, asteroid_width, asteroid_height))

# # Main game loop
# def main():
#     player = Player(player_x, player_y, player_width, player_height)

#     run = True
#     while run:
#         clock.tick(30)

#         for event in pygame.event.get():
#             if event.type == pygame.QUIT:
#                 run = False

#         keys = pygame.key.get_pressed()
#         if keys[pygame.K_LEFT]:
#             player.move("left")
#         if keys[pygame.K_RIGHT]:
#             player.move("right")

#         # Create asteroids
#         if random.randint(1, asteroid_frequency) == 1:
#             create_asteroid()

#         # Move asteroids
#         for asteroid in asteroids:
#             asteroid.y += asteroid_speed

#         # Remove asteroids that have passed the bottom of the screen
#         for asteroid in asteroids:
#             if asteroid.y > HEIGHT:
#                 asteroids.remove(asteroid)

#         # Collision detection
#         for asteroid in asteroids:
#             if player.rect.colliderect(asteroid):
#                 print("Game Over!")
#                 run = False

#         # Draw everything
#         WIN.fill(BLACK)
#         player.draw()
#         for asteroid in asteroids:
#             pygame.draw.rect(WIN, WHITE, asteroid)

#         pygame.display.update()

#     pygame.quit()

# if __name__ == "__main__":
#     main()
