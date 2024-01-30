//
// Created by jay on 1/23/24.
//

#include "EntityManager.hpp"
#include <iostream>

EntityManager::EntityManager() = default;

void EntityManager::update() {
    // TODO: add entities from m_entitiesToAdd to the proper location(s)
    // - add them to the vector of all entities//
    // - add them to the vector inside the map, with the tag as  a key
    for (const auto& e: m_entitiesToAdd) {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }

    m_entitiesToAdd.clear();

    // remove dead entities from the vector of all entities
    removeDeadEntities(m_entities);

    // remove dead entities from each vector in the entity map
    // c++17 way of iterating through [key_value] pairs in a map
    for (auto &[tag, entityVec] : m_entityMap) {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec &vec) {
    // this is called by the update() function
    // if the entity is marked as isActive = false, it will be deleted from the vector
    vec.erase(std::ranges::remove_if(vec, [this](const std::shared_ptr<Entity> &e) {
        if (!e->isActive()) {
            auto &entitiesWithTag = m_entityMap[e->tag()];
            std::erase(entitiesWithTag, e);
            if (entitiesWithTag.empty()) {
                m_entityMap.erase(e->tag());
            }
            return true;
        }
        return false;
    }).begin(),vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag) {
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_entitiesToAdd.push_back(entity);

    return entity;
}

const EntityVec& EntityManager::getEntities() {
    return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string &tag) {
    try {
        return m_entityMap.at(tag);

    } catch (const std::out_of_range &e) {
        //std::cout << "Empty get: " << tag << '\n';
        static const EntityVec emptyVec;
        return emptyVec;
    }
}
