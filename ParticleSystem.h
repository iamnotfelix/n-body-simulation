#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include<vector>
#include<set>
#include <unordered_map>
#include <unordered_set>

#include "Particle.h"

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
private:

    // TODO: add particle system bounds (after that also change the random position generated when adding particle)
    std::vector<Particle> particles;
    std::size_t particlesVertexCount;
    std::size_t collidersVertexCount;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    // get rand float between 0 and 1
    static float randFloat();

public:

    ParticleSystem();

    void setParticlesVertexCount(const std::size_t newCount);
    void setCollidersVertexCount(const std::size_t newCount);

    void distributeParticles(const std::size_t particleCount, const float maxRadius);

    //void addParticle();
    void addParticle(sf::Vector2f position, float mass, sf::Vector2f acceleration = { 0.f, 0.f });
    void addParticle(sf::Vector2f position, sf::Vector2f velocity, float mass, sf::Vector2f acceleration = { 0.f, 0.f });

    void update(sf::Time deltaTime);
    void handleCollisions();

};