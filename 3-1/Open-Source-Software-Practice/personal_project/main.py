import pygame
import os

def is_docker():
    path = '/.dockerenv'
    return os.path.exists(path)

pygame.init()

if not is_docker():
    pygame.mixer.init()
    start_game_sound = pygame.mixer.Sound('sound/start_game.ogg')
    tower_place_sound = pygame.mixer.Sound('sound/tower_place.wav')
    enemy_hit_sound = pygame.mixer.Sound('sound/enemy_hit.wav')
    enemy_reach_end_sound = pygame.mixer.Sound('sound/enemy_reach_end.wav')

screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("Tower Defense Game")

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GREEN = (0, 255, 0)
RED = (255, 0, 0)
BLUE = (0, 0, 255)
GRAY = (128, 128, 128)
YELLOW = (255, 255, 0)
MINT = (0, 255, 255)
BROWN = (150,75,0)


towers = pygame.sprite.Group()
enemies = pygame.sprite.Group()
projectiles = pygame.sprite.Group()
sound_on = True

path = [
    (0, 300), (50, 300), (50, 200), (150, 200), (150, 400),
    (250, 400), (250, 100), (350, 100), (350, 500), (450, 500),
    (450, 100), (550, 100), (550, 400), (650, 400), (650, 300), 
    (800, 300)
]

tower_positions = [
    (100, 250), (200, 350), (300, 150), (400, 300), (400, 550), (500, 150), (600, 350)
]
money = 50
lives = 5
wave = 1
enemies_spawned = 0
next_wave_time = pygame.time.get_ticks() + 10000
spawn_interval = 1000
last_spawn_time = pygame.time.get_ticks()

font = pygame.font.SysFont(None, 36)

clock = pygame.time.Clock()

