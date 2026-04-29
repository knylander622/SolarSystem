#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <time.h>
using namespace std;

const double g = 1.0;//6.67430e-11;
const double dt = .25;
float scale = 2.0f;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> starCount(1, 100);
uniform_int_distribution<> starXPlacement(-960,960);
uniform_int_distribution<> starYPlacement(-540, 540);
vector<sf::Vector2f> stars;
bool draw = true;
clock_t t;

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

  void initializeOrbit(Sphere& p, const Sphere& sun, double dt)
{
    double r = length(p.pos);
    double speed = sqrt(g * sun.mass / r);

    vect dir = {p.pos.y, p.pos.x};
    p.vel = normalize(dir) * speed;

    vect acc = computeGravity(p, sun);
    p.prevPos = p.pos - p.vel * dt + acc * (0.5 * dt * dt);
}

  void drawStars(sf::RenderWindow& window, sf::RectangleShape& star)
  {
    star.setFillColor(sf::Color::White);
          for(int i = 0;i < 2500;i++)
          {
            int x = starXPlacement(gen);
            int y = starYPlacement(gen);
            stars.push_back(sf::Vector2f(static_cast<float>(x),static_cast<float>(y)));
          }
  }

  void orbitPath(sf::VertexArray& trail, Sphere& earth, sf::VertexArray& marsTrail, Sphere& mars, sf::RenderWindow& window, float time)
  {

        sf::Color colorEarth(static_cast<uint8_t>((sin(time) * 0.5f + 0.5f) * 2.55), static_cast<uint8_t>((sin(time + 2.0f) * 0.25f + 0.5f) * 255),static_cast<uint8_t>((sin(time + 4.0f) * 0.25f + 0.5f) * 2.55));
        trail.append({sf::Vector2f(static_cast<float>(earth.pos.x / scale), static_cast<float>(earth.pos.y / scale))});
        marsTrail.append({sf::Vector2f(static_cast<float>(mars.pos.x / scale), static_cast<float>(mars.pos.y / scale))});
        //jupiterTrail.append({sf::Vector2f(static_cast<float>(jupiter.pos.x / scale), static_cast<float>(jupiter.pos.y / scale))});
        trail[trail.getVertexCount() - 1].color = colorEarth;
        window.draw(trail);
        window.draw(marsTrail);
        //window.draw(jupiterTrail);
  }

  void drawBody(sf::CircleShape& sunShape, sf::CircleShape& earthShape, sf::CircleShape& marsShape, sf::RenderWindow& window)
  {
        window.draw(sunShape);
        window.draw(earthShape);
        window.draw(marsShape);
        //window.draw(jupiterShape);
  }

int main()
{
    

    Sphere sun = {{0,0}, {0,0}, {0,0}, 1000};
    Sphere earth = {{0,-400}, {0,0}, {0,0}, 100};
    Sphere mars = {{0,-608}, {0,0}, {0,0}, 10};
    Sphere venus = {{0, -288}, {0,0}, {0,0}, 81.5};
    Sphere jupiter = {{0, -2080}, {0,0}, {0,0}, 50};
    

    initializeOrbit(earth, sun, dt);
    initializeOrbit(mars, sun, dt);
    initializeOrbit(venus, sun, dt);
    initializeOrbit(jupiter, sun, dt);

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1920, 1080)), "Solar System");
    sf::View view(sf::Vector2f(0.5f, 0.5f), sf::Vector2f(1920.0f,1080.0f));

    sf::VertexArray trail(sf::PrimitiveType::Points);
    sf::VertexArray marsTrail(sf::PrimitiveType::Points);
    sf::VertexArray jupiterTrail(sf::PrimitiveType::Points);

    sf::RectangleShape star({2.0f,2.0f});
    star.setRotation(sf::degrees(45.f));

    sf::CircleShape sunShape(10.9);
    sunShape.setFillColor(sf::Color(255, 255, 0));
    sunShape.setOrigin(sf::Vector2f(10.0f,10.0f));

    sf::CircleShape earthShape(1);
    earthShape.setFillColor(sf::Color(144, 238, 144));
    earthShape.setOrigin(sf::Vector2(earthShape.getGeometricCenter()));

    sf::CircleShape marsShape(.53);
    marsShape.setFillColor(sf::Color(255, 63, 52));
    marsShape.setOrigin(sf::Vector2(marsShape.getGeometricCenter()));

    sf::CircleShape jupiterShape(1);
    marsShape.setFillColor(sf::Color(255, 63, 52));
    marsShape.setOrigin(sf::Vector2(jupiterShape.getGeometricCenter()));

    while (window.isOpen())
    { 
        while (auto event = window.pollEvent())
        {
        if (event->is<sf::Event::Closed>())
        window.close();
        }

        t = clock();

        fixView(window, view);
        window.setView(view);

        updateV(earth, sun, dt);
        updateV(mars, sun, dt);
        updateV(jupiter,sun,dt);

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
        
        float time = static_cast<float>(t) / CLOCKS_PER_SEC;

        orbitPath(trail,earth,marsTrail, mars, window, time);
        drawBody(sunShape, earthShape, marsShape, window);

        window.display();
    }
}