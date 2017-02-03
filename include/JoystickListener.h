#ifndef JOYSTICKLISTENER_H
#define JOYSTICKLISTENER_H
#include <SDL2/SDL_events.h>
#include <vector>
#include <map>

namespace SDL
{
    extern int Uint8ToInt(Uint8 u);
    namespace Buttons
    {
        const int UP = 0,
                  DOWN = 1,
                  LEFT = 2,
                  RIGHT = 3,
                  START = 4,
                  SELECT = 5,
                  L3 = 6,
                  R3 = 7,
                  L1 = 8,
                  R1 = 9,
                  CROSS = 10,
                  CIRCLE = 11,
                  SQUARE = 12,
                  TRIANGLE = 13,
                  BACK=SELECT,
                  LS=L3,
                  RS=R3,
                  LB=L1,
                  RB=R1,
                  A=CROSS,
                  B=CIRCLE,
                  X=SQUARE,
                  Y=TRIANGLE;
    };

    namespace Axis
    {
        const int LEFT_STICK_X=0,
                  LEFT_STICK_Y=1,
                  RIGHT_STICK_X=2,
                  RIGHT_STICK_Y=3,
                  L2=4,
                  R2=5,
                  LT=L2,
                  RT=R2;


    };

    class JoystickListener;

    class AxisButtonListener
    {
    public:
        virtual void onAxisMotion(const SDL_JoyAxisEvent&)=0;
        virtual void onButtonDown(const SDL_JoyButtonEvent&)=0;
        virtual void onButtonUp(const SDL_JoyButtonEvent&)=0;

        void setListener(JoystickListener*);
    protected:
        JoystickListener* m_listener;
    private:

    };

    class ConnectionListener
    {
    public:
        virtual void onConnect(const SDL_JoyDeviceEvent&)=0;
        virtual void onDisconnect(const SDL_JoyDeviceEvent&)=0;

        void setListener(JoystickListener*);
    protected:
        JoystickListener* m_listener;
    private:

    };

    class JoystickManager;

    class JoystickListener
    {
        public:
            JoystickListener();
            virtual ~JoystickListener();

            void setManager(JoystickManager*);
            JoystickManager* getManager(){return m_manager;}

            void onAxisMotion(const SDL_JoyAxisEvent&);
            void onButtonDown(const SDL_JoyButtonEvent&);
            void onButtonUp(const SDL_JoyButtonEvent&);
            void onConnect(const SDL_JoyDeviceEvent&);
            void onDisconnect(const SDL_JoyDeviceEvent&);

            void addListener(AxisButtonListener*,int);
            void addListener(ConnectionListener*,int);

        protected:
            JoystickManager* m_manager = nullptr;
        private:
            std::map<int,std::vector<AxisButtonListener*>> m_abLis;
            std::vector<ConnectionListener*> m_conLis;


    };

    typedef std::map<int,std::vector<AxisButtonListener*>>::iterator AbIterator ;
}


#endif // JOYSTICKLISTENER_H
