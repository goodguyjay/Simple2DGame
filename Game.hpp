//
// Created by jay on 1/23/24.
//

/* SR = SHAPE RADIUS
 * CR = COLLISION RADIUS
 * FR =
 * */

#ifndef GAME_HPP
#define GAME_HPP

#include "Entity.hpp"
#include "EntityManager.hpp"

#include <SFML/Graphics.hpp>

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game {
private:
    sf::RenderWindow m_window; // the window to draw
    EntityManager m_entities;
    sf::Font m_font;
    sf::Text m_text;
    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    BulletConfig m_bulletConfig;
    int m_score = 0;
    int m_currentFrame = 0;
    int m_lastEnemySpawnTime = 0;
    bool m_paused = false;
    bool m_running = true;

    std::shared_ptr<Entity> m_player;

    void init(const std::string &config); // initialize the game state with a config file path
    void setPaused(bool paused); // pause the game

    void sMovement(); // system: entity position / movement update
    void sUserInput(); // system: user input
    void sLifeSpan();
    void sRender();
    void sEnemySpawner();
    void sCollision();

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);
    static float toFloat(const unsigned int x) { return static_cast<float>(x); }
public:
    Game(const std::string &config); // constructor that takes in game config

    void run();
};



#endif //GAME_HPP
