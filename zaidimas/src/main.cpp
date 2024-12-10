#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <ctime>
#include "Objects.h"
#include "Utils.h"

using namespace sf;
using namespace std;

void UpdatePositions(Player& player, float& dy, float& score, bool& isJumping)
{
    const float dx = 3.5f;
    const float jumpSpeed = -10.0f; // Jump speed

    // Player movement controls
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        player.x -= dx;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        player.x += dx;

    // Boundary checks
    if (player.x + PLAYER_WIDTH / 2 > WINDOW_WIDTH)
        player.x = -PLAYER_WIDTH / 2;

    if (player.x + PLAYER_WIDTH / 2 < 0)
        player.x = WINDOW_WIDTH - PLAYER_WIDTH / 2;

    // Gravity and jumping
    if (isJumping)
    {
        dy += 0.2f; // Gravity with acceleration
    }

    player.y += dy;

    // Jumping mechanic: Press space to jump
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !isJumping)
    {
        dy = jumpSpeed;  // Apply jump speed
        isJumping = true;  // Player is now jumping
    }

    // Prevent player from going below the ground
    if (player.y >= MAX_PLAYER_Y)
    {
        player.y = MAX_PLAYER_Y;
        dy = 0;  // Stop vertical movement when the player hits the ground
        isJumping = false;  // Player is on the ground, no longer jumping
    }

    // Update score (e.g., for falling)
    score -= 0.05f * dy;
}

int main()
{
    srand((unsigned)time(nullptr));
    RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Kill The Niggasaa");
    app.setFramerateLimit(60);

    // Load resources
    Texture tBackground, tPlayer1, tBullet, tEnemy;
    tBackground.loadFromFile("resources/background.png");
    tPlayer1.loadFromFile("resources/player.png");
    tBullet.loadFromFile("resources/bullet.png");
    tEnemy.loadFromFile("resources/enemy.png");

    // Font and text setup
    sf::Font font;
    font.loadFromFile("resources/arialbd.ttf");

    sf::Text text;
    text.setFont(font);
    text.setString("0");
    text.setCharacterSize(40);
    text.setFillColor(Color::Red);
    text.setOutlineThickness(1);
    text.setOutlineColor(Color::Black);
    text.setPosition(WINDOW_WIDTH / 2.0f - 25.f, 10.f);

    // Sprites
    Sprite sprBackground(tBackground);
    Sprite sprPlayer(tPlayer1);
    Sprite sprBullet(tBullet);
    Sprite sprEnemy(tEnemy);

#ifdef SOUND_ON
    sf::SoundBuffer buffer;
    buffer.loadFromFile("resources/music.wav");

    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
    sound.setLoop(true);
#endif SOUND_ON

    // Initialize player
    Player player;
    player.x = WINDOW_WIDTH / 2;
    player.y = MAX_PLAYER_Y;

    // Bullet setup
    Bullet bullets[10]; // Array of bullets
    int bulletIndex = 0;
    vector<Enemy> enemies; // Dynamic vector for enemies

    // Timer for enemy spawning
    Clock spawnClock;
    float spawnInterval = 2.0f; // Spawn a new enemy every 2 seconds
    float lastSpawnTime = 0;

    float dy = 0;
    float score = 0;
    bool isJumping = false; // Track whether the player is jumping

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

        // Player controls for shooting
        const float fireCooldown = 0.0001f; // 250ms cooldown between shots
        sf::Clock fireClock;             // Tracks time since last shot
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            if (fireClock.getElapsedTime().asSeconds() >= fireCooldown) // Check cooldown
            {
                bullets[bulletIndex].shoot(player.x + PLAYER_WIDTH, player.y + PLAYER_HEIGHT / 2);
                bulletIndex = (bulletIndex + 1) % 10; // Cycle through bullet array
                fireClock.restart(); // Reset cooldown timer
            }
        }

        // Update player position
        UpdatePositions(player, dy, score, isJumping);

        // Spawn new enemies at regular intervals
        if (spawnClock.getElapsedTime().asSeconds() - lastSpawnTime >= spawnInterval)
        {
            Enemy newEnemy;
            newEnemy.x = WINDOW_WIDTH + rand() % 200;  // Enemies spawn just outside the right edge of the screen
            newEnemy.y = player.y;  // Set the same y-level as the player
            newEnemy.speed = 2.0f;  // Set speed for the new enemy
            newEnemy.active = true; // Mark the enemy as active

            enemies.push_back(newEnemy); // Add the new enemy to the list
            lastSpawnTime = spawnClock.getElapsedTime().asSeconds(); // Update the last spawn time
        }

        // Background scrolling (optional)
        //static float bgOffset = 0;
        //bgOffset += 1.0f;
        //if (bgOffset > WINDOW_WIDTH) bgOffset = 0;
        //sprBackground.setPosition(-bgOffset, 0);
        app.draw(sprBackground);

        // Update and draw player
        sprPlayer.setPosition(player.x, player.y);
        app.draw(sprPlayer);

        // Update and draw bullets
        for (int i = 0; i < 10; ++i)
        {
            if (bullets[i].active)
            {
                bullets[i].update();
                bullets[i].y = MAX_BULLET_Y;
                sprBullet.setPosition(bullets[i].x, bullets[i].y);
                app.draw(sprBullet);
            }
        }

        // Update and draw enemies
        for (auto& enemy : enemies)
        {
            if (enemy.active)
            {
                // Move enemies towards the player
                if (enemy.x > player.x)
                {
                    enemy.x -= enemy.speed;  // Move towards the player
                }
                enemy.y = player.y;  // Set the enemy y-level to the same as the player's y-level
                sprEnemy.setPosition(enemy.x, enemy.y);
                app.draw(sprEnemy);

                // If the enemy goes off the screen, mark it as inactive
                if (enemy.x < 0)
                {
                    enemy.active = false;
                }
            }
        }

        // Collision detection: bullets vs enemies
        for (auto& enemy : enemies)
        {
            if (enemy.active)
            {
                for (int j = 0; j < 10; ++j)
                {
                    if (bullets[j].active &&
                        bullets[j].x + BULLET_WIDTH > enemy.x &&
                        bullets[j].x < enemy.x + ENEMY_WIDTH &&
                        bullets[j].y + BULLET_HEIGHT > enemy.y &&
                        bullets[j].y < enemy.y + ENEMY_HEIGHT)
                    {
                        bullets[j].active = false;  // Deactivate bullet
                        enemy.health -= 1;  // Decrease enemy health
                        if (enemy.health <= 0)
                        {
                            enemy.active = false;  // Destroy enemy if health is 0
                            score += 10;  // Increase score
                        }
                        break;  // Break out of bullet loop to avoid multiple hits in one frame
                    }
                }
            }
        }

        // Draw score
        text.setString(to_string((int)score));
        app.draw(text);

        // Update the window
        app.display();
    }

    return 0;
}
