#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include<vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>

#include "Particle.h"

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
private:

    std::vector<Particle> particles;
    std::size_t particlesVertexCount;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    // get rand float between 0 and 1
    static float randFloat();

    void collisionBroadPhase();
    void collisionNarrowPhase(std::vector<std::pair<Particle*, std::size_t>>& activeGroup);

public:

    ParticleSystem();

    std::size_t getParticleCount() const;

    void setParticlesVertexCount(const std::size_t newCount);

    void distributeParticles(const std::size_t particleCount);
    void distributeParticles(const std::size_t particleCount, const float maxRadius);

    void addParticle(sf::Vector2f position, float mass, sf::Vector2f acceleration = { 0.f, 0.f });
    void addParticle(sf::Vector2f position, sf::Vector2f velocity, float mass, sf::Vector2f acceleration = { 0.f, 0.f });

    void update(sf::Time deltaTime);
    void handleCollisions();

    void update(sf::Time deltaTime, std::size_t nrThreads);
};