#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <map>
#include "Actor.h"

using namespace std;
namespace SDL
{
    struct MouseCoords
    {
        unsigned int x;
        unsigned int y;
    };

    class InputManager : public Actor
    {
        public:
            InputManager(int order = IPMGR_ORDER);
            virtual ~InputManager();

             bool isPressed(unsigned int kc);
             bool justPressed(unsigned int kc);

             void releaseKey(unsigned int kc);
             void pressKey(unsigned int kc);
             void setMouseCoords(unsigned int x,unsigned int y);

             bool update() override;

             const MouseCoords getMouseCoords()
            {
                return m_mouseCords;
            }

        protected:

        private:
             map<unsigned int,bool> m_keyMap;
             map<unsigned int,bool> m_previousKeyMap;

             bool wasPressed(unsigned int kc);
             MouseCoords m_mouseCords;

    };
}


#endif // INPUTMANAGER_H
