import pygame
import random
import sys

# Initialize pygame
pygame.init()
pygame.mixer.init()

# Load sound effects
game_start_sound = pygame.mixer.Sound("audio/game_start.wav")
game_bonus_sound = pygame.mixer.Sound("audio/game_bonus.wav")
game_over_sound = pygame.mixer.Sound("audio/game_over.wav")

# Play game start sound
game_start_sound.play()

# Game settings
WIDTH, HEIGHT = 720, 480
FPS = 10
BLOCK_SIZE = 30

# Load images
background_img = pygame.image.load("space image.png")
astronaut_img = pygame.image.load("astronaut image.png")
black_hole_img = pygame.image.load("black hole image.png")
asteroid_img = pygame.image.load("asteroid.png")

# Resize images
background_img = pygame.transform.scale(background_img, (WIDTH, HEIGHT))
astronaut_img = pygame.transform.scale(astronaut_img, (30, 30))
black_hole_img = pygame.transform.scale(black_hole_img, (30, 30))
asteroid_img = pygame.transform.scale(asteroid_img, (30, 30))

# Load fruit images
fruit_images = {
    "apple": pygame.transform.scale(pygame.image.load("apple.png"), (30, 30)),
    "banana": pygame.transform.scale(pygame.image.load("banana.png"), (30, 30)),
    "grape": pygame.transform.scale(pygame.image.load("grape.png"), (30, 30)),
    "cherry": pygame.transform.scale(pygame.image.load("cherry.png"), (30, 30))
}

# Fruits and scores
fruits = {
    "apple": {"image": fruit_images["apple"], "score": 1},
    "banana": {"image": fruit_images["banana"], "score": 2},
    "grape": {"image": fruit_images["grape"], "score": 1},
    "cherry": {"image": fruit_images["cherry"], "score": 1}
}

# Set up display
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Astronaut Snake in Space")
clock = pygame.time.Clock()

def move_enemy(snake_pos, enemy_pos):
    """Moves the enemy towards the snake with a slow speed."""
    speed = 5
    if enemy_pos[0] < snake_pos[0]:
        enemy_pos[0] += speed
    elif enemy_pos[0] > snake_pos[0]:
        enemy_pos[0] -= speed
    if enemy_pos[1] < snake_pos[1]:
        enemy_pos[1] += speed
    elif enemy_pos[1] > snake_pos[1]:
        enemy_pos[1] -= speed
    return enemy_pos

def draw_game_elements(snake_body, food_pos, current_fruit, enemy_pos, obstacles):
    """Draws all game elements on the screen."""
    screen.blit(background_img, (0, 0))
    screen.blit(astronaut_img, (snake_body[0][0], snake_body[0][1]))

    for pos in snake_body[1:]:
        pygame.draw.rect(screen, (0, 255, 0), pygame.Rect(pos[0], pos[1], BLOCK_SIZE, BLOCK_SIZE))

    screen.blit(fruits[current_fruit]["image"], (food_pos[0], food_pos[1]))
    screen.blit(black_hole_img, (enemy_pos[0], enemy_pos[1]))

    for obs in obstacles:
        screen.blit(asteroid_img, (obs[0], obs[1]))

