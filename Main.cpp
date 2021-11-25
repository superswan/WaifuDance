#include <iostream>
#include "SFML/Graphics.hpp"    
#include <Windows.h>
#include <Dwmapi.h>
#include <ShObjIdl.h>

#pragma comment (lib, "Dwmapi.lib")

enum animation_default {
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

enum animation_modern {
    gumi = 67,
    gumi_running = 130,
    gumi_breeze = 260,
    gumi_bicycle = 385,
    gumi_wave = 520,
    gumi_fallover = 650,
    cirno_hold = 1330,
    cirno_sway = 1590,
    cirno_jump = 1795,
    cirno_wave = 2100,
    cirno_zombie = 2350,
    cirno_spin = 2645,
    green = 1680,
    pink = 1818,
    red = 1945,
    orange = 2075,
};


int main(int argc, char** argv)  { 
    //configuration options
    int animation = cirno_hold;
    int rate = 105;
    int frames = 8;
    int sheet_width = 880;
    int sheet_width2 = sheet_width*2;
    int rectangle_left = sheet_width2 / frames;
    char stylesheet[] = "images/sheet2.png";
    
    // WINDOW
    sf::RenderWindow renderWindow(sf::VideoMode(220, 205), "Waifu Dance", sf::Style::None);

    HWND hwnd = renderWindow.getSystemHandle();

    MARGINS margins;
    margins.cxLeftWidth = -1;
    
    SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    // Remove icon from taskbar
    ITaskbarList* pTaskList = NULL;
    HRESULT initRet = CoInitialize(NULL);
    HRESULT createRet = CoCreateInstance(CLSID_TaskbarList,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_ITaskbarList,
        (LPVOID*)&pTaskList);

    if (createRet == S_OK)
    {

        pTaskList->DeleteTab(hwnd);

        pTaskList->Release();
    }

    CoUninitialize();
    
    // Render loop and animation settings
    sf::Event event;
    sf::Texture texture;
    sf::IntRect rectSource(0, animation, 220, 205);
    texture.loadFromFile(stylesheet);
    
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
            rectSource.left += 220;

            if (rectSource.left == 1760) {
                rectSource.left = 0;
            } 
            
            clock.restart();
        }

        renderWindow.clear(sf::Color::Transparent);
        renderWindow.draw(sprite);
        renderWindow.display();
    }
}