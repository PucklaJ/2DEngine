#ifndef JOYSTCKMANAGER_H
#define JOYSTCKMANAGER_H
#include <vector>
#include <SDL2/SDL_events.h>

namespace SDL
{
    class JoystickListener;

    class JoystickManager
    {
        public:
            JoystickManager();
            virtual ~JoystickManager();

            void pollEvents(const SDL_Event& e);
            void update();

            void setListener(JoystickListener*);
            void addJoystick(SDL_Joystick*);
            void removeJoystick(SDL_Joystick*);
            void removeJoystick(int);

            std::vector<SDL_Joystick*>& getJoysticks(){return m_joysticks;}

        protected:

        private:
            JoystickListener* m_listener = nullptr;
            std::vector<SDL_Joystick*> m_joysticks;
    };
}


#endif // JOYSTCKMANAGER_H
