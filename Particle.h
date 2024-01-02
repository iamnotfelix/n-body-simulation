#pragma once
#include <SFML/Graphics.hpp>

class Particle : public sf::Drawable, public sf::Transformable
{
private:

    sf::CircleShape shape;

    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float mass;

    bool isActive;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    float calculateRadius(const float m) const;

public:
    // TODO: make a color setter for particles

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
    bool getIsActive() const;

    float getRadius() const;
    sf::Vector2f getCenter() const;
    sf::Vector2f getOXProjection() const;

    void setPosition(sf::Vector2f newPosition);
    void setVelocity(sf::Vector2f newVelocity);
    void setMass(float newMass);
    void setAcceleration(sf::Vector2f newAcceleration);
    void setIsActive(const bool newState);

    void setParticleVertexCount(const std::size_t newCount);

    void move(sf::Time deltaTime);
    bool intersects(const Particle& otherParticle) const;

};