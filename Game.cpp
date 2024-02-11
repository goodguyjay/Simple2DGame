//
// Created by jay on 1/23/24.
//

/* WINDOW 1280 720 60 0
 * FONT FONTS/TECH.TTF 24 255 255 255
 * PLAYER 32 32 5 5 5 5 255 0 0 4 8
 * ENEMY 32 32 3 3 255 255 255 2 3 8 90 60
 * BULLET 10 10 20 255 255 255 255 255 255 2 20 90
 */

#include "Game.hpp"

#include <fstream>
#include <iostream>
#include <valarray>
#include <random>

Game::Game(const std::string &config) {
    init(config);
}

void Game::init(const std::string &path) {
    // TODO: read in config file here
    // use the premate PlayerConfig, EnemyConfig, BulletConfig variables
    //std::ifstream fin(path);

    //fin >> m_playerConfig.SR >> m_playerConfig.CR /* ...*/;

    // set up default window parameters
    // todo change to read from the config file
    // remember to do the fullscreen or not
    m_window.create(sf::VideoMode(1280, 720), "Simple 2D Game");
    m_window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("C:/Users/jairr/CLionProjects/SimpleGame/font/BebasNeueRegular.ttf")) {
        std::cerr << "Error loading font";
    }

    // this is wacky shit, todo redo later
    m_font = font;

    m_text.setFont(m_font);
    m_text.setCharacterSize(24);
    m_text.setFillColor(sf::Color::White);

    m_highestScoreText.setFont(m_font);
    m_highestScoreText.setCharacterSize(24);
    m_highestScoreText.setFillColor(sf::Color::White);

    spawnPlayer();
}

void Game::run() {
    // TODO: add pause functionality in here
    // some systems should function while paused (rendering)
    // some systems shouldn't (movement / input)

    while (m_running) {
        m_entities.update();

        /*if (!m_paused) {
            // todo decide things
        }*/

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sLifeSpan();

        m_text.setString("Score: " + std::to_string(m_score));
        sf::FloatRect scoreRect = m_text.getLocalBounds();
        m_text.setOrigin(scoreRect.left + scoreRect.width/2.0f, scoreRect.top + scoreRect.height/2.0f);
        m_text.setPosition(scoreRect.width, m_window.getSize().y - m_text.getCharacterSize() - 10);

        // Update the highest score text todo ve isso depois
        m_highestScoreText.setString("Highest Score: " + std::to_string(highestScore));
        sf::FloatRect textRect = m_highestScoreText.getLocalBounds();
        m_highestScoreText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
        m_highestScoreText.setPosition(m_window.getSize().x - textRect.width + 5.0f, m_window.getSize().y - m_highestScoreText.getCharacterSize() - 10);

        sRender();

        // increment the current frame
        // may need to be moved when pause implemented
        m_currentFrame++;
    }
}

void Game::setPaused(bool paused) {
    // TODO
}

void Game::spawnPlayer() {
    // TODO: finish adding all properties of the player with the correct values from the config
    m_score = 0;

    const auto entity = m_entities.addEntity("player");

    // middle of the screen
    const float middleOfScreenX = toFloat(m_window.getSize().x) / 2.0f;
    const float middleOfScreenY = toFloat(m_window.getSize().y) / 2.0f;

    // give this entity a transform so it spawns at (200, 200) with velocity (1, 1) and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vec2(middleOfScreenX, middleOfScreenY), Vec2(1.0f, 1.0f), 0.0f);
    entity->cCollision = std::make_shared<CCollision>(35.0f);
    // the entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness 4
    // TODO READ FROM THE CONFIG FILE
    //entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
    entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

    // added an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // since we want this Entity to be our player, set our game's player variable to be this entity
    m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy() {
    // TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
    // the enemy must be spawned completely within the bounds of the window
    const auto entity = m_entities.addEntity("enemy");

    entity->cCollision = std::make_shared<CCollision>(25.0f);

    std::random_device random;
    std::mt19937 gen(random());
    std::uniform_real_distribution<> ex(entity->cCollision->radius * 2, toFloat(m_window.getSize().x) - entity->cCollision->radius * 2);
    std::uniform_real_distribution<> ey(entity->cCollision->radius * 2, toFloat(m_window.getSize().y) - entity->cCollision->radius * 2);

    std::uniform_int_distribution<> points(1, 8);
    std::uniform_int_distribution<> color(0, 255);

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex(gen), ey(gen)), Vec2(0.0f, 0.0f), 0.0f);
    entity->cShape = std::make_shared<CShape>(16.0f, points(gen), sf::Color(color(gen), color(gen), color(gen)), sf::Color(255,255 , 255), 4.0f);

    // prevents the enemy from spawning within 150 pixels of the player
    if (m_player->cTransform->pos.dist(entity->cTransform->pos) < 150) {
        std::cout << "enemy destroyed" << std::endl;
        entity->destroy();
        std::cout << "prevented shit" << std::endl;
        spawnEnemy();
    }

    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
    // TODO: spawn small enemies at the location of the input enemy e

    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the vertices of the original enemy
    // - set each small enemy to the same color as the original, half the size
    // - small enemies are worth double points of the original enemy

}

