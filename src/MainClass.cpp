#include <MainClass.h>
#include <SDL2/SDL.h>
#ifndef __APPLE__
#include <SDL2/SDL_image.h>
#else
#include <SDL2_image/SDL_image.h>
#endif
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <InputManager.h>
#include <ResourceManager.h>
#include <Physics.h>
#include <Camera.h>
#include <LogManager.h>
#include <JoystickManager.h>
#include <operators.h>
#include <TextureHandle.h>
#include <Colors.h>
#include <Sprite.h>

using namespace std;

namespace SDL
{
    MainClass* MainClass::instance = nullptr;

    MainClass::MainClass(const char* title,int width,int height) : Actor(0,"MainClass"),
                                                                   m_windowTitle(title),
                                                                   m_windowWidth(width),
                                                                   m_windowHeight(height),
																   m_scaleW(1.0f),
																   m_scaleH(1.0f)

    {
        if(instance == nullptr)
        {
            instance = this;
        }

        m_FPSes[0] = m_FPSes[1] = m_FPSes[2] = 0.0;
        m_size = Vector2(m_windowWidth,m_windowHeight);
        m_isAffectedByCamera = false;
        m_affectedByScaling = false;
        m_mainClass = this;
        
        m_backBufferDst.x = m_backBufferDst.y = 0;
        m_backBufferDst.w = width;
        m_backBufferDst.h = height;
    }

    MainClass::~MainClass()
    {

    }

    void MainClass::run()
    {
        try
        {
            m_init();
            if(init())
                mainLoop();

            m_all_quit();
        }
        catch(exception& e)
        {
            cout << "An error accoured!" << endl << e.what() << endl;
        }

    }

    void MainClass::quit()
    {
        delete m_testRect;

        delete m_testTexture;
        m_testTexture = nullptr;

    }

    void MainClass::m_quit()
    {
        Actor::m_quit();
        
        if(m_joystickManager)
        {
            delete m_joystickManager;
            m_joystickManager = nullptr;
        }


        removeChild(m_inputManager);
        m_resourceManager->clear();
        
        delete m_resourceManager;

        SDL_DestroyRenderer(m_renderer);
        delete m_window;
        SDL_Quit();
    }

    void MainClass::m_init()
    {
        LogManager::log("Initializing Engine");
        
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            LogManager::log(std::string("Error Initializing SDL: ") + std::string(SDL_GetError()));
            getchar();
            exit(-1);
        }
        if(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != (IMG_INIT_PNG | IMG_INIT_JPG))
        {
            LogManager::log(std::string("Error Initializing IMG: ") + std::string(SDL_GetError()));
            getchar();
            exit(-1);
        }
        if(TTF_Init() < 0)
        {
            LogManager::log(std::string("Error Initializing TTF: ") + std::string(SDL_GetError()));
            getchar();
            exit(-1);
        }


        m_window = new WindowHandle(this,m_windowTitle,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,m_windowWidth,m_windowHeight,SDL_WINDOW_SHOWN);
        if(!m_window)
        {
            LogManager::log(std::string("Error Creating Window: ") + std::string(SDL_GetError()));
            getchar();
            exit(-1);
        }
        m_renderer = SDL_CreateRenderer(m_window->getWindow(),-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if(!m_renderer)
        {
            LogManager::log(std::string("Error Creating Renderer: ") + std::string(SDL_GetError()));
            getchar();
            exit(-1);
        }
        
        m_nativeResolution.set(m_windowWidth, m_windowHeight);

        m_scaleW = m_windowWidth/m_nativeResolution.getX();
        m_scaleH = m_windowHeight/m_nativeResolution.getY();

        m_backBuffer = SDL_CreateTexture(m_renderer,
                                   m_window->getPixelFormat(),
                                   SDL_TEXTUREACCESS_TARGET,
                                   m_nativeResolution.getX(),
                                   m_nativeResolution.getY());

        SDL_SetRenderTarget(m_renderer,m_backBuffer);

        m_camera = new Camera();
        addChild(m_camera);

        SDL_SetRenderDrawColor(m_renderer,0,0,(Uint8)255,(Uint8)255);
        m_inputManager = new InputManager;
        m_inputManager->setMainClass(this);
        m_inputManager->setParent(this);
        m_inputManager->init();
        m_resourceManager = new ResourceManager(m_renderer);
        
        // FPS Manager
        SDL_initFramerate(&m_fpsManager);
        SDL_setFramerate(&m_fpsManager,m_maxFPS);
        
        m_dstRect.x = 0;
        m_dstRect.y = 0;
        m_dstRect.w = m_nativeResolution.getX();
        m_dstRect.h = m_nativeResolution.getY();
        
        m_ambientLight = SDL::Colors::WHITE;
        
        LogManager::log("Finished Initializing Engine");
    }


