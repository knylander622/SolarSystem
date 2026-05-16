#include "orbitHandler.h"

int main()
{
    Sphere sun = {{0,0}, {0,0}, {0,0}, 33300000};
    Sphere earth = {{0,-1600}, {0,0}, {0,0}, 100};
    Sphere mars = {{0,-2432}, {0,0}, {0,0}, 38};
    Sphere venus = {{0, -288}, {0,0}, {0,0}, 81.5};
    Sphere jupiter = {{0, -8320}, {0,0}, {0,0}, 234};
    Sphere neptune = {{0, -48000}, {0,0}, {0,0}, 1715};
    

    initializeOrbit(earth, sun, dt);
    initializeOrbit(mars, sun, dt);
    initializeOrbit(venus, sun, dt);
    initializeOrbit(jupiter, sun, dt);
    initializeOrbit(neptune, sun, dt);


    sf::VideoMode deskWindow = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(deskWindow, "Solar System", sf::State::Fullscreen);
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::View view(sf::Vector2f(0.5f,0.5f), sf::Vector2f(desktop.size.x, desktop.size.y));

    sf::VertexArray trail(sf::PrimitiveType::Points);
    sf::VertexArray marsTrail(sf::PrimitiveType::Points);
    sf::VertexArray jupiterTrail(sf::PrimitiveType::Points);
    sf::VertexArray neptuneTrail(sf::PrimitiveType::Points);

    sf::RectangleShape star({2.0f,2.0f});
    star.setRotation(sf::degrees(45.f));

    sf::CircleShape sunShape(109);
    sunShape.setFillColor(sf::Color(255, 255, 0));
    sunShape.setOrigin(sf::Vector2(sunShape.getGeometricCenter()));

    sf::CircleShape earthShape(1);
    earthShape.setFillColor(sf::Color(144, 238, 144));
    earthShape.setOrigin(sf::Vector2(earthShape.getGeometricCenter()));

    sf::CircleShape marsShape(.53);
    marsShape.setFillColor(sf::Color(255, 63, 52));
    marsShape.setOrigin(sf::Vector2(marsShape.getGeometricCenter()));

    sf::CircleShape jupiterShape(11.2);
    jupiterShape.setFillColor(sf::Color(255, 30, 15));
    jupiterShape.setOrigin(sf::Vector2(jupiterShape.getGeometricCenter()));

    sf::CircleShape neptuneShape(3.95);
    neptuneShape.setFillColor(sf::Color(15, 30, 255));
    neptuneShape.setOrigin(sf::Vector2(neptuneShape.getGeometricCenter()));

    while (window.isOpen())
    { 
        while (auto event = window.pollEvent())
        {
        if (event->is<sf::Event::Closed>())
        window.close();
        if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
        {
            if (wheel->wheel == sf::Mouse::Wheel::Vertical)
            {

                sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
                sf::Vector2f before = window.mapPixelToCoords(mousePixel, view);

                if (wheel->delta > 0)
                    view.zoom(0.9f);
                else if (wheel->delta < 0)
                    view.zoom(1.1f);
                
                sf::Vector2f after = window.mapPixelToCoords(mousePixel, view);
                view.move(before - after);
            }
        }
        }
        

        t = clock();
        fixView(window, view);
        edgeQuestion(window,view);
        window.setView(view);

        updateV(earth, sun, dt);
        updateV(mars, sun, dt);
        updateV(jupiter,sun,dt);
        updateV(neptune, sun, dt);

        window.clear();

        if(draw)
        {
          drawStars(window,star);
          draw = false;
        }

        for(const auto& starz : stars)
        {
          star.setPosition(starz);
          window.draw(star);
        }

        sunShape.setPosition(sf::Vector2f(0.0f, 0.f));
        earthShape.setPosition(sf::Vector2f(static_cast<float>(earth.pos.x / scale), static_cast<float>(earth.pos.y / scale)));
        marsShape.setPosition(sf::Vector2f(static_cast<float>(mars.pos.x / scale), static_cast<float>(mars.pos.y / scale)));
        jupiterShape.setPosition(sf::Vector2f(static_cast<float>(jupiter.pos.x / scale), static_cast<float>(jupiter.pos.y / scale)));
        neptuneShape.setPosition(sf::Vector2f(static_cast<float>(neptune.pos.x / scale), static_cast<float>(neptune.pos.y / scale)));
        
        float time = static_cast<float>(t) / CLOCKS_PER_SEC;

        orbitPath(trail,earth,marsTrail, mars, jupiterTrail, jupiter, neptuneTrail, neptune, window, time);
        drawBody(sunShape, earthShape, marsShape, jupiterShape, neptuneShape, window);

        window.display();
    }
}