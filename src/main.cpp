#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include "Camera.hpp"
#include "Chunk.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

const float FPS = 60;
const float DT  = 1.0/FPS;
const float WORLD_RADIUS = 200;
const float CAM_SPEED = 30;

template<class T,int N>
std::ostream& operator<<(std::ostream& o,const Vector<T,N>& v) {
    o << "Vec" << N << "{";
    bool first = true;
    for(auto x:v.values) {
        if(!first) {
            o << ",";
        }
        first = false;
        o << x;
    }
    o << "}";
    return o;
}

void initViewport(int width,int height) {
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(height != 0) ? width/(float)height : 1,0.1,100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
float frand() {
    return rand()/(float)RAND_MAX;
}

int main() {
    sf::Window window(sf::VideoMode(640,480),
                      "JackOfClubs test");
    bool mouseCaptured = false;
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2i windowCenter(windowSize.x/2,
                              windowSize.y/2);
    int windowWidth  = windowSize.x,
        windowHeight = windowSize.y;
    Chunk chunk;
    Chunk::generate(chunk);

    glClearColor(0,0,0,0);
    glClearDepth(1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    initViewport(windowSize.x,windowSize.y);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    Camera camera;
    camera.loc[1] = 40;
    camera.heading = 180;
    std::cout << "Camera = {\n"
              << "  loc = " << camera.loc << "\n"
              << "  heading = " << camera.heading << "\n"
              << "  pitch = " << camera.pitch << "\n"
              << "}" << std::endl;

    while(window.isOpen()) {
        sf::Event e;
        while(window.pollEvent(e)) {
            switch(e.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::Resized:
                windowSize = window.getSize();
                windowWidth  = windowSize.x,
                windowHeight = windowSize.y;
                windowCenter.x = windowWidth/2;
                windowCenter.y = windowHeight/2;
                initViewport(windowWidth,windowHeight);
                break;
            case sf::Event::KeyPressed:
                if(e.key.code == sf::Keyboard::Escape) {
                    mouseCaptured = !mouseCaptured;
                    window.setMouseCursorVisible(!mouseCaptured);
                    if(mouseCaptured) {
                        sf::Mouse::setPosition(windowCenter,window);
                    }
                }
                break;
            case sf::Event::LostFocus:
                mouseCaptured = false;
                window.setMouseCursorVisible(!mouseCaptured);
                break;
            default:
                break;
            }
        }

        Vec3f cameraDiff{{0,0,0}};
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            cameraDiff[2] += -1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            cameraDiff[2] += 1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            cameraDiff[0] += -1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            cameraDiff[0] += 1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            cameraDiff[1] += -1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            cameraDiff[1] += 1;
        }

        camera.translateRelative(cameraDiff*CAM_SPEED*DT);

        if(mouseCaptured) {
            sf::Vector2i mouseLoc2i = sf::Mouse::getPosition(window);
            Vec2i mouseCenter = { windowWidth/2,
                                  windowHeight/2 };
            Vec2i mouseLoc = { mouseLoc2i.x,
                               windowHeight-1-mouseLoc2i.y };
            mouseLoc -= mouseCenter;
            camera.heading += mouseLoc[0]/40.0;
            camera.pitch   += mouseLoc[1]/40.0;

            sf::Mouse::setPosition(windowCenter,window);
        }

        glLoadMatrixf(camera.matrix().values);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        VertexArray varr;

        chunk.buildVertArray(varr);
        drawVertArray(GL_QUADS,varr);

        window.display();
    }
    return 0;
}