void Game::spawnBullet(const std::shared_ptr<Entity>& entity, const Vec2 &target) {

    // calculate direction vector from player to target
    const Vec2 direction = target - entity->cTransform->pos;

    // normalize the direction vector get a unit vector
    const Vec2 unitDirection = direction / direction.length();

    // calculate the spawn position for the bullet
    Vec2 spawnPos = entity->cTransform->pos + unitDirection * (entity->cShape->circle.getRadius() + 5.0f);

    const auto bullet = m_entities.addEntity("bullet");

    bullet->cTransform = std::make_shared<CTransform>(spawnPos, unitDirection * 10, 0);
    bullet->cShape = std::make_shared<CShape>(10, 8, sf::Color(255, 255, 255), sf::Color(255, 0, 0), 0);
    bullet->cLifespan = std::make_shared<CLifespan>(40);
    bullet->cCollision = std::make_shared<CCollision>(20);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
    // TODO: implement special weapon
}

void Game::sMovement() {

    m_player->cTransform->velocity = { 0, 0 };

    for (std::map<sf::Keyboard::Key, Vec2> directions = {
             {sf::Keyboard::W, {0, -5}},
             {sf::Keyboard::A, {-5, 0}},
             {sf::Keyboard::S, {0, 5}},
             {sf::Keyboard::D, {5, 0}},
         }; const auto &[key, direction] : directions) {
        if (sf::Keyboard::isKeyPressed(key)) {
            m_player->cTransform->velocity += direction;
        }
    }

    m_player->cTransform->pos += m_player->cTransform->velocity;

    // handle bullet movement
    for (const auto& bullet : m_entities.getEntities("bullet")) {
        bullet->cTransform->pos += bullet->cTransform->velocity;
    }

    for (const auto& enemy : m_entities.getEntities("enemy")) {
        enemy->cTransform->pos += enemy->cTransform->velocity;
    }
}

void Game::sCollision() {
    //TODO: implement all proper collisions between entities
    // be sure to use the collision radius, not the shape radius

    for (const auto& e : m_entities.getEntities("enemy")) {
        sf::FloatRect playerBounds = m_player->cShape->circle.getGlobalBounds();
        sf::FloatRect enemyBounds = e->cShape->circle.getGlobalBounds();

        // if the distance between the player and the enemy is less than the sum of their radii, they are colliding
        // all enemies are destroyed and the player is destroyed and respawned
        if (playerBounds.intersects(enemyBounds)) {
            for (const auto& allEntities : m_entities.getEntities()) {
                allEntities->destroy();
            }
            std::cout << "Collision with enemy" << std::endl;
            spawnPlayer();
            break;
        }

        for (auto b : m_entities.getEntities("bullet")) {
            float bx = b->cTransform->pos.x - e->cTransform->pos.x;
            float by = b->cTransform->pos.y - e->cTransform->pos.y;
            float bulletDistance = std::sqrt(bx * bx + by * by);

            // if the bullet hits the enemy, the bullet and enemy will be destroyed
            if (bulletDistance < b->cCollision->radius + e->cCollision->radius) {
                std::cout << "enemy destroyed" << std::endl;
                e->destroy();
                b->destroy();

                // increase the score by the number of points the enemy was worth
                m_score += e->cShape->circle.getPointCount() * 2;

                // if the score is higher than the highest score, set the highest score to the current score
                if(m_score >= highestScore) {
                    highestScore = m_score;
                }
                std::cout << "Score: " << m_score << std::endl;
                break;
            }
        }
    }

    // destroys player if it collides with the wall
    if (m_player->cTransform->pos.x + m_player->cShape->circle.getRadius() > toFloat(m_window.getSize().x) || m_player->cTransform->pos.x - m_player->cShape->circle.getRadius() <= 0) {
        m_score = 0;
        m_player->destroy();
        std::cout << "collision with a wall" << std::endl;
        spawnPlayer();
    }

    if (m_player->cTransform->pos.y + m_player->cShape->circle.getRadius() > toFloat(m_window.getSize().y) || m_player->cTransform->pos.y - m_player->cShape->circle.getRadius() <= 0) {
        m_score = 0;
        m_player->destroy();
        std::cout << "collision with a wall" << std::endl;
        spawnPlayer();
    }
}

