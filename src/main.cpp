#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include "Camera.hpp"
#include "World.hpp"
#include "NumUtil.hpp"

#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

const float FPS = 50;
const float DT  = 1.0/FPS;
/*const float WORLD_RADIUS = 200;*/
const float CAM_SPEED = 30;
const float LOOK_SCALE = 1.0/10;
const float CLICKS_PER_SECOND = 4;
const float MAX_RESOLUTION_SPEED = 10;
const std::string TITLE = "JackOfClubs Test";

void initViewport(int width,int height,Camera& camera) {
    glViewport(0,0,width,height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.aspectRatio = (height != 0) ? width/(float)height : 1;
}

void initGL() {
    glClearColor(0,0.4,0.99,0);
    glClearDepth(1);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

float frand() {
    return rand()/(float)RAND_MAX;
}

class DelayedRepeat {
    const float _timeForEach;
    float _time = 0;
    bool _prevVal = false;
public:
    DelayedRepeat(float timeForEach) :
      _timeForEach(timeForEach) {}
    bool next(bool val,float dt) {
        bool ret = false;
        if(val && !_prevVal) {
            _time = 0;
            ret = true;
        } else if(val) {
            _time += dt;
            while(_time > _timeForEach) {
                _time -= _timeForEach;
                ret = true;
            }
        }
        _prevVal = val;
        return ret;
    }
};

int main() {
    std::cout << "DT: " << DT << std::endl;
    sf::Window window(sf::VideoMode(640,480),
                      "JackOfClubs test",
                      sf::Style::Default,
                      sf::ContextSettings(32));
    {
        auto settings = window.getSettings();
        std::cout << "Depth bits:    " << settings.depthBits         << "\n"
                  << "Stencil bits:  " << settings.stencilBits       << "\n"
                  << "AA Level:      " << settings.antialiasingLevel << "\n"
                  << "Major version: " << settings.majorVersion      << "\n"
                  << "Minor version: " << settings.minorVersion      << "\n";
    }
    bool mouseCaptured = false;
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2i windowCenter(windowSize.x/2,
                              windowSize.y/2);
    int windowWidth  = windowSize.x,
        windowHeight = windowSize.y;
    World world;
    Camera camera;

    Maybe<World::BlockSelection> blockSelection;

    DelayedRepeat leftMouse(1.0/CLICKS_PER_SECOND),
                  rightMouse(1.0/CLICKS_PER_SECOND);

    initGL();
    initViewport(windowWidth,windowHeight,camera);

    VertexArray cube;
    {
        cube.push_back({ {{-0.004,-0.004,-0.004}},{{0,0,0}},{{0,0,-1}} });
        cube.push_back({ {{-0.004,1.004,-0.004}}, {{0,0,0}},{{0,0,-1}} });
        cube.push_back({ {{1.004,1.004,-0.004}},  {{0,0,0}},{{0,0,-1}} });
        cube.push_back({ {{1.004,-0.004,-0.004}}, {{0,0,0}},{{0,0,-1}} });

        cube.push_back({ {{-0.004,-0.004,1.004}}, {{0,0,0}},{{0,0,1}}  });
        cube.push_back({ {{1.004,-0.004,1.004}},  {{0,0,0}},{{0,0,1}}  });
        cube.push_back({ {{1.004,1.004,1.004}},   {{0,0,0}},{{0,0,1}}  });
        cube.push_back({ {{-0.004,1.004,1.004}},  {{0,0,0}},{{0,0,1}}  });
 
        cube.push_back({ {{-0.004,-0.004,-0.004}},{{0,0,0}},{{-1,0,0}} });
        cube.push_back({ {{-0.004,-0.004,1.004}}, {{0,0,0}},{{-1,0,0}} });
        cube.push_back({ {{-0.004,1.004,1.004}},  {{0,0,0}},{{-1,0,0}} });
        cube.push_back({ {{-0.004,1.004,-0.004}}, {{0,0,0}},{{-1,0,0}} });

        cube.push_back({ {{1.004,-0.004,-0.004}}, {{0,0,0}},{{1,0,0}}  });
        cube.push_back({ {{1.004,1.004,-0.004}},  {{0,0,0}},{{1,0,0}}  });
        cube.push_back({ {{1.004,1.004,1.004}},   {{0,0,0}},{{1,0,0}}  });
        cube.push_back({ {{1.004,-0.004,1.004}},  {{0,0,0}},{{1,0,0}}  });

        cube.push_back({ {{-0.004,-0.004,-0.004}},{{0,0,0}},{{0,-1,0}} });
        cube.push_back({ {{1.004,-0.004,-0.004}}, {{0,0,0}},{{0,-1,0}} });
        cube.push_back({ {{1.004,-0.004,1.004}},  {{0,0,0}},{{0,-1,0}} });
        cube.push_back({ {{-0.004,-0.004,1.004}}, {{0,0,0}},{{0,-1,0}} });

        cube.push_back({ {{-0.004,1.004,-0.004}}, {{0,0,0}},{{0,1,0}}  });
        cube.push_back({ {{-0.004,1.004,1.004}},  {{0,0,0}},{{0,1,0}}  });
        cube.push_back({ {{1.004,1.004,1.004}},   {{0,0,0}},{{0,1,0}}  });
        cube.push_back({ {{1.004,1.004,-0.004}},  {{0,0,0}},{{0,1,0}}  });
    }

    camera.loc = Vec3f{{8,40,8}};
    camera.heading = 180;
    camera.pitch = -30;
    
    sf::Clock frameTime;

    while(window.isOpen()) {
        frameTime.restart();

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
                initViewport(windowWidth,windowHeight,camera);
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
            case sf::Event::MouseButtonPressed:
            default:
                break;
            }
        }

        Vec3f cameraDiff{{0,0,0}};
        
        if(mouseCaptured) {
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
        }
        
        AABB camBB{camera.loc+Vec3f{{0,-0.5,0}},
                   {{0.6,2,0.6}}};
        auto collision = world.checkCollision(camBB);
        if(collision) {
            collision.forward([=,&camera](Collision c) {
                auto offset = c.overlap;
                auto maxMag = MAX_RESOLUTION_SPEED*DT;
                if(offset.magSquared() > maxMag*maxMag) {
                    offset = maxMag*norm(offset);
                }
                camera.loc += offset;
            });
        } else {
            auto translation = camera.relativeTranslation(cameraDiff*CAM_SPEED*DT);
            do {
                camBB.center = camera.loc+translation+Vec3f{{0,-0.5,0}};
                collision = world.checkCollision(camBB);
                Collision c;
                if(collision.extract(c)) {
                    float maxInd = 0;
                    for(int i=1;i<3;++i) {
                        if(std::abs(translation[i]) 
                           > std::abs(translation[maxInd])) {
                            maxInd = i;
                        }
                    }
                    auto overlap = c.overlap;
                    bool found = false;
                    for(int i=0;i<3;++i) {
                        if(translation[i] != 0 && overlap[i] != 0 &&
                           signum(translation[i]) != signum(overlap[i])) {
                            if(i == maxInd) {
                                translation[maxInd] += overlap[i];
                            } else {
                                translation[i] = 0;
                            }
                            found = true;
                            break;
                        }
                    }
                    if(!found) {
                        translation = {{0,0,0}};
                        break;
                    }
                }
            } while(collision);
            camera.loc += translation;
        }

        if(mouseCaptured) {
            sf::Vector2i mouseLoc2i = sf::Mouse::getPosition(window);
            Vec2i mouseCenter = {{ windowWidth /2,
                                   windowHeight/2 }};
            Vec2i mouseLoc = {{ mouseLoc2i.x,
                                windowHeight-1-mouseLoc2i.y }};
            mouseLoc -= mouseCenter;
            camera.heading += mouseLoc[0]*LOOK_SCALE;
            camera.pitch   += mouseLoc[1]*LOOK_SCALE;
            camera.constrain();

            sf::Mouse::setPosition(windowCenter,window);
            blockSelection.forward([=,&world,&leftMouse,&rightMouse]
                                    (World::BlockSelection selected) {
                bool left  = leftMouse.next(sf::Mouse::isButtonPressed(sf::Mouse::Left),DT),
                     right = rightMouse.next(sf::Mouse::isButtonPressed(sf::Mouse::Right),DT);
                if(left) {
                    auto block = selected.block;
                    world.deleteBlock(block[0],
                                      block[1],
                                      block[2]);
                } else if(right) {
                    auto placeLoc = selected.block+selected.face;
                    if(placeLoc != Vec3i{{(int)std::floor(camera.loc[0]),
                                          (int)std::floor(camera.loc[1]),
                                          (int)std::floor(camera.loc[2])}} &&
                       !world.getBlock(placeLoc[0],
                                       placeLoc[1],
                                       placeLoc[2]).filled) {
                        world.setBlock(placeLoc[0],
                                       placeLoc[1],
                                       placeLoc[2],{{1,1,1}});
                    }
                }
            });
        } else {
            leftMouse.next(false,DT);
            rightMouse.next(false,DT);
        }
        
        auto viewDir = camera.viewDirection();

        /*std::cout << camera.loc << "\t" << viewDir << std::endl;*/

        world.setViewerLoc(camera.loc);
        world.update(DT);
        blockSelection = world.selectedBlock(viewDir);
        
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        camera.begin();
        {
            world.draw(viewDir);

            blockSelection.forward([cube](World::BlockSelection selection) {
                auto block = selection.block;
                glTranslatef(block[0],
                             block[1],
                             block[2]);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                drawVertArray(GL_QUADS,cube);

                glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            });
        }
        camera.end();

        glMatrixMode(GL_PROJECTION);

        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);

        glPointSize(2);
        glBegin(GL_POINTS);
            glColor3f(0,0,0);
            glVertex3f(0,0,0);
        glEnd();

        glMatrixMode(GL_PROJECTION);

        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);

        auto timeForFrame = frameTime.getElapsedTime();
        float currFPS = 1.0/timeForFrame.asSeconds();
        if(timeForFrame.asSeconds() < DT) {
            sf::sleep(sf::seconds(DT)-timeForFrame);
            currFPS = FPS;
        }
        std::stringstream newTitle(TITLE);
        newTitle << ": " << (int)(currFPS+0.5)
                 << " FPS, " << timeForFrame.asMilliseconds() << " ms/frame";
        window.setTitle(newTitle.str());

        window.display();
    }
    return 0;
}