def game_over_screen():
    """Displays the Game Over screen with restart option."""
    screen.fill((0, 0, 0))
    font = pygame.font.Font(None, 50)
    text = font.render("GAME OVER - Press 'R' to Restart", True, (255, 0, 0))
    text_rect = text.get_rect(center=(WIDTH // 2, HEIGHT // 2))
    screen.blit(text, text_rect)
    pygame.display.flip()

    # Wait for user input to restart or quit
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_r:  # Press 'R' to restart
                    main()

def main():
    # Game variables
    snake_pos = [100, 50]
    snake_body = [[100, 50], [90, 50]]
    direction = 'RIGHT'
    change_to = direction
    score = 0
    current_fruit = random.choice(list(fruits.keys()))

    enemy_pos = [random.randrange(1, (WIDTH // BLOCK_SIZE)) * BLOCK_SIZE,
                 random.randrange(1, (HEIGHT // BLOCK_SIZE)) * BLOCK_SIZE]

    food_pos = [random.randrange(1, (WIDTH // BLOCK_SIZE)) * BLOCK_SIZE,
                random.randrange(1, (HEIGHT // BLOCK_SIZE)) * BLOCK_SIZE]

    # Generate obstacles
    obstacles = []
    for _ in range(5):
        start_x = random.randrange(1, (WIDTH // BLOCK_SIZE)) * BLOCK_SIZE
        start_y = random.randrange(1, (HEIGHT // BLOCK_SIZE)) * BLOCK_SIZE
        cluster_size = random.choice([2, 3])
        for i in range(cluster_size):
            obstacles.append([start_x, start_y + (i * BLOCK_SIZE)])

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.KEYDOWN:
                if event.key in [pygame.K_UP, ord('w')]:
                    change_to = 'UP'
                elif event.key in [pygame.K_DOWN, ord('s')]:
                    change_to = 'DOWN'
                elif event.key in [pygame.K_LEFT, ord('a')]:
                    change_to = 'LEFT'
                elif event.key in [pygame.K_RIGHT, ord('d')]:
                    change_to = 'RIGHT'

        if change_to == 'UP' and direction != 'DOWN':
            direction = 'UP'
        if change_to == 'DOWN' and direction != 'UP':
            direction = 'DOWN'
        if change_to == 'LEFT' and direction != 'RIGHT':
            direction = 'LEFT'
        if change_to == 'RIGHT' and direction != 'LEFT':
            direction = 'RIGHT'

        # Move the snake
        if direction == 'UP':
            snake_pos[1] -= BLOCK_SIZE
        if direction == 'DOWN':
            snake_pos[1] += BLOCK_SIZE
        if direction == 'LEFT':
            snake_pos[0] -= BLOCK_SIZE
        if direction == 'RIGHT':
            snake_pos[0] += BLOCK_SIZE

        enemy_pos = move_enemy(snake_pos, enemy_pos)
        
        # 뱀 머리의 새 위치 추가
        snake_body.insert(0, list(snake_pos))

        # 충돌 검사용 Rect 생성
        snake_rect = pygame.Rect(snake_body[0][0], snake_body[0][1], BLOCK_SIZE, BLOCK_SIZE)
        enemy_rect = pygame.Rect(enemy_pos[0], enemy_pos[1], BLOCK_SIZE, BLOCK_SIZE)
        food_rect = pygame.Rect(food_pos[0], food_pos[1], BLOCK_SIZE, BLOCK_SIZE)
        
        # 적과의 충돌 검사
        if snake_rect.colliderect(enemy_rect):
            game_over_sound.play()
            game_over_screen()
            return  # 게임 루프 종료
        
        # 장애물과의 충돌 검사
        for obs in obstacles:
            obs_rect = pygame.Rect(obs[0], obs[1], BLOCK_SIZE, BLOCK_SIZE)
            if snake_rect.colliderect(obs_rect):
                game_over_sound.play()
                game_over_screen()
                return
            
        # 음식과의 충돌 검사
        if snake_rect.colliderect(food_rect):
            score += fruits[current_fruit]["score"]
            food_pos = [random.randrange(1, (WIDTH // BLOCK_SIZE)) * BLOCK_SIZE,
                        random.randrange(1, (HEIGHT // BLOCK_SIZE)) * BLOCK_SIZE]
            current_fruit = random.choice(list(fruits.keys()))
            game_bonus_sound.play()
        else:
            snake_body.pop()


        # 게임 화면 경계 체크
        if (snake_pos[0] < 0 or snake_pos[0] >= WIDTH or
            snake_pos[1] < 0 or snake_pos[1] >= HEIGHT):
            game_over_sound.play()
            game_over_screen()
            return

        draw_game_elements(snake_body, food_pos, current_fruit, enemy_pos, obstacles)
        pygame.display.flip()
        clock.tick(FPS)

if __name__ == "__main__":
    main()
