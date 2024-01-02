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

void ParticleSystem::collisionBroadPhase()
{
    // sort particles based on their projection on OX axis and leave inactive particles at the end
    std::sort(this->particles.begin(), this->particles.end(), [](const Particle& p1, const Particle& p2) {
        if (!p1.getIsActive()) return false;
        if (!p2.getIsActive()) return true;
        float r1 = p1.getRadius(), r2 = p2.getRadius();
        sf::Vector2f c1 = p1.getCenter(), c2 = p2.getCenter();
        return c1.x - r1 < c2.x - r2;
        });

    // remove inactive particles if any (inactive particles are sorted to the end
    while (!this->particles[this->particles.size() - 1].getIsActive()) this->particles.pop_back();

    // initialize active group and active interval with the first particle
    std::vector<std::pair<Particle*, std::size_t>> activeGroup{ {&this->particles[0], 0} };
    sf::Vector2f activeInterval = this->particles[0].getOXProjection();
    for (std::size_t i = 1; i < this->particles.size(); ++i)
    {
        sf::Vector2f currentInterval = this->particles[i].getOXProjection();
        if (activeGroup.empty())
        {
            // first particle in the active group
            activeGroup.push_back({ &this->particles[i], i });
            activeInterval = currentInterval;
        }
        else
        {
            if (activeInterval.y >= currentInterval.x)
            {
                // extend interval if intervals intersect
                activeGroup.push_back({ &this->particles[i], i });
                activeInterval.y = std::max(activeInterval.y, currentInterval.y);
            }
            else
            {
                // proceed to narrow phase
                this->collisionNarrowPhase(activeGroup);

                // empty active group
                activeGroup = std::vector<std::pair<Particle*, std::size_t>>{};
            }
        }
    }
}

void ParticleSystem::collisionNarrowPhase(std::vector<std::pair<Particle*, std::size_t>>& activeGroup)
{
    // define and initialize parent vector used for union
    std::vector<std::size_t> parent(activeGroup.size());
    for (std::size_t i = 0; i < parent.size(); ++i) parent[i] = i;

    // define find function with path compression
    std::function<std::size_t(std::size_t)> find = [&](std::size_t x) {
        if (parent[x] == x) return x;
        std::size_t result = find(parent[x]);
        parent[x] = result;
        return result;
    };

    // define union function of two sets
    auto unify = [&](std::size_t x, std::size_t y) {
        std::size_t reprX = find(x);
        std::size_t reprY = find(y);

        // compare the masses of the representatives
        if (activeGroup[reprX].first->getMass() > activeGroup[reprY].first->getMass())
            parent[reprY] = reprX;
        else
            parent[reprX] = reprY;
    };

    // perform collision checking for the current active group
    for (std::size_t i = 0; i < activeGroup.size() - 1; ++i)
    {
        const Particle& part1 = *(activeGroup[i].first);
        for (std::size_t j = i + 1; j < activeGroup.size(); ++j)
        {
            const Particle& part2 = *(activeGroup[j].first);
            if (part1.intersects(part2)) unify(i, j);
        }
    }

    // calculate final masses of the representatives of each set
    std::unordered_map<std::size_t, float> massAccumulation;
    for (std::size_t i = 0; i < activeGroup.size(); ++i)
    {
        std::size_t repr = find(i);
        massAccumulation[repr] += activeGroup[i].first->getMass();

        // if current particle is not a representative of a set mark as inactive for later deletion
        if (repr != i) activeGroup[i].first->setIsActive(false);
    }

    // set new mass for representative particles
    for (auto& [index, finalMass] : massAccumulation)
    {
        // set new mass if mass changed(basically the set contained more than one particle)
        if (activeGroup[index].first->getMass() != finalMass) 
            activeGroup[index].first->setMass(finalMass);
    }
}

std::size_t ParticleSystem::getParticleCount() const
{
    return this->particles.size();
}

void ParticleSystem::setParticlesVertexCount(const std::size_t newCount)
{
    this->particlesVertexCount = newCount;
    for (auto& particle : this->particles)
        particle.setParticleVertexCount(newCount);
}

void ParticleSystem::distributeParticles(const std::size_t particleCount)
{
    const float PI = 3.14159265f;
    for (std::size_t i = 0; i < particleCount; ++i)
    {
        float angle = randFloat() * 2 * PI;
        float sin = std::sin(angle);
        float cos = std::cos(angle);
        
        float r = std::abs((randFloat() + randFloat() + randFloat() + randFloat() + randFloat() + randFloat()) / 3.f - 1.f);
        float velMult = 1.f;

        sf::Vector2f pos = sf::Vector2f{ cos, sin } * std::sqrtf(static_cast<int>(particleCount)) * 10.f * r;
        sf::Vector2f vel = sf::Vector2f{ sin , -cos } * velMult;

        this->particles.push_back({ pos, vel, 1.f });
    }

    /*std::sort(this->particles.begin(), this->particles.end(), [](const Particle& part1, const Particle& part2) {
        auto p1 = part1.getPosition();
        auto p2 = part2.getPosition();
        return (p1.x * p1.x + p1.y * p1.y) < (p2.x * p2.x + p2.y * p2.y);
    });

    for (std::size_t i = 0; i < particleCount; ++i)
    {
        sf::Vector2f p = this->particles[i].getPosition();
        float mag = std::sqrtf(p.x * p.x + p.y * p.y);
        float v = std::sqrtf(i / mag);
        this->particles[i].setVelocity(this->particles[i].getVelocity() * v);
    }*/
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
        float velMult = 1.f;

        sf::Vector2f pos = sf::Vector2f{ cos, sin } * radius;
        sf::Vector2f vel = sf::Vector2f{ sin , -cos } *velMult;

        this->particles.push_back({ pos, vel, 1.f });
    }

    /*std::sort(this->particles.begin(), this->particles.end(), [](const Particle& part1, const Particle& part2) {
        auto p1 = part1.getPosition();
        auto p2 = part2.getPosition();
        return (p1.x * p1.x + p1.y * p1.y) < (p2.x * p2.x + p2.y * p2.y);
    });

    for (std::size_t i = 0; i < particleCount; ++i)
    {
        sf::Vector2f p = this->particles[i].getPosition();
        float mag = std::sqrtf(p.x * p.x + p.y * p.y);
        float v = std::sqrtf(i / mag);
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
    const float G = 1.f;
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
            sf::Vector2f tmp = G * diff / (std::max(magnitude_squared, magnitudeThreshold) * magnitude);

            this->particles[i].setAcceleration(this->particles[i].getAcceleration() + mass2 * tmp);
            this->particles[j].setAcceleration(this->particles[j].getAcceleration() - mass1 * tmp);
        }
    }

    for (auto& particle : this->particles)
        particle.move(deltaTime);
}

void ParticleSystem::handleCollisions()
{
    // no collisions to check if empty
    if (this->particles.empty()) return;

    // start broad phase
    this->collisionBroadPhase();
}