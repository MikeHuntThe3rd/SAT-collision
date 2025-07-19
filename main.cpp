#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
void AddIfNew(std::vector<std::pair<float, float>>& vectors, std::pair<float, float> add) {
    for (std::pair<float, float> curr: vectors) {
        float det = curr.first * add.second - curr.second * add.first;
        if (det == 0)return;
    }
    vectors.push_back(add);
}
bool intersects(std::pair<std::vector<std::pair<float, float>>, std::vector<std::pair<float, float>>> points) {
    //getting the possibles axis-es:
    std::vector<std::pair<float, float>> vectors;
    for (int i = 0; i < points.first.size();i++) {
        if (i == points.first.size() - 1) {
            float x = points.first[0].first - points.first[i].first;
            float y = points.first[0].second - points.first[i].second;
            AddIfNew(vectors, { x, y });
        }
        else {
            float x = points.first[i + 1].first - points.first[i].first;
            float y = points.first[i + 1].second - points.first[i].second;
            AddIfNew(vectors, { x, y });
        }
    }
    for (int i = 0; i < points.second.size();i++) {
        if (i == points.second.size() - 1) {
            float x = points.second[0].first - points.second[i].first;
            float y = points.second[0].second - points.second[i].second;
            AddIfNew(vectors, { x, y });
        }
        else {
            float x = points.second[i + 1].first - points.second[i].first;
            float y = points.second[i + 1].second - points.second[i].second;
            AddIfNew(vectors, { x, y });
        }
    }
    //main check:
    std::pair<std::pair<float, float>, std::pair<float, float>> distances = { {0.0f, 0.0f}, {0.0f, 0.0f} };
    for (std::pair<float, float> axis: vectors) {
        //initial vector
        float Ax = axis.first;
        float Ay = axis.second;
        //rotated 90 degrees
        float Bx = axis.second;
        float By = axis.first * -1;
        float det = Ax * By - Ay * Bx;
        float min1 = std::numeric_limits<float>::max();
        float max1 = std::numeric_limits<float>::min();
        for (std::pair<float, float> point : points.first) {
            float multiplyer = (point.first * By - point.second * Bx) / det;
            float projected = Ax * (point.first + multiplyer * Ax) + Ay * (point.second + multiplyer * Ay);
            min1 = std::min(min1, projected);
            max1 = std::max(max1, projected);
        }

        // Projections for second shape
        float min2 = std::numeric_limits<float>::max();
        float max2 = std::numeric_limits<float>::min();
        for (std::pair<float, float> point : points.second) {
            float multiplyer = (point.first * By - point.second * Bx) / det;
            float projected = Ax * (point.first + multiplyer * Ax) + Ay * (point.second + multiplyer * Ay);
            min2 = std::min(min2, projected);
            max2 = std::max(max2, projected);
        }

        // Check for overlap
        if (max1 < min2 || max2 < min1) {
            return false;  // Separating axis found
        }
    }
    return true;
}
std::vector<std::pair<float, float>> FindEdges(sf::RectangleShape obj, std::pair<float, float> push = {0.0f, 0.0f}) {
    std::vector<std::pair<float, float>> curr;
    sf::Transform transform = obj.getTransform();
    size_t pointCount = obj.getPointCount();
    for (size_t i = 0; i < pointCount; ++i) {
        sf::Vector2f localPoint = obj.getPoint(i);
        sf::Vector2f worldPoint = transform.transformPoint(localPoint);
        curr.push_back({worldPoint.x, worldPoint.y});
    }
    return curr;
}

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "amongus", sf::Style::None);
    //sf::RenderWindow window(sf::VideoMode({1000, 500}), "amongus", sf::Style::None);
    sf::RectangleShape rect1(sf::Vector2f(100.f, 100.f));
    rect1.setPosition({ 500.f, 200.f });
    rect1.setFillColor(sf::Color::Green);

    sf::RectangleShape rect2(sf::Vector2f(100.f, 100.f));
    rect2.setPosition({ 250.f, 250.f });
    rect2.setFillColor(sf::Color::Red);
    sf::Vector2f prev = rect1.getPosition();
    sf::Vector2f objcords = rect1.getPosition();
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                rect1.rotate(sf::degrees(5));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) window.close();
        }
        sf::Vector2f mouse = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

        prev = objcords;

        if (objcords.x < mouse.x && !intersects({ FindEdges(rect1, {1.0f, 0.0f}), FindEdges(rect2) })) {
            objcords.x += 1.0f;
        }
        else if (objcords.x > mouse.x && !intersects({ FindEdges(rect1, {-1.0f, 0.0f}), FindEdges(rect2) })) {
            objcords.x -= 1.0f;
        }

        if (objcords.y < mouse.y && !intersects({ FindEdges(rect1, {0.0f, 1.0f}), FindEdges(rect2) })) {
            objcords.y += 1.0f;
        }
        else if (objcords.y > mouse.y && !intersects({ FindEdges(rect1, {0.0f, -1.0f}), FindEdges(rect2) })) {
            objcords.y -= 1.0f;
        }

        rect1.setPosition(objcords);

        if (intersects({ FindEdges(rect1), FindEdges(rect2) })) {
            rect1.setPosition(prev);
            objcords = prev;
        }
        window.clear();
        window.draw(rect1);
        window.draw(rect2);
        window.display();
    }

    return 0;
}
