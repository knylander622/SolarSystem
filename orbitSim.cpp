#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <random>
#include <ctime>
#include <time.h>
#include <cstdlib>
#include <cmath>
#include <windows.h>
using namespace std;

const double g = 1.0;//6.67430e-11;
const double dt = 1;
float scale = 2.0f;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> starXPlacement(-25000, 25000);
uniform_int_distribution<> starYPlacement(-25000, 25000);
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
          for(int i = 0;i < 30000;i++)
          {
            int x = starXPlacement(gen);
            int y = starYPlacement(gen);
            stars.push_back(sf::Vector2f(static_cast<float>(x),static_cast<float>(y)));
          }
  }

  void orbitPath(sf::VertexArray& trail, Sphere& earth, sf::VertexArray& marsTrail, Sphere& mars, sf::VertexArray& jupiterTrail, Sphere& jupiter, sf::VertexArray& neptuneTrail, Sphere& neptune, sf::RenderWindow& window, float time)
  {

        sf::Color colorEarth(static_cast<uint8_t>((sin(time) * 0.5f + 0.5f) * 2.55), static_cast<uint8_t>((sin(time + 2.0f) * 0.25f + 0.5f) * 255),static_cast<uint8_t>((sin(time + 4.0f) * 0.25f + 0.5f) * 2.55));
        trail.append({sf::Vector2f(static_cast<float>(earth.pos.x / scale), static_cast<float>(earth.pos.y / scale))});
        marsTrail.append({sf::Vector2f(static_cast<float>(mars.pos.x / scale), static_cast<float>(mars.pos.y / scale))});
        jupiterTrail.append({sf::Vector2f(static_cast<float>(jupiter.pos.x / scale), static_cast<float>(jupiter.pos.y / scale))});
        neptuneTrail.append({sf::Vector2f(static_cast<float>(neptune.pos.x / scale), static_cast<float>(neptune.pos.y / scale))});
        trail[trail.getVertexCount() - 1].color = colorEarth;
        window.draw(trail);
        window.draw(marsTrail);
        window.draw(jupiterTrail);
        window.draw(neptuneTrail);
  }

  void drawBody(sf::CircleShape& sunShape, sf::CircleShape& earthShape, sf::CircleShape& marsShape, sf::CircleShape& jupiterShape, sf::CircleShape& neptuneShape, sf::RenderWindow& window)
  {
        window.draw(sunShape);
        window.draw(earthShape);
        window.draw(marsShape);
        window.draw(jupiterShape);
        window.draw(neptuneShape);
  }

void edgeQuestion(sf::RenderWindow& window, sf::View& view)
{
  const int pixMargin = 200;
  const float pan = 100;

  sf::Vector2i mouse = sf::Mouse::getPosition(window);
  sf::Vector2u size = window.getSize();
  sf::Vector2f movement(0.f,0.f);

  if(mouse.x <= pixMargin)
    movement.x -= pan;
  else if(mouse.x >= static_cast<int>(size.x) - pixMargin)
    movement.x += pan;
  if(mouse.y <= pixMargin)
    movement.y -= pan;
  else if(mouse.y >= static_cast<int>(size.y) - pixMargin)
  movement.y += pan;

  if(movement.x != 0 || movement.y != 0)
    view.move(movement);

}

int main()
{
    

    Sphere sun = {{0,0}, {0,0}, {0,0}, 1000};
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
    sf::RenderWindow window(deskWindow/*sf::VideoMode(sf::Vector2u(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)))*/, "Solar System", sf::State::Fullscreen);
    sf::View view(sf::Vector2f(0.5f, 0.5f), sf::Vector2f(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)));

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