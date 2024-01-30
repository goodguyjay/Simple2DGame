//
// Created by jay on 1/23/24.
//

#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include "Entity.hpp"
#include <vector>
#include <map>

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager {
private:
    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    EntityMap m_entityMap;
    size_t m_totalEntities = 0;

    void removeDeadEntities(EntityVec &vec);

public:
    EntityManager();
    void update();
    std::shared_ptr<Entity> addEntity(const std::string &tag);

    const EntityVec &getEntities();
    const EntityVec &getEntities(const std::string &tag);
};



#endif //ENTITYMANAGER_HPP
