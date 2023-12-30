#include "CircleCollider.h"

CircleCollider::CircleCollider(float radius, std::size_t vertexCount, sf::Color color)
    : radius{ radius }, vertexCount{ vertexCount }, color{ color }
{
    this->update();
    this->updateColor();
}

void CircleCollider::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // no texture
    states.texture = NULL;

    // draw the vertex array
    target.draw(this->vertices, states);
}

sf::Vector2f CircleCollider::getPoint(std::size_t index) const
{
    const float PI = 3.14159265f;

    // get the angle based on the index (offsetted by -90)
    const float angle = static_cast<float>(index) / static_cast<float>(this->vertexCount) * 360.f - 90.f;
    const float angleInRadians = angle * PI / 180.f;

    // convert the angle and radius to a position
    return sf::Vector2f{ this->radius * std::cos(angleInRadians), this->radius * std::sin(angleInRadians) };
}

//sf::Vector2f CircleCollider::getCenter() const
//{
//    auto [maxX, maxY] = this->vertices[0].position;
//    auto [minX, minY] = this->vertices[0].position;
//
//    for (std::size_t i = 0; i < this->vertexCount; ++i)
//    {
//        minX = std::min(minX, this->vertices[i].position.x);
//        maxX = std::max(maxX, this->vertices[i].position.x);
//        minY = std::min(minY, this->vertices[i].position.y);
//        maxY = std::max(maxY, this->vertices[i].position.y);
//    }
//
//    return { (minX + maxX) / 2.f, (minY + maxY) / 2.f };
//}

int CircleCollider::getVertexCount() const
{
    return this->vertexCount;
}

float CircleCollider::getRadius() const
{
    return this->radius;
}

void CircleCollider::setVertexCount(const std::size_t newVertexCount)
{
    this->vertexCount = newVertexCount;
    this->update();
}

void CircleCollider::setRadius(const float newRadius)
{
    this->radius = newRadius;
    this->update();
}

void CircleCollider::setColor(const sf::Color newColor)
{
    this->color = newColor;
    this->updateColor();
}

void CircleCollider::update()
{
    // vertexCount + 1 => one extra vertex for linking the line strip to the first vertex
    this->vertices.resize(this->vertexCount + 1);

    for (std::size_t i = 0; i < this->vertexCount; ++i)
        // map indices to a position (x, y)
        this->vertices[i].position = this->getPoint(i);
    // finish polygon by setting extra vertex to start vertex
    this->vertices[this->vertexCount].position = this->vertices[0].position;

    // the origin for the collider will already be in the center of 
    // its local coordinate system because of the way it is build => no need to set it
}

void CircleCollider::updateColor()
{
    for (std::size_t i = 0; i <= this->vertexCount; ++i)
    {
        this->vertices[i].color = this->color;
    }
}

sf::Vector2f CircleCollider::getGlobalPosition(const std::size_t index) const
{
    return this->getTransform().transformPoint(this->vertices[index].position);
}

bool CircleCollider::intersects(const sf::Vector2f& point) const
{
    // this is local position, in the local coordinate system
    // convert it to global coordinate system

    int intersections = 0;
    sf::Vector2f pos1 = this->getTransform().transformPoint(this->vertices[this->vertexCount - 1].position);
    for (std::size_t i = 0; i < this->vertexCount; ++i)
    {
        sf::Vector2f pos2 = this->getTransform().transformPoint(this->vertices[i].position);
        float minY = std::min(pos1.y, pos2.y);
        float maxY = std::max(pos1.y, pos2.y);
        float minX = std::min(pos1.x, pos2.x);
        float maxX = std::max(pos1.x, pos2.x);
        if (point.x <= maxX
            && point.y >= minY 
            && point.y <= maxY)
        {
            if (pos1.y == pos2.y) ++intersections;
            else
            {
                // (vertex.position.y - minY) / (maxY - minY) => get proportion for vertex.y 
                // ((vertex.position.y - minY) / (maxY - minY)) * (maxX - minX) = find out how much do you need to add to minX to get intersectionX
                float possibleIntersectionX 
                    = minX + ((point.y - minY) / (maxY - minY)) * (maxX - minX);

                // if vertex.x is not further than that point
                if (point.x <= possibleIntersectionX) ++intersections;
            }
        }
        pos1 = pos2;
    }

    // inside polygon if intersections are odd
    return (intersections % 2 == 1);
}

bool CircleCollider::intersects(const CircleCollider& other) const
{
    for (std::size_t i = 0; i < other.vertices.getVertexCount(); ++i)
        if (this->intersects(other.getGlobalPosition(i))) return true;
    return false;
}
