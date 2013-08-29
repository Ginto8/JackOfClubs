#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include "Camera.hpp"
#include "World.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

const float FPS = 60;
const float DT  = 1.0/FPS;
const float WORLD_RADIUS = 200;
const float CAM_SPEED = 30;

void initViewport(int width,int height) {
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(height != 0) ? width/(float)height : 1,0.1,500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void initGL() {
    glClearColor(0,0.4,0.99,0);
    glClearDepth(1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);

    Color4f ambient = {0.5,0.5,0.5,1},
            diffuse = {1,1,1,1};
    glLightfv(GL_LIGHT1,GL_AMBIENT,&ambient[0]);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,&diffuse[0]);
    Vec4f lightLoc = {Chunk::WIDTH/2,Chunk::HEIGHT+5,Chunk::WIDTH/2,1};
    glLightfv(GL_LIGHT1,GL_POSITION,&lightLoc[0]);

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
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
    World world;

    initGL();
    initViewport(windowSize.x,windowSize.y);

    Camera camera;
    camera.loc = Vec3f{{8,40,8}};
    camera.heading = 180;
    camera.pitch = -30;
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
            camera.constrain();

            sf::Mouse::setPosition(windowCenter,window);
        }

        world.setViewerLoc(camera.loc);

        world.update(DT);

        camera.setLighting();

        glLoadMatrixf(camera.matrix().values);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        world.draw(camera.viewDirection());

        window.display();
    }
    return 0;
}