def draw_intro_screen():
    screen.fill(BLACK)
    title_text = font.render('Tower Defense Game', True, WHITE)
    press_space_text = font.render('Press Spacebar to Start', True, WHITE)
    instructions = [
        "Introduction:",
        "1. You can touch the mint circle to make and upgrade a tower",
        "2. Making a upgrading a tower will cost 50 coins.",
        "3. 5 lives will be given.",
        "4. Enemies will respawn every 15 seconds.",
        "5. You will earn 10 coins each enemy is killed"
    ]

    screen.blit(title_text, (screen.get_width() // 2 - title_text.get_width() // 2, 200))
    screen.blit(press_space_text, (screen.get_width() // 2 - press_space_text.get_width() // 2, 300))

    for i, instruction in enumerate(instructions):
        instruction_text = font.render(instruction, True, WHITE)
        screen.blit(instruction_text, (50, 400 + i * 30))

    pygame.display.flip()
    if not is_docker() and sound_on:
        start_game_sound.play()

def draw_menu_button():
    menu_button_rect = pygame.Rect(screen.get_width() - 110, screen.get_height() - 50, 100, 40)
    pygame.draw.rect(screen, WHITE, menu_button_rect)
    menu_text = font.render('Menu', True, BLACK)
    screen.blit(menu_text, (screen.get_width() - 100, screen.get_height() - 45))
    return menu_button_rect

def draw_menu_screen():
    screen.fill(BLACK)
    menu_text = font.render('Menu', True, BROWN)
    resume_text = font.render('Resume', True, WHITE)
    sound_on_text = font.render('Sound On', True, WHITE)
    sound_off_text = font.render('Sound Off', True, WHITE)
    quit_text = font.render('Quit', True, WHITE)

    screen.blit(menu_text, (screen.get_width() // 2 - menu_text.get_width() // 2, 100))
    screen.blit(resume_text, (screen.get_width() // 2 - resume_text.get_width() // 2, 200))
    screen.blit(sound_on_text, (screen.get_width() // 2 - sound_on_text.get_width() // 2, 300))
    screen.blit(sound_off_text, (screen.get_width() // 2 - sound_off_text.get_width() // 2, 350))
    screen.blit(quit_text, (screen.get_width() // 2 - quit_text.get_width() // 2, 400))

    pygame.display.flip()

def handle_menu_screen_click(pos):
    global game_state, sound_on, running
    resume_rect = pygame.Rect(screen.get_width() // 2 - 50, 200, 100, 50)
    sound_on_rect = pygame.Rect(screen.get_width() // 2 - 50, 300, 100, 50)
    sound_off_rect = pygame.Rect(screen.get_width() // 2 - 50, 350, 100, 50)
    quit_rect = pygame.Rect(screen.get_width() // 2 - 50, 400, 100, 50)
    
    if resume_rect.collidepoint(pos):
        game_state = 'playing'
    elif sound_on_rect.collidepoint(pos):
        sound_on = True
    elif sound_off_rect.collidepoint(pos):
        sound_on = False
    elif quit_rect.collidepoint(pos):
        running = False

#GAME OVER 창
def draw_game_over_screen():
    screen.fill(BLACK)
    game_over_text = font.render('GAME OVER', True, RED)
    restart_text = font.render('Restart', True, WHITE)
    exit_text = font.render('Exit', True, WHITE)

    screen.blit(game_over_text, (screen.get_width() // 2 - game_over_text.get_width() // 2, 200))
    screen.blit(restart_text, (screen.get_width() // 2 - restart_text.get_width() // 2, 300))
    screen.blit(exit_text, (screen.get_width() // 2 - exit_text.get_width() // 2, 350))

    pygame.display.flip()

def handle_game_over_click(pos):
    global game_state, running, lives, money, wave, enemies_spawned, next_wave_time, spawn_interval, last_spawn_time
    restart_rect = pygame.Rect(screen.get_width() // 2 - 50, 300, 100, 50)
    exit_rect = pygame.Rect(screen.get_width() // 2 - 50, 350, 100, 50)

    if restart_rect.collidepoint(pos):
        game_state = 'playing'
        lives = 5
        money = 50
        wave = 1
        enemies_spawned = 0
        next_wave_time = pygame.time.get_ticks() + 10000
        spawn_interval = 1000
        last_spawn_time = pygame.time.get_ticks()
        towers.empty()
        enemies.empty()
        projectiles.empty()
    elif exit_rect.collidepoint(pos):
        running = False

def main_game():

    global game_state, running, menu_button_rect, towers, enemies, projectiles
    global money, lives, wave, enemies_spawned, next_wave_time, spawn_interval, last_spawn_time

    class Tower(pygame.sprite.Sprite):
        def __init__(self, x, y):
            super().__init__()
            self.upgrade_level = 0
            self.shapes = ['triangle', 'square', 'pentagon', 'hexagon', 'heptagon']
            self.image = self.create_shape(self.shapes[self.upgrade_level])
            self.rect = self.image.get_rect()
            self.rect.center = (x, y)
            self.range = 150
            self.fire_rate = 60
            self.last_shot = pygame.time.get_ticks()

        def create_shape(self, shape):
            size = 50
            image = pygame.Surface((size, size), pygame.SRCALPHA)
            if shape == 'triangle':
                points = [(size // 1, 0), (size, size), (0, size)]
            elif shape == 'square':
                points = [(0, 0), (size, 0), (size, size), (0, size)]
            elif shape == 'pentagon':
                points = [(size // 2, 0), (size, size // 3), (2 * size // 3, size), (size // 3, size), (0, size // 3)]
            elif shape == 'hexagon':
                points = [(size // 2, 0), (size, size // 4), (size, 3 * size // 4), (size // 2, size), (0, 3 * size // 4), (0, size // 4)]
            elif shape == 'heptagon':
                points = [(size // 2, 0), (size, size // 6), (size, 5 * size // 6), (size // 2, size), (0, 5 * size // 6), (0, size // 6)]
            pygame.draw.polygon(image, BLUE, points)
            return image

        def upgrade(self):
            self.upgrade_level += 1
            new_shape = self.shapes[min(self.upgrade_level, len(self.shapes) - 1)]
            self.image = self.create_shape(new_shape)
            self.rect = self.image.get_rect(center=self.rect.center)
            self.range += 20
            self.fire_rate = max(10, self.fire_rate - 10)

        def attack(self, enemies, projectiles):
            now = pygame.time.get_ticks()
            if now - self.last_shot >= self.fire_rate:
                for enemy in enemies:
                    distance = pygame.math.Vector2(self.rect.center).distance_to(enemy.rect.center)
                    if distance <= self.range:
                        projectile = Projectile(self.rect.center, enemy)
                        projectiles.add(projectile)
                        self.last_shot = now
                        break

    class Enemy(pygame.sprite.Sprite):
        def __init__(self, path):
            super().__init__()
            self.image = pygame.Surface((30, 30))
            self.image.fill(RED)
            self.rect = self.image.get_rect()
            self.path = path
            self.path_index = 0
            self.rect.center = self.path[self.path_index]
            self.speed = 2 + (wave * 0.05)
            self.max_health = 40 + (wave * 2)
            self.health = self.max_health
            self.reached_end = False

        def update(self):
            if self.path_index < len(self.path) - 1:
                target = self.path[self.path_index + 1]
                direction = pygame.math.Vector2(target) - pygame.math.Vector2(self.rect.center)
                if direction.length() > self.speed:
                    direction = direction.normalize() * self.speed
                self.rect.move_ip(direction)
                if self.rect.center == target:
                    self.path_index += 1
            else:
                self.reached_end = True

        def take_damage(self, damage):
            self.health -= damage
            if self.health <= 0:
                self.kill()
                return True
            return False

        def draw_health_bar(self, surface):
            if self.health > 0:
                health_ratio = self.health / self.max_health
                pygame.draw.rect(surface, RED, (self.rect.x, self.rect.y - 10, 30, 5))
                pygame.draw.rect(surface, GREEN, (self.rect.x, self.rect.y - 10, 30 * health_ratio, 5))

    class Projectile(pygame.sprite.Sprite):
        def __init__(self, pos, target):
            super().__init__()
            self.image = pygame.Surface((10, 10))
            self.image.fill(YELLOW)
            self.rect = self.image.get_rect()
            self.rect.center = pos
            self.target = target
            self.speed = 5
            self.damage = 1

        def update(self):
            if self.target.alive():
                direction = pygame.math.Vector2(self.target.rect.center) - pygame.math.Vector2(self.rect.center)
                if direction.length() > self.speed:
                    direction = direction.normalize() * self.speed
                self.rect.move_ip(direction)
                if self.rect.colliderect(self.target.rect): 
                    if self.target.take_damage(self.damage):
                        global money
                        money += 10
                        if not is_docker() and sound_on:
                            enemy_hit_sound.play()
                    self.kill()
            else:
                self.kill()


    def place_tower(x, y):
        global money
        for tower in towers:
            if tower.rect.center == (x, y):
                if money >= 50:
                    money -= 50
                    tower.upgrade()
                    if not is_docker() and sound_on:
                        tower_place_sound.play()
                return
        if money >= 50:
            money -= 50
            tower = Tower(x, y)
            towers.add(tower)
            if not is_docker() and sound_on:
                tower_place_sound.play()

    def spawn_enemy():
        enemy = Enemy(path)
        enemies.add(enemy)

    def handle_menu_click(pos, menu_button_rect):
        global game_state
        if menu_button_rect.collidepoint(pos):
            game_state = 'menu'


    menu_button_rect = draw_menu_button() 

    while game_state == 'playing' and running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    game_state = 'menu'
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                mouse_pos = pygame.mouse.get_pos()
                handle_menu_click(mouse_pos, menu_button_rect)
                for pos in tower_positions:
                    if pygame.math.Vector2(mouse_pos).distance_to(pos) < 25:
                        place_tower(*pos)
                        break

        current_time = pygame.time.get_ticks()
        if game_state == 'playing':
            if current_time >= next_wave_time:
                wave += 1
                enemies_spawned = 0
                next_wave_time = current_time + 20000

            if enemies_spawned < wave and current_time - last_spawn_time >= spawn_interval:
                spawn_enemy()
                enemies_spawned += 1
                last_spawn_time = current_time

        enemies.update()
        projectiles.update()

        for tower in towers:
            tower.attack(enemies, projectiles)

        screen.fill(GRAY)

        for i in range(len(path) - 1):
            pygame.draw.line(screen, BLACK, path[i], path[i + 1], 10)

        for pos in tower_positions:
            pygame.draw.circle(screen, MINT, pos, 25, 2)

        towers.draw(screen)
        enemies.draw(screen)
        projectiles.draw(screen)

        for enemy in enemies:
            enemy.draw_health_bar(screen)

        for enemy in enemies.copy():
            if enemy.reached_end:
                lives -= 1
                if not is_docker() and sound_on:
                    enemy_reach_end_sound.play()
                enemy.kill()
                if lives <= 0:
                    game_state = 'game_over'

        wave_text = font.render(f'Wave: {wave}', True, WHITE)
        screen.blit(wave_text, (10, 10))
        money_text = font.render(f'Money: {money}', True, WHITE)
        screen.blit(money_text, (screen.get_width() - 150, 10))
        lives_text = font.render(f'Lives: {lives}', True, WHITE)
        screen.blit(lives_text, (screen.get_width() // 2 - lives_text.get_width() // 2, 10))

        menu_button_rect = draw_menu_button()

        pygame.display.flip()
        clock.tick(60)

running = True
show_intro = True
game_state = 'intro'
sound_on = True

while running:
    if game_state == 'intro':
        draw_intro_screen()
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE:
                game_state = 'playing'
                show_intro = False
    elif game_state == 'playing':
        main_game()
    elif game_state == 'menu':
        draw_menu_screen()
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                mouse_pos = pygame.mouse.get_pos()
                handle_menu_screen_click(mouse_pos)
    elif game_state == 'game_over':
        draw_game_over_screen()
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                mouse_pos = pygame.mouse.get_pos()
                handle_game_over_click(mouse_pos)

    pygame.display.flip()
    clock.tick(60)

pygame.quit()