    bool MainClass::init()
    {
        m_testRect = new SDL_Rect();
        m_testRect->x = m_windowWidth/2;
        m_testRect->y = m_windowHeight/2;

        m_testTexture = m_resourceManager->loadTexture("actionHero.png");
        if(m_testTexture)
        {
            m_testRect->w = m_testTexture->getWidth();
            m_testRect->h = m_testTexture->getHeight();
        }
        
        return true;
    }

    void MainClass::mainLoop()
    {
        
        while(true)
        {
                #ifdef MAX_FPS_LOCK
                if(m_timeForMaxFPS == 0.0)
                {
                #endif
                startTimeMeasure();
                #ifdef MAX_FPS_LOCK
                }
                m_timeForMaxFPS += m_deltaClockTimeForMaxFPS.count() / 1000000.0;
                
                m_startTimeForMaxFPS = high_resolution_clock::now();
                if(m_timeForMaxFPS >= 1.0/1000.0)
                {
                #endif
                    if(!pollEvents())
                    {
                        break;
                    }

                    if(!m_all_update())
                    {
                        break;
                    }
                    m_inputManager->m_all_update();
                    if(m_joystickManager)
                        m_joystickManager->update();

                    if(!m_all_render())
                    {
                        break;
                    }
                    SDL_SetRenderTarget(m_renderer,nullptr);
                    
                    SDL_RenderCopy(m_renderer,m_backBuffer,nullptr,m_scaleToResolution ? &m_backBufferDst : &m_dstRect);

                    SDL_RenderPresent(m_renderer);
                    SDL_SetRenderDrawColor(m_renderer,m_backgroundColor.r,m_backgroundColor.g,m_backgroundColor.b,m_backgroundColor.a);
                    SDL_RenderClear(m_renderer);

                    SDL_SetRenderTarget(m_renderer,m_backBuffer);
                    SDL_RenderClear(m_renderer);
                #ifdef MAX_FPS_LOCK
                    m_timeForMaxFPS = 0.0;
                }
                
                m_endTimeForMaxFPS = high_resolution_clock::now();
                
                m_deltaClockTimeForMaxFPS = m_endTimeForMaxFPS-m_startTimeForMaxFPS;
                
                if(m_timeForMaxFPS == 0.0)
                {
                    #endif
                    m_fps = endTimeMeasure();
                    #ifdef MAX_FPS_LOCK
                }
                #endif
        }
    }

    bool MainClass::pollEvents()
    {
        SDL_Event e;
        m_inputManager->setMouseWheel(0,0);
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT:
                    return false;
                case SDL_KEYDOWN:
                    m_inputManager->pressKey(e.key.keysym.sym);
                    break;
                case SDL_KEYUP:
                    m_inputManager->releaseKey(e.key.keysym.sym);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    m_inputManager->pressKey(e.button.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    m_inputManager->releaseKey(e.button.button);
                    break;
                case SDL_MOUSEMOTION:
                    m_inputManager->setMouseCoords(e.motion.x,e.motion.y);
                    break;
                case SDL_MOUSEWHEEL:
                    m_inputManager->setMouseWheel(e.wheel.x,e.wheel.y);
                    break;
            }

            if(m_joystickManager)
                m_joystickManager->pollEvents(e);

            if(!pollEvent(e))
            {
                return false;
            }
        }

