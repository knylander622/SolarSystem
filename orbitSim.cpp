#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;

struct vect
{
  double x,y;
  vect operator+(const vect& other) const {
    return {x + other.x, y + other.y};
  }

  vect operator-(const vect& other) const{
    return {x - other.x, y - other.y};
  }

  vect operator*(double scalar) const{
    return {x * scalar, y * scalar};
  }
};

struct Sphere{
    vect pos;
    vect prevPos;
    vect vel;
    double mass;
};

double length(const vect& v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

  vect normalize(const vect& v)
  {
    double len = length(v);
    return {v.x / len, v.y / len};
  }

  vect computeGravity(const Sphere& planet, const Sphere& sun)
  {
    const double g = 1.0;//6.67430e-11;

    vect dir = sun.pos - planet.pos;
    double dist = length(dir) + 1e-6;

    vect forceDir = normalize(dir);

    double forceMag = g * planet.mass * sun.mass / (dist * dist);

    return forceDir * (forceMag / planet.mass);
  }

  void updateV(Sphere& planet, const Sphere& sun, double dt)
  {
    vect acc = computeGravity(planet, sun);
    vect newPos = planet.pos * 2 - planet.prevPos + acc * (dt * dt);

    planet.prevPos = planet.pos;
    planet.pos = newPos;
  }

  void fixView(sf::RenderWindow & window, sf::View&view)
  {
    float windWidth = static_cast<float>(window.getSize().x);
    float windHeight = static_cast<float>(window.getSize().y);
    float windRatio = windWidth / windHeight;
    float viewRatio = view.getSize().x / view.getSize().y;

    float sizeX = 1.f;
    float sizeY = 1.f;
    float posX = 0.f;
    float posY = 0.f;

    if(windRatio > viewRatio)
    {
        sizeX = viewRatio / windRatio;
        posX = (1.f - sizeX) / 2.f;
    }
    else
    {
        sizeY = windRatio / viewRatio;
        posY = (1.f - sizeY) / 2.f;
    }
    view.setViewport(sf::Rect<float>(sf::Vector2f(posX, posY),sf::Vector2f(sizeX,sizeY)));
  }

int main()
{
    double dt = .01;

    Sphere sun = {{0,0}, {0,0}, {0,0}, 1000};

    Sphere earth = {{0,-100}, {0,0}, {-3.16,0}, 1};

    earth.prevPos = earth.pos - earth.vel * dt;

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 800)), "Orbit");
    sf::View view(sf::Vector2f(400.0f, 400.0f), sf::Vector2f(800.0f,800.0f));

    float scale = 2.0f;

    while (window.isOpen())
    {
        
        while (auto event = window.pollEvent())
        {
        if (event->is<sf::Event::Closed>())
        window.close();
        }
        fixView(window, view);
        window.setView(view);

        updateV(earth, sun, dt);

        window.clear();

        // draw sun
        sf::CircleShape sunShape(5);

        sunShape.setOrigin(sf::Vector2f(5.0,5.0));
        sunShape.setPosition(sf::Vector2f(400.0, 400.0));
        window.draw(sunShape);

        // draw planet
        sf::CircleShape planetShape(3);
        float posx = 400.0f + static_cast<float>(earth.pos.x / scale);
        float posy = 400.0f + static_cast<float>(earth.pos.y / scale);
        planetShape.setPosition(sf::Vector2f(posx,posy));
        window.draw(planetShape);

        window.display();
    }
}