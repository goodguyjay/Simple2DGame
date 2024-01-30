//
// Created by jay on 1/23/24.
//

#ifndef ENTITY_HPP
#define ENTITY_HPP
#include "Components.hpp"
#include <memory>
#include <string>

class Entity {
    friend class EntityManager;

private:
    bool m_active = true;
    size_t m_id = 0;
    std::string m_tag = "default";

    // private constructor
    Entity() = default;
    Entity(const size_t id, const std::string &tag);
public:
    // component pointers
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifespan> cLifespan;

    bool isActive() const;
    const std::string &tag() const;
    const size_t &id() const;
    void destroy();
};



#endif //ENTITY_HPP
