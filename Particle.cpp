#include "Particle.h"

Particle::Particle()
    : velocity{ 0,0 }, shape{ 0 }, mass{ 0 }, acceleration{ 0,0 } {}

Particle::Particle(
    sf::Vector2f position,
    sf::Vector2f velocity,
    float mass,
    sf::Vector2f acceleration,
    std::size_t particleVertexCount,
    std::size_t colliderVertexCount
) : velocity{ velocity }, mass{ mass }, acceleration{ acceleration }, shape{ this->calculateRadius(mass), particleVertexCount }, collider{ this->calculateRadius(mass), colliderVertexCount }
{
    const float r = this->calculateRadius(mass);
    this->shape.setOrigin({ r, r });
    
    this->shape.setPosition(position);
    this->collider.setPosition(position);

    this->shape.setFillColor(sf::Color::White);
};

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // no texture
    states.texture = NULL;

    // draw the vertex array
    target.draw(this->shape, states);
    //target.draw(this->collider, states);
}

float Particle::calculateRadius(const float m) const
{
    const float PI = 3.14159265f;
    return std::sqrtf(m / PI);
}

sf::Vector2f Particle::getPosition() const
{
    return this->shape.getPosition();
}

sf::Vector2f Particle::getVelocity() const
{
    return this->velocity;
}

float Particle::getMass() const
{
    return this->mass;
}

sf::Vector2f Particle::getAcceleration() const
{
    return this->acceleration;
}

void Particle::setVelocity(sf::Vector2f newVelocity)
{
    this->velocity = newVelocity;
}

void Particle::setPosition(sf::Vector2f newPosition)
{
    this->shape.setPosition(newPosition);
}

void Particle::setMass(float newMass)
{
    this->mass = newMass;
    const float newRadius = this->calculateRadius(newMass);

    this->shape.setRadius(newRadius);
    this->shape.setOrigin({ newRadius, newRadius });

    this->collider.setRadius(newRadius);
}

void Particle::setAcceleration(sf::Vector2f newAcceleration)
{
    this->acceleration = newAcceleration;
}

void Particle::setParticleVertexCount(const std::size_t newCount)
{
    this->shape.setPointCount(newCount);
}

void Particle::setColliderVertexCount(const std::size_t newCount)
{
    this->collider.setVertexCount(newCount);
}

void Particle::move(sf::Time deltaTime)
{
    // calculate new velocity based on acceleration
    this->velocity = this->velocity + this->acceleration * deltaTime.asSeconds();
    
    // calculate new position based on new velocity
    sf::Vector2f newPosition = 
        this->shape.getPosition() + this->velocity * deltaTime.asSeconds();

    // set new position
    this->shape.setPosition(newPosition);
    this->collider.setPosition(newPosition);

    // reset acceleration to 0
    this->acceleration = { 0.f, 0.f };
}

bool Particle::intersects(const Particle& other) const
{
    //sf::FloatRect boundingBox = this->shape.getGlobalBounds();
    //return boundingBox.intersects(other.shape.getGlobalBounds());
    return this->collider.intersects(other.collider);
}