        return true;
    }

    void MainClass::activateJoystick()
    {
        m_joystickManager = new JoystickManager();
    }


    bool MainClass::pollEvent(const SDL_Event& e)
    {
        return true;
    }

    void MainClass::startTimeMeasure()
    {
        m_startTime = high_resolution_clock::now();
    }

    int MainClass::endTimeMeasure()
    {
        if(m_maxFPS != NO_FPS_LOCK)
            SDL_framerateDelay(&m_fpsManager);
        
        m_endTime = high_resolution_clock::now();
        m_deltaClockTime = m_endTime - m_startTime;
        m_realDeltaTimeInSeconds = m_deltaClockTime.count()/1000000.0;
        
        m_realDeltaTimeSize = getDeltaTimesSize();
        if(m_realDeltaTimeSize == 11)
            moveDeltaTimesOneIndexDown();

        // Set the last frameTime
        m_realDeltaTimes[m_realDeltaTimeSize == 11 ? 10 : m_realDeltaTimeSize] = m_realDeltaTimeInSeconds;

        // Copy Real Frame Times into the smooth ones
        copyRealTimesToSmoothTimes();

        // Calculate the mean Time
        m_smoothDeltaTimeInSeconds = calculateMeanDeltaTime();

        m_FPSes[m_lastFrame] = 1.0/m_smoothDeltaTimeInSeconds;

        m_lastFrame++;
        if(m_lastFrame == 3)
        {
            m_lastFrame = 0;
        }


        return (m_FPSes[0]+m_FPSes[1]+m_FPSes[2])/3.0;
    }

    void MainClass::copyRealTimesToSmoothTimes()
    {
        for(int i = 0;i<11;i++)
        {
            m_smoothDeltaTimes[i] = m_realDeltaTimes[i];
        }
    }


    int MainClass::getDeltaTimesSize()
    {
        for(int i = 0;i<11;i++)
        {
            if(m_realDeltaTimes[i] == -1.0)
                return i;
        }

        return 11;
    }


    void MainClass::moveDeltaTimesOneIndexDown()
    {
        for(int i = 0;i<11;i++)
        {
            if(i > 0)
            m_realDeltaTimes[i-1] = m_realDeltaTimes[i];
        }

        m_realDeltaTimes[10] = -1.0;
    }


    double MainClass::calculateMeanDeltaTime()
    {
        double mean = 0.0;
        int size = 11;

        for(int i = 0;i<11;i++)
        {
            if(m_smoothDeltaTimes[i] == -1.0)
            {
                size = i;
                break;
            }
        }

        if(size > 4)
        {
            takeAwayHighestAndLowestDts();
        }

        for(int i = 0;i<11;i++)
        {
            if(m_smoothDeltaTimes[i] != -1.0)
            {
                mean += m_smoothDeltaTimes[i];
            }
        }

        return mean/(size > 4 ? 7.0 : (double)size);
    }

    void MainClass::takeAwayHighestAndLowestDts()
    {
        int numIndex = 0;
        double num=0;

        // Take away two Lowest
        for(int j = 0;j<2;j++)
        {
            setNum(num,numIndex);
            for(int i = 0;i<11;i++)
            {
                if(m_smoothDeltaTimes[i] != -1.0)
                {
                    if(m_smoothDeltaTimes[i] < num)
                    {
                        num = m_smoothDeltaTimes[i];
                        numIndex = i;
                    }
                }
            }

            m_smoothDeltaTimes[numIndex] = -1.0;

        }


        numIndex = 0;


        //Take away two highest
        for(int j = 0;j<2;j++)
        {
            setNum(num,numIndex);
            for(int i = 0;i<11;i++)
            {
                if(m_smoothDeltaTimes[i] != -1.0)
                {
                    if(m_smoothDeltaTimes[i] > num)
                    {
                        num = m_smoothDeltaTimes[i];
                        numIndex = i;
                    }
                }
            }

            m_smoothDeltaTimes[numIndex] = -1.0;
        }

    }

    void MainClass::setNum(double& num,int& numIndex)
    {
        for(int i = 0;i<11;i++)
        {
            if(m_smoothDeltaTimes[i] != -1.0)
            {
                num = m_smoothDeltaTimes[i];
                numIndex = i;
            }
        }
    }

    bool MainClass::m_render()
    {
        Actor::m_render();
        
        if(m_physics && m_physics->getDebug())
        {
            m_physics->getWorld()->DrawDebugData();
        }
        
        return true;
    }

    bool MainClass::render()
    {
        if(m_testTexture)
            m_testTexture->renderCopy(m_renderer,m_testRect);

        return true;
    }

    bool MainClass::m_update()
    {
        if(m_physics)
        {
            m_physics->update();
        }
        
        if(m_ambientSprite)
        {
            // Delete everything on ambient Sprite
            m_ambientSprite->getTexture()->setRenderTarget(m_renderer);
            SDL_SetRenderDrawBlendMode(m_renderer,SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(m_renderer,m_ambientLight.r,m_ambientLight.g,m_ambientLight.b,m_ambientLight.a);
            SDL_RenderClear(m_renderer);
            SDL_SetRenderTarget(m_renderer,m_backBuffer);
            SDL_SetRenderDrawColor(m_renderer,0,0,0,255);
            SDL_SetRenderDrawBlendMode(m_renderer,SDL_BLENDMODE_BLEND);
        }

        return Actor::m_update();
    }


    bool MainClass::update()
    {
        if(m_inputManager->isPressed(SDLK_a))
        {
            m_testRect->x -= 1;
        }
        else if(m_inputManager->isPressed(SDLK_d))
        {
            m_testRect->x += 1;
        }
        else if(m_inputManager->isPressed(SDLK_w))
        {
            m_testRect->y -= 1;
        }
        else if(m_inputManager->isPressed(SDLK_s))
        {
            m_testRect->y += 1;
        }


        if(m_inputManager->justPressed(SDLK_f))
        {
            m_window->setFullscreen(!m_window->isFullscreen());
        }
        else if(m_inputManager->justPressed(SDLK_t))
        {
            SDL_Point res = m_window->getResolution();

            m_window->setResolution(res.x == 1280 ? 1920 : 1280,res.x == 720 ? 1080 : 720);

            if(m_window->isFullscreen())
            {
                m_window->setFullscreen(false);
                m_window->setFullscreen(true);
            }
        }

        return true;
    }

    Camera* MainClass::getCamera()
    {
        return m_camera;
    }

    Physics* MainClass::getPhysics()
    {
        return m_physics;
    }

    void MainClass::activatePhysics(const Vector2& gravity,const Vector2& worldSize)
    {
        m_physics = new Physics(this,gravity,worldSize);
    }
    
    void MainClass::onResize(int w,int h)
    {
        double normScale = m_nativeResolution.getX()/m_nativeResolution.getY();
        double scaleDif = ((double)w/(double)h)/normScale;
        
        
        if(scaleDif < 0.5 || scaleDif > 2.0)
        {
            m_backBufferDst.h = h;
            m_backBufferDst.w = m_backBufferDst.h*normScale;
            m_backBufferDst.x = w/2-m_backBufferDst.w/2;
        }
        else
        {
            m_backBufferDst.x = 0;
            m_backBufferDst.y = 0;
            m_backBufferDst.w = w;
            m_backBufferDst.h = h;
        }
        
        #ifdef _WIN32
        recreateBuffer();
        SDL_Rect viewport = {0,0,w,h};
        resetViewport(&viewport);
        #endif
    }
    
    void MainClass::onFullscreen(bool full)
    {
        SDL_Point newRes = m_window->getResolution();
        
        double normScale = m_nativeResolution.getX()/m_nativeResolution.getY();
        double scaleDif = ((double)newRes.x/(double)newRes.y)/normScale;
        
        
        if((scaleDif < 0.5 || scaleDif > 2.0) && full)
        {
            m_backBufferDst.h = newRes.y;
            m_backBufferDst.w = m_backBufferDst.h*normScale;
            m_backBufferDst.x = newRes.x/2-m_backBufferDst.w/2;
        }
        else
        {
            m_backBufferDst.x = 0;
            m_backBufferDst.y = 0;
            m_backBufferDst.w = newRes.x;
            m_backBufferDst.h = newRes.y;
        }
        
        #ifdef _WIN32
        recreateBuffer();
        SDL_Rect viewport = {0,0,newRes.x,newRes.y};
        resetViewport(&viewport);
        #endif
    }
    
    
    void MainClass::setMaxFPS(int fps)
    {
        if(fps == NO_FPS_LOCK)
        {
            m_maxFPS = fps;
            return;
        }
        
        if(fps > FPS_UPPER_LIMIT)
        {
            fps = FPS_UPPER_LIMIT;
        }
        else if(fps < FPS_LOWER_LIMIT)
        {
            fps = FPS_LOWER_LIMIT;
        }
        
        m_maxFPS = fps;
        
        if(SDL_setFramerate(&m_fpsManager,m_maxFPS)<0)
        {
            LogManager::log("Error while setting max FPS");
        }
    }
    
    void MainClass::setAmbientLight(const SDL_Color& col)
    {
        m_ambientLight = col;
        
        if(!m_ambientSprite)
        {
            TextureHandle* tex = Textures::BOX(this,Vector2(m_nativeResolution.getX(),m_nativeResolution.getY()),Colors::WHITE);
            tex->setColorMod(m_ambientLight);
            tex->setAlphaMod(m_ambientLight.a);
            tex->setBlendMode(SDL_BLENDMODE_MOD);
            
            m_ambientSprite = new Sprite(tex,AMB_ORDER);
            m_ambientSprite->setAffectedByCamera(false);
            m_ambientSprite->setName("AmbientSprite");
            
            addChild(m_ambientSprite);
        }
    }
    
    Sprite* MainClass::getAmbientSprite()
    {
        return m_ambientSprite;
    }
    
    void MainClass::setNativeRes(const Vector2& v)
    {
        m_nativeResolution = v;
        recreateBuffer();
        SDL_Rect rect = {0,0,(int)v.getX(),(int)v.getY()};
        resetViewport(&rect);
        
        if(m_ambientSprite)
        {
            m_ambientSprite->setSize(m_nativeResolution);
        }
    }
    
    void MainClass::recreateBuffer()
    {
        if(m_backBuffer)
            SDL_DestroyTexture(m_backBuffer);
        m_backBuffer = SDL_CreateTexture(m_renderer, m_window->getPixelFormat(), SDL_TEXTUREACCESS_TARGET, (int)m_nativeResolution.getX(), (int)m_nativeResolution.getY());
        SDL_SetRenderTarget(m_renderer, m_backBuffer);
        SDL_RenderClear(m_renderer);
        SDL_SetTextureBlendMode(m_backBuffer, SDL_BLENDMODE_BLEND);
        m_backBufferDst.w = m_nativeResolution.getX();
        m_backBufferDst.h = m_nativeResolution.getY();
        m_dstRect = m_backBufferDst;
        m_dstRect.x = m_dstRect.y = 0;
    }
    
    void MainClass::resetViewport(SDL_Rect* rect)
    {
        SDL_SetRenderTarget(m_renderer, m_backBuffer);
        SDL_RenderSetViewport(m_renderer,rect);
        SDL_SetRenderTarget(m_renderer,nullptr);
        SDL_RenderSetViewport(m_renderer,rect);
        SDL_SetRenderTarget(m_renderer,m_backBuffer);
    }

    void MainClass::setBackgroundColor(const SDL_Color& col)
    {
    	m_backgroundColor = col;
    }
}
