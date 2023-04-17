#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>

typedef long double real;
const real G = 9.8;

struct Point{

	real mass, x, y, vx, vy, ax, ay;
	Point(real _mass, real _x, real _y, real _vx = 0, real _vy = 0) : mass(_mass), x(_x), y(_y), vx(_vx), vy(_vy), ax(0), ay(0) { }
};

class System{

public:
	System(real _dt);
	void push(Point nparticle);
	void update();
	void init();
	std::vector<sf::CircleShape> getCircles();
protected:
	real acceleration(const Point& p, const std::vector<Point>& pnts, bool xy);
	std::vector<Point> points;
	size_t counter;
	real dt;
};

real System::acceleration(const Point& p, const std::vector<Point>& pnts, bool xy){

	real a = 0.0;
	for (const auto& i : pnts){
		real dv = xy ? p.x - i.x : p.y - i.y;
		if (fabs(dv) > std::numeric_limits<real>::epsilon()){

			a += -(dv < 0.0 ? -1.0 : 1.0) * dv * i.mass / dv;
		}
	}
	return a * G;
}

System::System(real _dt) : points(), counter(0), dt(_dt) { }

void System::push(Point x){

	points.push_back(x);
}

void System::init(){

	counter = 0;
	std::vector<Point> new_system;
	for (const auto& i : points){

		auto next = i;
		next.ax = acceleration(i, points, 1) / 2.0;
		next.ay = acceleration(i, points, 0) / 2.0;
		new_system.push_back(next);
	}
	points = new_system;
}

void System::update(){
	std::vector<Point> new_system;
	for (const auto& i : points){

		auto next = i;
		next.x += dt * (i.vx + dt / 2.0 * i.ax);
		next.y += dt * (i.vy + dt / 2.0 * i.ay);

		next.ax = acceleration(i, points, 1);
		next.ay = acceleration(i, points, 0);

		next.vx += dt / 2.0 * (i.ax + next.ax);
		next.vy += dt / 2.0 * (i.ay + next.ay);

		new_system.push_back(next);
	}
	points = new_system;
	++counter;
}

std::vector<sf::CircleShape> System::getCircles(){
	std::mt19937_64 c;
	c.seed(6);
	std::uniform_int_distribution<int> dist(0, 255);
	std::vector<sf::CircleShape> circles;
	for (const auto& i : points){
		sf::CircleShape circle(i.mass * 3); //изменение размеров объектов
		circle.setFillColor(sf::Color(dist(c), dist(c), dist(c)));
		circle.setPosition(i.x - i.mass * 10, i.y - i.mass * 2); //центрирование объектов
		circles.push_back(circle);
	}
	return circles;
}

int main()
{
	std::mt19937_64 g;
	g.seed(6);
	std::uniform_int_distribution<int> dist(0, 360);
	std::uniform_real_distribution<real> dr(-1.0, 1.0);

	size_t steps = 1000;
	System s(1.0 / 60.0 / steps / 0.1); //изменение tраектории объектов

	size_t ipcount = 20;
	double da = 2.0 * acos(-1.0) / ipcount;
	double r = 150.0, angle = 0.0;
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window"/*, sf::Style::Fullscreen*/);
	window.setFramerateLimit(60);
	for (size_t i = 0; i < ipcount; ++i){

		s.push(Point(0.5 * (dist(g) % 3)+1, window.getSize().x / 2.0 + r * cos(angle) + dr(g), window.getSize().y / 2.0 + r * sin(angle) + dr(g), 1.25 * dr(g), 1.25 * dr(g)));
		angle += da;
	}

	s.init();
	while (window.isOpen()){

		sf::Event event;
		while (window.pollEvent(event)){

			if (event.type == sf::Event::Closed){

				window.close();
			}
		}

		for (size_t i = 0; i < 100; i++){

			s.update();
		}

		window.clear(sf::Color::Black);

		auto circles = s.getCircles();
		for (const auto& i : circles){

			window.draw(i);
		}

		window.display();
	}

	return 0;
}
