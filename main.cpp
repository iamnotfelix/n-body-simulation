#include <SFML/Graphics.hpp>

#include "ParticleSystem.h"

int main()
{
    // add anti aliasing
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.f;

    // create the window
    sf::RenderWindow window(sf::VideoMode(1200, 700), "n-body", sf::Style::Default, settings);

    // setting the origin of window to be the center
    sf::Vector2u size = window.getSize();
    sf::View view(sf::Vector2f(0, 0), sf::Vector2f(size.x, size.y));
    window.setView(view);

    // enable vertical sync
    //window.setVerticalSyncEnabled(true);

    // set frame rate to 60
    window.setFramerateLimit(30);

    // create the particle system
    ParticleSystem particleSystem;
    particleSystem.setParticlesVertexCount(15);
    particleSystem.distributeParticles(2000, 250);

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
        particleSystem.update(elapsed);

        // draw
        window.clear();
        window.draw(particleSystem);
        window.display();
    }

    return 0;
}