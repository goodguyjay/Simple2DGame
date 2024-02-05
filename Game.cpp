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

    spawnPlayer();
}

void Game::run() {
    // TODO: add pause functionality in here
    // some systems should function while paused (rendering)
    // some systems shouldn't (movement / input)

    while(m_running) {
        m_entities.update();

        /*if (!m_paused) {
            // todo decide things
        }*/

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sLifeSpan();

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

    // we create every entity by calling EntityManager.addEntity(tag)
    // this returns a std::shared_ptr<Entity> so we use auto to save typing
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

    // add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // since we want this Entity to be our player, set our game's player variable to be this entity
    // this goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy() {
    // TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
    // the enemy must be spawned completely within the bounds of the window
    const auto entity = m_entities.addEntity("enemy");

    std::random_device random;
    std::mt19937 gen(random());
    std::uniform_real_distribution<float> ex(0, static_cast<float>(m_window.getSize().x) - 30);
    std::uniform_real_distribution<float> ey(0, static_cast<float>(m_window.getSize().y) + 45);

    std::uniform_int_distribution<> points(1, 8);

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex(gen), ey(gen)), Vec2(3.0f, 1.0f), 0.0f);
    entity->cShape = std::make_shared<CShape>(16.0f, points(gen), sf::Color(0, 0, 255), sf::Color(255,255 , 255), 4.0f);
    entity->cCollision = std::make_shared<CCollision>(25.0f);

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

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target) {

    // calculate direction vector from player to target
    Vec2 direction = target - entity->cTransform->pos;

    // normalize the direction vector get a unit vector
    Vec2 unitDirection = direction / direction.length();

    // calculate the spawn position for the bullet
    Vec2 spawnPos = entity->cTransform->pos + unitDirection * (entity->cShape->circle.getRadius() + 5.0f);

    const auto bullet = m_entities.addEntity("bullet");

    bullet->cTransform = std::make_shared<CTransform>(spawnPos, unitDirection * 5, 0);
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

    for(const auto& e : m_entities.getEntities("enemy")) {
        float dx = m_player->cTransform->pos.x - e->cTransform->pos.x;
        float dy = m_player->cTransform->pos.y - e->cTransform->pos.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // if the distance between the player and the enemy is less than the sum of their radii, they are colliding
        // all enemies are destroyed and the player is destroyed and respawned
        if(distance < m_player->cCollision->radius + e->cCollision->radius) {
            m_player->destroy();
            for (const auto& allEnemies : m_entities.getEntities("enemy")) {
                allEnemies->destroy();
            }
            std::cout << "Collision with enemy" << std::endl;
            spawnPlayer();
            break;
        }

        for(auto b : m_entities.getEntities("bullet")) {
            float bx = b->cTransform->pos.x - e->cTransform->pos.x;
            float by = b->cTransform->pos.y - e->cTransform->pos.y;
            float bulletDistance = std::sqrt(bx * bx + by * by);

            if(bulletDistance < b->cCollision->radius + e->cCollision->radius) {
                e->destroy();
                b->destroy();
                m_score += e->cShape->circle.getPointCount() * 2;
                std::cout << "Score: " << m_score << std::endl;
                break;
            }
        }
    }

    if(m_player->cTransform->pos.x + m_player->cShape->circle.getRadius() > m_window.getSize().x || m_player->cTransform->pos.x - m_player->cShape->circle.getRadius() <= 0) {
        m_score = 0;
        m_player->destroy();
        std::cout << "collision with a wall" << std::endl;
        spawnPlayer();
    }

    if(m_player->cTransform->pos.y + m_player->cShape->circle.getRadius() > m_window.getSize().y || m_player->cTransform->pos.y - m_player->cShape->circle.getRadius() <= 0) {
        m_score = 0;
        m_player->destroy();
        std::cout << "collision with a wall" << std::endl;
        spawnPlayer();
    }
}


void Game::sEnemySpawner() {
    if (m_currentFrame - m_lastEnemySpawnTime > 90 || m_currentFrame == 1) {
        spawnEnemy();
    }


    // todo look at this later, it seems to be reverting the velocity of the enemy when it hits the wall
    for(const auto& e : m_entities.getEntities("enemy")) {
        if(e->cTransform->pos.x + e->cShape->circle.getRadius() > static_cast<float>(m_window.getSize().x) || e->cTransform->pos.x - e->cShape->circle.getRadius() <= 0) {
            e->cTransform->velocity.x *= -1;
            //std::cout << "outside boundaries" << std::endl;
        }

        if(e->cTransform->pos.y + e->cShape->circle.getRadius() > m_window.getSize().y || e->cTransform->pos.y - e->cShape->circle.getRadius() <= 0) {
            e->cTransform->velocity.y *= -1;
            //std::cout << "outside boundaries" << std::endl;
        }
    }

}



void Game::sRender() {
    // TODO: change the code below to draw all of the entities
    // sample drawing of the player entity that we have created
    m_window.clear();

    //std::cout << m_entities.getEntities().size();

    // set the position of the shape based on the entity's transform->pos
    m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

    // set the rotation of the shape based on the entity's transform->angle
    m_player->cTransform->angle += 1.0f;
    m_player->cShape->circle.setRotation(m_player->cTransform->angle);

    // draw the entity's sf::CircleShape
    // todo draw the enemies bruh ps-> bullets also?
    m_window.draw(m_player->cShape->circle);

    for (auto e : m_entities.getEntities()) {
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);

        m_window.draw(e->cShape->circle);
    }

    m_window.display();
}

void Game::sUserInput() {

    sf::Event event;
    while(m_window.pollEvent(event)) {
        //this event triggers when the window is closed
        if(event.type == sf::Event::Closed) {
            m_running = false;
        }

        //this event is triggered when a key is pressed
        if(event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {

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
        if(event.type == sf::Event::KeyReleased) {

            switch(event.key.code) {

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

        if(event.type == sf::Event::MouseButtonPressed) {
            if(event.mouseButton.button == sf::Mouse::Left) {
                //std::cout << "Left mouse button clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << std::endl;
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }

            if(event.mouseButton.button == sf::Mouse::Right) {
                std::cout << "Right mouse button clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << std::endl;
                //todo call spawnSpecialWeapon
            }
        }
    }
}

void Game::sLifeSpan() {
    for (auto b : m_entities.getEntities("bullet")) {
        b->cLifespan->remaining -= 1;

        if (b->cLifespan->remaining == 0) {
            b->destroy();
        }
    }
}
