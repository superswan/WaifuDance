#include <iostream>
#include "SFML/Graphics.hpp"    
#include <Windows.h>
#include <Dwmapi.h>

#pragma comment (lib, "Dwmapi.lib")

enum animation_set {
    walking = 0,
    running = 130,
    breeze = 260,
    bicycle = 385,
    wave = 520,
    fallover = 650,
    dancing = 775,
    jumping = 900,
    karate = 1035,
    wave2 = 1160,
    running2 = 1290,
    detach = 1415,
    explosion = 1538,
    dance1 = 1680,
    miku = 1818,
    dance2 = 1945,
    dance3 = 2075,
    dance4 = 2200,
    dance5 = 2330,
    swimming = 2440
};

int main(int argc, char** argv)  {    
    int animation = swimming;

    int rate = 125;

    sf::RenderWindow renderWindow(sf::VideoMode(110, 125), "Waifu Dance", sf::Style::None);

    MARGINS margins;
    margins.cxLeftWidth = -1;

    SetWindowLong(renderWindow.getSystemHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
    DwmExtendFrameIntoClientArea(renderWindow.getSystemHandle(), &margins);
    
    sf::Event event;
    sf::Texture texture;
    sf::IntRect rectSource(0, animation, 110, 124);
    texture.loadFromFile("images/sheet1.png");
    
    sf::Sprite sprite(texture, rectSource);
    sf::Clock clock;


    bool grabbed = false;
    sf::Vector2i grabbedOffset;
    while (renderWindow.isOpen()) {
        while (renderWindow.pollEvent(event)) {
            if (event.type == sf::Event::EventType::Closed) {
                renderWindow.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    grabbed = true;
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left)
                    grabbed = false;
                    grabbedOffset = renderWindow.getPosition() - sf::Mouse::getPosition();
            }
            else if (event.type == sf::Event::MouseMoved && grabbed) {
                renderWindow.setPosition(sf::Mouse::getPosition() + grabbedOffset);
            }
            
        }
        
        if (clock.getElapsedTime().asMilliseconds() >= rate) {
            sprite.setTextureRect(rectSource);
            rectSource.left += 110;

            if (rectSource.left == 880) {
                rectSource.left = 0;
            } 
            
            clock.restart();
        }

        renderWindow.clear(sf::Color::Transparent);
        renderWindow.draw(sprite);
        renderWindow.display();
    }
}