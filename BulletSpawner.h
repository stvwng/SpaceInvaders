#pragma once
#include <SFML/Graphics.hpp>

class BulletSpawner
{
    public:
        virtual ~BulletSpawner() = default;

        virtual void spawnBullet(sf::Vector2f spawnLocation, bool forPlayer) = 0;
};