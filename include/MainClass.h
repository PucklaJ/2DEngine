#ifndef MAINCLASS_H
#define MAINCLASS_H
#include "InputManager.h"
#include <SDL2/SDL_events.h>
#include "Actor.h"
#include "WindowHandle.h"
#include <chrono>
#include <SDL2_framerate.h>

#define NO_FPS_LOCK -1

using namespace std::chrono;

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class SDL_Rect;


namespace SDL
{            
    class ResourceManager;
    class Physics;
    class Camera;
    class JoystickManager;
    class TextureHandle;
    class Sprite;

    class MainClass : public Actor
    {
        public:
            MainClass(const char* title = "New Window",int width = NORM_W,int height = NORM_H);
            virtual ~MainClass();

            void run();

            void setTimeScale(double d){m_timeScale = d;}
            void setMaxFPS(int);
            void setAmbientLight(const SDL_Color& col);

            static MainClass* getInstance() {return instance;}

            InputManager* getInputManager() {return m_inputManager;}
            ResourceManager* getResourceManager() {return m_resourceManager;}
            SDL_Renderer* getRenderer() {return m_renderer;}
            WindowHandle* getWindow() {return m_window;}
            SDL_Texture* getBackBuffer() {return m_backBuffer;}
            Camera* getCamera();
            int getMaxFPS() {return m_maxFPS;}
            Physics* getPhysics();
            float getScaleW() const {return m_scaleW;}
            float getScaleH() const {return m_scaleH;}
            #ifdef _WIN32
            double getDeltaTimeInSeconds() const {return m_realDeltaTimeInSeconds*m_timeScale;}
            #else
            double getDeltaTimeInSeconds() const {return m_smoothDeltaTimeInSeconds*m_timeScale;}
            #endif
            double getFPS() const {return m_fps;}
            const SDL_Color& getAmbientLight() const {return m_ambientLight;}
            Sprite* getAmbientSprite();

            void activatePhysics(const Vector2& gravity = Vector2(0.0,-10.0),const Vector2& worldSize = Vector2(NORM_W,NORM_H));
            
            
            
            virtual bool m_render() override;
            virtual bool render() override;
            virtual bool update() override;
            virtual bool m_update() override;

            virtual void onResize(int,int);
            virtual void onFullscreen(bool);

        protected:
            virtual void quit() = 0;
            virtual bool init() = 0;
            virtual bool pollEvents();
            virtual bool pollEvent(const SDL_Event& e);
            void activateJoystick();

            int m_maxFPS = 60;
            InputManager* m_inputManager = nullptr;
            ResourceManager* m_resourceManager = nullptr;
            WindowHandle* m_window = nullptr;
            Physics* m_physics = nullptr;
            Camera* m_camera = nullptr;
            JoystickManager* m_joystickManager = nullptr;

            double m_timeScale = 1.0;
            bool m_scaleToResolution = true;

        private:
            static MainClass* instance;

            void m_quit();

            void startTimeMeasure();
            void mainLoop();
            int endTimeMeasure();

            void m_init();

            const char* m_windowTitle;
            int m_windowWidth;
            int m_windowHeight;


            double calculateMeanDeltaTime();
            void takeAwayHighestAndLowestDts();
            void moveDeltaTimesOneIndexDown();
            int getDeltaTimesSize();
            void copyRealTimesToSmoothTimes();
            int m_realDeltaTimeSize = 0;
            void setNum(double&,int&);
            double m_realDeltaTimeInSeconds = 0.0;
            double m_smoothDeltaTimeInSeconds = 0.0;
            double m_realDeltaTimes[11] = {-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0};
            double m_smoothDeltaTimes[11] = {-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0};
            double m_FPSes[3];
            double m_fps = 0.0;
            int m_lastFrame = 0;

            TextureHandle* m_testTexture = nullptr;
            SDL_Rect* m_testRect = nullptr;
            SDL_Texture* m_backBuffer = nullptr;
            float m_scaleW;
            float m_scaleH;
            SDL_Rect m_backBufferDst;

            double m_time = 0.0;
            high_resolution_clock::time_point m_startTime;
            high_resolution_clock::time_point m_endTime;
            #ifdef MAX_FPS_LOCK
            double m_timeForMaxFPS = 0.0;
            high_resolution_clock::time_point m_startTimeForMaxFPS;
            high_resolution_clock::time_point m_endTimeForMaxFPS;
            duration<double,micro> m_deltaClockTimeForMaxFPS;
            #endif
            duration<double,micro> m_deltaClockTime;
            
            FPSmanager m_fpsManager;
            
            SDL_Color m_ambientLight;
            Sprite* m_ambientSprite = nullptr;

    };
}



#endif // MAINCLASS_H
