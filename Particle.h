#pragma once
#include <SFML/Graphics.hpp>

class Particle : public sf::Drawable, public sf::Transformable
{
private:

    sf::CircleShape shape;

    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float mass;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    float calculateRadius(const float m) const;

public:
    // TODO: make a color setter for particles
    // TODO: create a flag that decides if collider is drawn or not 
    // or maybe separate the colliders into a differet structure, 
    // and assign them to particles using composition 
    // => better separation and better for when multithreading will be implemented

    Particle();
    Particle(
        sf::Vector2f position,
        sf::Vector2f velocity,
        float mass,
        sf::Vector2f acceleration = { 0.f, 0.f },
        std::size_t particleVertexCount = 30
    );

    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    float getMass() const;
    sf::Vector2f getAcceleration() const;

    sf::Vector2f getCenter() const;

    void setPosition(sf::Vector2f newPosition);
    void setVelocity(sf::Vector2f newVelocity);
    void setMass(float newMass);
    void setAcceleration(sf::Vector2f newAcceleration);

    void setParticleVertexCount(const std::size_t newCount);

    void move(sf::Time deltaTime);
    bool intersects(const Particle& otherParticle) const;

};