int i = 0;

void Game::sEnemySpawner() {

    // spawns the enemy every 90 frames and on the first frame as well
    if (m_currentFrame - m_lastEnemySpawnTime > 90 || m_currentFrame == 1) {
        m_entities.update();
        spawnEnemy();
        i++;
        std::cout << "spawned enemy " << i << std::endl;
    }

    // reverts the enemy velocity when it hits the wall
    for (const auto& e : m_entities.getEntities("enemy")) {

        if (e->cTransform->pos.x + e->cShape->circle.getRadius() > toFloat(m_window.getSize().x) || e->cTransform->pos.x - e->cShape->circle.getRadius() <= 0) {
            e->cTransform->velocity.x *= -1;
        }

        if (e->cTransform->pos.y + e->cShape->circle.getRadius() > toFloat(m_window.getSize().y) || e->cTransform->pos.y - e->cShape->circle.getRadius() <= 0) {
            e->cTransform->velocity.y *= -1;
        }
    }
}



void Game::sRender() {
    // sample drawing of the player entity that we have created
    m_window.clear();

    // set the position of the shape based on the entity's transform->pos
    m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

    // set the rotation of the shape based on the entity's transform->angle
    m_player->cTransform->angle += 1.0f;
    m_player->cShape->circle.setRotation(m_player->cTransform->angle);

    // draw the shape
    m_window.draw(m_player->cShape->circle);

    // draw the font
    m_window.draw(m_text);
    m_window.draw(m_highestScoreText);

    for (const auto& e : m_entities.getEntities()) {
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);

        m_window.draw(e->cShape->circle);
    }

    m_window.display();
}

void Game::sUserInput() {

    sf::Event event{};
    while (m_window.pollEvent(event)) {
        // this event triggers when the window is closed
        if(event.type == sf::Event::Closed) {
            m_running = false;
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {

                case sf::Keyboard::W:
                    m_player->cInput->up = true;
                break;

                case sf::Keyboard::A:
                    m_player->cInput->left = true;
                break;

                case sf::Keyboard::S:
                    m_player->cInput->down = true;
                break;

                case sf::Keyboard::D:
                    m_player->cInput->right = true;
                break;

                default:
                    break;
            }
        }

        //this event is triggered when the key is released
        if (event.type == sf::Event::KeyReleased) {

            switch (event.key.code) {

                case sf::Keyboard::W:
                    m_player->cInput->up = false;
                    break;

                case sf::Keyboard::A:
                    m_player->cInput->left = false;

                case sf::Keyboard::S:
                    m_player->cInput->down = false;
                break;

                case sf::Keyboard::D:
                    m_player->cInput->right = false;
                break;

                default:
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                spawnBullet(m_player, Vec2(toFloat(event.mouseButton.x), toFloat(event.mouseButton.y)));
            }

            if (event.mouseButton.button == sf::Mouse::Right) {
                std::cout << "Right mouse button clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << std::endl;
                //todo call spawnSpecialWeapon
            }
        }
    }
}

void Game::sLifeSpan() {
    for (const auto& b : m_entities.getEntities("bullet")) {
        b->cLifespan->remaining -= 1;

        if (b->cLifespan->remaining == 0) {
            b->destroy();
        }
    }
}
