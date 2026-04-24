#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <random>
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

  void orbitPath(sf::VertexArray& trail, Sphere& earth, Sphere& mars)
  {
        trail.append({sf::Vector2f(static_cast<float>(earth.pos.x / scale), static_cast<float>(earth.pos.y / scale))});
        trail.append({sf::Vector2f(static_cast<float>(mars.pos.x / scale), static_cast<float>(mars.pos.y / scale))});
  }

int main()
{
    Sphere sun = {{0,0}, {0,0}, {0,0}, 1000};
    Sphere earth = {{0,-400}, {0,0}, {0,0}, 100};
    Sphere mars = {{0,-608}, {0,0}, {0,0}, 38};
    

    initializeOrbit(earth, sun, dt);
    initializeOrbit(mars, sun, dt);

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1920, 1080)), "Orbit");
    sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(1920.0f,1080.0f));

    sf::VertexArray trail(sf::PrimitiveType::Points);
    sf::VertexArray marsTrail(sf::PrimitiveType::Points);
    sf::RectangleShape star({2.0f,2.0f});

    sf::CircleShape sunShape(10);
    sunShape.setFillColor(sf::Color(255, 255, 0));
    sunShape.setOrigin(sf::Vector2f(10.0f,10.0f));

    sf::CircleShape earthShape(5);
    earthShape.setFillColor(sf::Color(144, 238, 144));
    earthShape.setOrigin(sf::Vector2(5.0f,5.0f));

    sf::CircleShape marsShape(4.5);
    marsShape.setFillColor(sf::Color(255, 63, 52));
    marsShape.setOrigin(sf::Vector2(5.0f,5.0f));

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
        updateV(mars, sun, dt);

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
        window.draw(sunShape);

        earthShape.setPosition(sf::Vector2f(static_cast<float>(earth.pos.x / scale), static_cast<float>(earth.pos.y / scale)));
        window.draw(earthShape);

        marsShape.setPosition(sf::Vector2f(static_cast<float>(mars.pos.x / scale), static_cast<float>(mars.pos.y / scale)));
        window.draw(marsShape);
        
        orbitPath(trail,earth,mars);

        window.draw(trail);
        window.draw(marsTrail);
        window.display();
    }
}