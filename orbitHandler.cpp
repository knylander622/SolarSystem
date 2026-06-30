#include "orbitHandler.h"
using namespace std;
const double g = 6.67430e-11;
const double dt = 1000;
float scale = 2.0f;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> starXPlacement(-25000, 25000);
uniform_int_distribution<> starYPlacement(-25000, 25000);
vector<sf::Vector2f> stars;
bool draw = true;
clock_t t;

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

void orbitPath(sf::VertexArray& mercuryTrail, Sphere& mercury, sf::VertexArray& venusTrail, Sphere& venus,sf::VertexArray& trail, Sphere& earth, sf::VertexArray& marsTrail, Sphere& mars, sf::VertexArray& jupiterTrail, Sphere& jupiter, sf::VertexArray& saturnTrail, Sphere& saturn, sf::VertexArray& uranusTrail, Sphere& uranus, sf::VertexArray& neptuneTrail, Sphere& neptune, sf::RenderWindow& window, float time)
  {

        sf::Color colorEarth(static_cast<uint8_t>((sin(time) * 0.5f + 0.5f) * 2.55), static_cast<uint8_t>((sin(time + 2.0f) * 0.25f + 0.5f) * 255),static_cast<uint8_t>((sin(time + 4.0f) * 0.25f + 0.5f) * 2.55));
        sf::Color colorMars(static_cast<uint8_t>((sin(time) * 0.5f + 0.5f) * 255), static_cast<uint8_t>((sin(time + 2.0f) * 0.25f + 0.5f) * 2.55),static_cast<uint8_t>((sin(time + 4.0f) * 0.25f + 0.5f) * 2.55));
        mercuryTrail.append({sf::Vector2f(static_cast<float>(mercury.pos.x / scale), static_cast<float>(mercury.pos.y / scale))});
        venusTrail.append({sf::Vector2f(static_cast<float>(venus.pos.x / scale), static_cast<float>(venus.pos.y / scale))});
        trail.append({sf::Vector2f(static_cast<float>(earth.pos.x / scale), static_cast<float>(earth.pos.y / scale))});
        marsTrail.append({sf::Vector2f(static_cast<float>(mars.pos.x / scale), static_cast<float>(mars.pos.y / scale))});
        jupiterTrail.append({sf::Vector2f(static_cast<float>(jupiter.pos.x / scale), static_cast<float>(jupiter.pos.y / scale))});
        saturnTrail.append({sf::Vector2f(static_cast<float>(saturn.pos.x / scale), static_cast<float>(saturn.pos.y / scale))});
        uranusTrail.append({sf::Vector2f(static_cast<float>(uranus.pos.x / scale), static_cast<float>(uranus.pos.y / scale))});
        neptuneTrail.append({sf::Vector2f(static_cast<float>(neptune.pos.x / scale), static_cast<float>(neptune.pos.y / scale))});
        trail[trail.getVertexCount() - 1].color = colorEarth;
        marsTrail[marsTrail.getVertexCount() - 1].color = colorMars;
        window.draw(mercuryTrail);
        window.draw(venusTrail);
        window.draw(trail);
        window.draw(marsTrail);
        window.draw(jupiterTrail);
        window.draw(saturnTrail);
        window.draw(uranusTrail);
        window.draw(neptuneTrail);
  }

void drawBody(sf::CircleShape& sunShape, sf::CircleShape& mercuryShape, sf::CircleShape& venusShape, sf::CircleShape& earthShape, sf::CircleShape& marsShape, sf::CircleShape& jupiterShape, sf::CircleShape& saturnShape, sf::CircleShape& uranusShape, sf::CircleShape& neptuneShape, sf::RenderWindow& window)
  {
        window.draw(sunShape);
        window.draw(mercuryShape);
        window.draw(venusShape);
        window.draw(earthShape);
        window.draw(marsShape);
        window.draw(jupiterShape);
        window.draw(saturnShape);
        window.draw(uranusShape);
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
