#include <SFML/Graphics.hpp>
#include <mpi.h>

#include "ParticleSystem.h"

#define particleCount 5000

std::string timeToString(sf::Time time)
{
    char c[15];
    sprintf_s(c, "%dms", time.asMilliseconds());
    return std::string(c);
}

int main()
{
    // add anti aliasing
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.f;

    // create the window
    sf::RenderWindow window(sf::VideoMode(1200, 800), "n-body", sf::Style::Default, settings);

    // setting the origin of window to be the center
    sf::Vector2u size = window.getSize();
    sf::View view(sf::Vector2f(0, 0), sf::Vector2f(size.x, size.y));
    window.setView(view);

    // enable vertical sync
    //window.setVerticalSyncEnabled(true);

    // set frame rate to 60
    //window.setFramerateLimit(60);

    // loading font
    sf::Font font;
    if (!font.loadFromFile(".\\courier_prime_font\\CourierPrime-Regular.ttf"))
    {
        printf("Could not load font.");
        return 0;
    }

    // create text object to display performance
    sf::Text performance;
    performance.setFont(font);
    performance.setCharacterSize(15);
    performance.setFillColor(sf::Color::Green);

    sf::Vector2u windowSize = window.getSize();
    performance.setPosition({ -1 * static_cast<float>(windowSize.x) / 2, -1 * static_cast<float>(windowSize.y) / 2 });

    // create the particle system
    ParticleSystem particleSystem;
    particleSystem.setParticlesVertexCount(15);
    particleSystem.distributeParticles(particleCount);

    // create a clock to track the elapsed time
    sf::Clock clock;

    // run the main loop
    while (window.isOpen())
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window while keeping the center (0, 0) in the middle
                sf::Vector2u size = window.getSize();
                sf::View view(sf::Vector2f(0, 0), sf::Vector2f(size.x, size.y));
                window.setView(view);

                // move text in right position
                performance.setPosition({ -1 * static_cast<float>(size.x) / 2, -1 * static_cast<float>(size.y) / 2 });
            }
            if (event.type == sf::Event::MouseButtonReleased)
            {
                // add particle when mouse button (any) clicked
                sf::Vector2i position{ event.mouseButton.x, event.mouseButton.y };
                particleSystem.addParticle(window.mapPixelToCoords(position), 10.f);
            }
        }

        // update
        sf::Time elapsed = clock.restart();

        particleSystem.handleCollisions();
        sf::Time collisionTime = clock.getElapsedTime();

        //particleSystem.update(elapsed);
        particleSystem.update(elapsed, 16);
        sf::Time physicsTime = clock.getElapsedTime() - collisionTime;

        // change performance text
        std::string performanceString = "";
        int fps = std::floor(1.f / elapsed.asSeconds());
        performanceString += "FPS: " + std::to_string(fps) + '\n';
        performanceString += "Collision time: " + timeToString(collisionTime) + '\n';
        performanceString += "Physics time: " + timeToString(physicsTime) + '\n';
        performanceString += "Particle count: " + std::to_string(particleSystem.getParticleCount());

        performance.setString(sf::String(performanceString));


        // draw
        window.clear();
        window.draw(performance);
        window.draw(particleSystem);
        window.display();
    }

    return 0;
}