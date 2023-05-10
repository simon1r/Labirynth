#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;

sf::Texture loadTexture(const string& path)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path))
    {
        cerr << "Couldn't load the texture " << path << endl;
    }
    return texture;
}

class BoxCollider : public sf::RectangleShape
{
public:
    float width, height;
    sf::Vector2f spriteTrans, s;
    sf::Sprite sprite;

    BoxCollider() = default;

    void setTexture(const sf::Vector2u& w, const sf::Texture& t, float tsx, float wsx, float tsy = 0, float wsy = 0)
    {
        sprite.setTexture(t);
        if (tsy == 0) {
            sprite.setScale(static_cast<float>(w.x) / wsx / tsx, static_cast<float>(w.x) / wsx / tsx);
            width = w.x / wsx;
            height = width;
        }
        else {
            sprite.setScale(static_cast<float>(w.x) / wsx / tsx, static_cast<float>(w.y) / wsy / tsy);
            width = w.x / wsx;
            height = w.y / wsy;
        }
        spriteTrans = sf::Vector2f(-width / 2.0, -height / 2.0);
    }

    sf::Sprite getSprite()
    {
        sprite.setPosition(getPosition() + spriteTrans);
        return sprite;
    }
};

class Player : public BoxCollider
{
public:
    Player(const sf::Vector2f& position, float speed)
    {
        setPosition(position);
        this->speed = speed;
    }

    void moveInDirection(const sf::Time& elapsed, const sf::Keyboard::Key& key, const vector<BoxCollider>& colliders)
    {
        sf::Vector2f pos = getPosition();
        sf::FloatRect size = getGlobalBounds();
        float x = 0, y = 0;

        if (key == sf::Keyboard::S)
            y = speed;
        else if (key == sf::Keyboard::W)
            y = -speed;
        else if (key == sf::Keyboard::D)
            x = speed;
        else if (key == sf::Keyboard::A)
            x = -speed;

        x *= elapsed.asSeconds();
        y *= elapsed.asSeconds();

        for (const BoxCollider& c : colliders)
        {
            sf::Vector2f d = c.getPosition() - (getPosition() + sf::Vector2f(x, y));

            if ((abs(d.x) < (width / 2.0 + c.width / 2.0)) && (abs(d.y) < (c.height / 2.0 + height / 2.0)))
            {
                x = 0;
                y = 0;
                cout << d.x << " " << d.y << "\t" << width << "\t" << c.width << endl;
            }
        }

        if (pos.y + y - height / 2.0 < 0 || pos.y + height / 2.0 + y > maxY)
            y = 0;
        if (pos.x + x - width / 2.0 < 0 || pos.x + width / 2.0 + x > maxX)
            x = 0;

        setPosition(pos.x + x, pos.y + y);
    }

        void setBounds(float minX, float minY, float maxX, float maxY)
    {
        this->minX = minX;
        this->maxX = maxX;
        this->minY = minY;
        this->maxY = maxY;
    }

private:
    float speed;
    float minX, minY, maxX, maxY;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 550), "laab");
    sf::Vector2u w = window.getSize();

    sf::Texture wall = loadTexture("wall.png");
    sf::Texture grass = loadTexture("grass.jpg");
    sf::Texture p = loadTexture("guy.png");

    Player player(sf::Vector2f(w.x / 2.0, w.y / 2.0), 350);
    player.setBounds(0, 0, window.getSize().x, window.getSize().y);
    player.setTexture(window.getSize(), p, 500, 12);
    player.height *= 379 / 277.0;
    player.spriteTrans.y *= 379 / 277.0;

    int mapS = 4;
    vector<sf::Sprite> bg;
    for (int x = 0; x < mapS; x++)
    {
        for (int y = 0; y < mapS; y++)
        {
            sf::Sprite sprite;
            sprite.setTexture(grass);
            sprite.setScale(static_cast<float>(w.x) / mapS / 1134.0, static_cast<float>(w.x) / mapS / 1134.0);
            sprite.setPosition(w.x / static_cast<float>(mapS) * x, w.y / static_cast<float>(mapS) * y);
            bg.push_back(sprite);
        }
    }

    int pos[]{2, 0, 6, 0,
              1, 1, 2, 1, 4, 1,
              4, 2, 6, 2, 7, 2,
              0, 3, 1, 3, 3, 3,
              4, 4, 5, 4, 6, 4,
              0, 5, 2, 5, 3, 5, 6, 5,
              1, 7, 2, 7, 7, 7};

    int wallS = 7;
    vector<BoxCollider> map;
    for (int i = 0; i < 14; i++)
    {
        BoxCollider a;
        a.setTexture(w, wall, 300, wallS);
        a.setPosition(w.x / wallS * pos[i * 2] + a.width / 2.0, w.x / wallS * pos[i * 2 + 1] + a.height / 2.0);
        map.push_back(a);
    }

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Keyboard::Key keys[4] = {sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D};
        for (int i = 0; i < 4; i++)
        {
            if (sf::Keyboard::isKeyPressed(keys[i]))
                player.moveInDirection(elapsed, keys[i], map);
        }

                window.clear(sf::Color::Black);
        for (const auto& s : bg)
        {
            window.draw(s);
        }
        for (const auto& s : map)
        {
            window.draw(s.getSprite());
        }
        window.draw(player.getSprite());
        window.display();
    }

    return 0;
}


   
