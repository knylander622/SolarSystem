#ifndef ORBITHANDLER_H
#define ORBITHANDLER_H
#ifdef _WIN32
  #include <windows.h>
#endif
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

extern const double g;
extern const double dt;
extern float scale;
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_int_distribution<> starXPlacement;
extern std::uniform_int_distribution<> starYPlacement;
extern std::vector<sf::Vector2f> stars;
extern bool draw;
extern clock_t t;

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

double length(const vect& v);
vect normalize(const vect& v);
vect computeGravity(const Sphere& planet, const Sphere& sun);
void updateV(Sphere& planet, const Sphere& sun, double dt);
void fixView(sf::RenderWindow & window, sf::View&view);
void initializeOrbit(Sphere& p, const Sphere& sun, double dt);
void drawStars(sf::RenderWindow& window, sf::RectangleShape& star);
void orbitPath(sf::VertexArray& trail, Sphere& earth, sf::VertexArray& marsTrail, Sphere& mars, sf::VertexArray& jupiterTrail, Sphere& jupiter, sf::VertexArray& neptuneTrail, Sphere& neptune, sf::RenderWindow& window, float time);
void drawBody(sf::CircleShape& sunShape, sf::CircleShape& earthShape, sf::CircleShape& marsShape, sf::CircleShape& jupiterShape, sf::CircleShape& neptuneShape, sf::RenderWindow& window);
void edgeQuestion(sf::RenderWindow& window, sf::View& view);

#endif
