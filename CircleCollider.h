#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class CircleCollider : public sf::Drawable, public sf::Transformable
{
private:

	sf::VertexArray vertices{ sf::LinesStrip };
	float radius;
	std::size_t vertexCount;
	sf::Color color;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Vector2f getPoint(std::size_t index) const;
	//sf::Vector2f getCenter() const;

public:
	// TODO:: maybe create a base collider class so the intersect could check collisions with other types of colliders

	CircleCollider() : radius{ 0 }, vertexCount{ 30 }, color{ sf::Color::Red } {}
	CircleCollider(float radius, std::size_t vertexCount = 30, sf::Color color = sf::Color::Red);

	int getVertexCount() const;
	float getRadius() const;

	void setVertexCount(const std::size_t newVertexCount);
	void setRadius(const float newRadius);
	void setColor(const sf::Color newColor);

	void update();
	void updateColor();

	// return vertex position at the given index, conveted to global coordinate system
	sf::Vector2f getGlobalPosition(const std::size_t index) const;

	// point needs to be already converted to global coordinate system for the intersect to work
	bool intersects(const sf::Vector2f& point) const;
	bool intersects(const CircleCollider& collider) const;

};