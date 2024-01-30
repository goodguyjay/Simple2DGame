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

Game::Game(const std::string &config) {
    init(config);
}

void Game::init(const std::string &path) {
    // TODO: read in config file here
    // use the premate PlayerConfig, EnemyConfig, BulletConfig variables
    std::ifstream fin(path);

    fin >> m_playerConfig.SR >> m_playerConfig.CR /* ...*/;

    // set up default window parameters
    // todo change to read from the config file
    // remember to do the fullscreen or not
    m_window.create(sf::VideoMode(1280, 720), "LOREM IPSUM");
    m_window.setFramerateLimit(60);

    spawnPlayer();
}

void Game::run() {
    // TODO: add pause functionality in here
    // some systems should function while paused (rendering)
    // some systems shouldn't (movement / input)

    while(m_running) {
        m_entities.update();

        if (!m_paused) {
            // todo decide things
        }

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

    // we create every entity by calling EntityManager.addEntity(tag)
    // this returns a std::shared_ptr<Entity> so we use auto to save typing
    auto entity = m_entities.addEntity("player");

    // middle of the screen
    float mx = m_window.getSize().x / 2.0f;
    float my = m_window.getSize().y / 2.0f;

    // give this entity a transform so it spawns at (200, 200) with velocity (1, 1) and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(1.0f, 1.0f), 0.0f);
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
    auto entity = m_entities.addEntity("enemy");

    // todo implement true randomness
    float ex = rand() % m_window.getSize().x;
    float ey = rand() % m_window.getSize().y;

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(0.0f, 0.0f), 0.0f);

    entity->cShape = std::make_shared<CShape>(16.0f, 3, sf::Color(0, 0, 255), sf::Color(255,255 , 255), 4.0f);
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
    // TODO: implement the spawning of a bullet which travels toward target
    // -bullet speed is given as a scalar speed
    // -you must set the velocity by using formula in notes

    auto bullet = m_entities.addEntity("bullet");
    // todo properties transform and mouse pos

    bullet->cTransform = std::make_shared<CTransform>(target, Vec2(0, 0), 0);
    bullet->cShape = std::make_shared<CShape>(10, 8, sf::Color(255, 255, 255), sf::Color(255, 0, 0), 0);
    bullet->cLifespan = std::make_shared<CLifespan>(40);
    bullet->cCollision = std::make_shared<CCollision>(15);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
    // TODO: implement special weapon
}

void Game::sMovement() {

    m_player->cTransform->velocity = { 0, 0 };

    std::map<sf::Keyboard::Key, Vec2> directions = {
        {sf::Keyboard::W, {0, -5}},
        {sf::Keyboard::A, {-5, 0}},
        {sf::Keyboard::S, {0, 5}},
        {sf::Keyboard::D, {5, 0}},
    };

    for (const auto &[key, direction] : directions) {
        if (sf::Keyboard::isKeyPressed(key)) {
            m_player->cTransform->velocity += direction;
        }
    }

    // todo other entities
    /*m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;*/
    m_player->cTransform->pos += m_player->cTransform->velocity;
}

void Game::sCollision() {
    //TODO: implement all proper collisions between entities
    // be sure to use the collision radius, not the shape radius

    for(auto e : m_entities.getEntities("enemy")) {
        float dx = m_player->cTransform->pos.x - e->cTransform->pos.x;
        float dy = m_player->cTransform->pos.y - e->cTransform->pos.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if(distance < m_player->cCollision->radius + e->cCollision->radius) {
            m_player->destroy();
            spawnPlayer();
            break;
        }
    }

    if(m_player->cTransform->pos.x + m_player->cShape->circle.getRadius() > m_window.getSize().x || m_player->cTransform->pos.x - m_player->cShape->circle.getRadius() <= 0) {
        m_player->destroy();
        spawnPlayer();
    }

    if(m_player->cTransform->pos.y + m_player->cShape->circle.getRadius() > m_window.getSize().y || m_player->cTransform->pos.y - m_player->cShape->circle.getRadius() <= 0) {
        m_player->destroy();
        spawnPlayer();
    }
}


void Game::sEnemySpawner() {
    //
    // (use m_currentFrame - m_lastEnemySpawnTime) to determine
    // how long it has been since the last enemy spawned
    if (m_currentFrame - m_lastEnemySpawnTime > 90 || m_currentFrame == 1) {
        spawnEnemy();
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
    // TODO: handle user input here
    // note that you should only be setting the player's input component variables here
    // you should not implement the player's movement logic here
    // the movement system will read the variables you set in this function

    //m_player->cInput->resetMovement();

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
                std::cout << "Left mouse button clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << std::endl;
                // todo call spawnbullet and well, speed and stuff
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
