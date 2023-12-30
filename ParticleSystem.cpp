#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
    : particles{}, particlesVertexCount{ 30 } {}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // no texture
    states.texture = NULL;

    // draw the vertex array
    for (auto particle : this->particles)
        target.draw(particle, states);
}

float ParticleSystem::randFloat()
{
    //srand(time(NULL));
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void ParticleSystem::setParticlesVertexCount(const std::size_t newCount)
{
    this->particlesVertexCount = newCount;
    for (auto& particle : this->particles)
        particle.setParticleVertexCount(newCount);
}

void ParticleSystem::distributeParticles(const std::size_t particleCount, const float maxRadius)
{
    const float PI = 3.14159265f;
    for (std::size_t i = 0; i < particleCount; ++i)
    {
        float angle = randFloat() * 2 * PI;
        float sin = std::sin(angle);
        float cos = std::cos(angle);
        
        float radius = randFloat() * maxRadius;

        sf::Vector2f pos{ cos * radius, sin * radius };
        sf::Vector2f vel{ sin , -cos };

        this->particles.push_back({ pos, vel, 3.f });
    }

    /*std::sort(this->particles.begin(), this->particles.end(), [](const Particle& part1, const Particle& part2) {
        auto p1 = part1.getPosition();
        auto p2 = part2.getPosition();
        return (p1.x * p1.x + p1.y * p1.y) < (p2.x * p2.x + p2.y * p2.y);
    });

    for (std::size_t i = 0; i < particleCount; ++i)
    {
        sf::Vector2f p = this->particles[1].getPosition();
        float mag = std::sqrtf(p.x * p.x + p.y * p.y);
        float v = std::sqrtf(1.f / mag);
        this->particles[i].setVelocity(this->particles[i].getVelocity() * v);
    }*/
}

void ParticleSystem::addParticle(sf::Vector2f position, float mass, sf::Vector2f acceleration)
{
    // TODO: get rid of magic values
    sf::Vector2f velocity{ (std::rand() % 600 - 300) / 10.f, (std::rand() % 600 - 300) / 10.f };
    this->particles.push_back({ position, velocity, mass, acceleration, this->particlesVertexCount });
}

void ParticleSystem::addParticle(sf::Vector2f position, sf::Vector2f velocity, float mass, sf::Vector2f acceleration)
{
    this->particles.push_back({ position, velocity, mass, acceleration, this->particlesVertexCount });
}

void ParticleSystem::update(sf::Time deltaTime)
{
    const float magnitudeThreshold = 0.01f;
    for (std::size_t i = 0; i < this->particles.size(); ++i)
    {
        sf::Vector2f pos1 = this->particles[i].getPosition();
        float mass1 = this->particles[i].getMass();
        sf::Vector2f acceleration{ 0.f, 0.f };
        for (std::size_t j = i + 1; j < this->particles.size(); ++j)
        {
            sf::Vector2f pos2 = this->particles[j].getPosition();
            float mass2 = this->particles[j].getMass();

            sf::Vector2f diff = pos2 - pos1;
            float magnitude_squared = diff.x * diff.x + diff.y * diff.y;
            float magnitude = std::sqrtf(magnitude_squared);
            sf::Vector2f tmp = diff / (std::max(magnitude_squared, magnitudeThreshold) * magnitude);

            this->particles[i].setAcceleration(this->particles[i].getAcceleration() + mass2 * tmp);
            this->particles[j].setAcceleration(this->particles[j].getAcceleration() - mass1 * tmp);
        }
    }

    for (auto& particle : this->particles)
        particle.move(deltaTime);
}

void ParticleSystem::handleCollisions()
{
    if (this->particles.empty()) return;

    // search for collisions
    std::unordered_map<int, std::unordered_set<int>> merge;
    for (int i = 0; i < this->particles.size() - 1; ++i)
    {
        const Particle& part1 = this->particles[i];
        for (int j = i + 1; j < this->particles.size(); ++j)
        {
            const Particle& part2 = this->particles[j];
            if (part1.intersects(part2)) merge[i].insert(j);
        }
    }

    std::set<int, std::greater<int>> particlesToMerge;
    for (auto& [key, set] : merge)
    {
        // find the parent particle to merge the rest into it
        // find the final mass of that particle
        float massAccumulation = this->particles[key].getMass();
        float maxMass = this->particles[key].getMass();
        int parent = key;
        for (auto& current : set)
        {
            float currentMass = this->particles[current].getMass();
            massAccumulation += currentMass;
            if (maxMass < currentMass)
            {
                maxMass = currentMass;
                parent = current;
            }
        }

        // change the mass of the parent particle
        this->particles[parent].setMass(massAccumulation);

        // store indexes of particles that need to be deleted
        if (key != parent) particlesToMerge.insert(key);
        for (auto current : set)
            if (current != parent) particlesToMerge.insert(current);
    }

    // remove merged particles
    for (auto partIndex : particlesToMerge)
        this->particles.erase(this->particles.begin() + partIndex);
}