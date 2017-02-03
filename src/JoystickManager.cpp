#include "JoystickManager.h"
#include "JoystickListener.h"
#include <SDL2/SDL_joystick.h>
#include <iostream>
#include "LogManager.h"
#include "operators.h"

namespace SDL
{
    JoystickManager::JoystickManager()
    {
        //ctor
    }

    JoystickManager::~JoystickManager()
    {
        if(m_listener)
        delete m_listener;
    }
    void JoystickManager::setListener(JoystickListener* jl)
    {
        m_listener = jl;
        m_listener->setManager(this);
    }


    void JoystickManager::pollEvents(const SDL_Event& e)
    {
        switch(e.type)
        {
        case SDL_JOYAXISMOTION:
            m_listener->onAxisMotion(e.jaxis);
            break;
        case SDL_JOYBUTTONDOWN:
            m_listener->onButtonDown(e.jbutton);
            break;
        case SDL_JOYBUTTONUP:
            m_listener->onButtonUp(e.jbutton);
            break;
        case SDL_JOYDEVICEADDED:
            m_listener->onConnect(e.jdevice);
            break;
        case SDL_JOYDEVICEREMOVED:
            m_listener->onDisconnect(e.jdevice);
            break;
        }
    }

    bool equal(Uint8 u1[], Uint8 u2[])
    {
        int size = sizeof((Uint8*)u1)/sizeof(*u1);
        for(int i = 0;i<size;i++)
        {
            if(u1[i] != u2[i])
            {
                return false;
            }
        }
        return true;
    }

    void JoystickManager::addJoystick(SDL_Joystick* j)
    {
        for(size_t i = 0;i<m_joysticks.size();i++)
        {
            if(m_joysticks[i] == nullptr)
                continue;
            if(equal(SDL_JoystickGetGUID(j).data,SDL_JoystickGetGUID(m_joysticks[i]).data))
                return;
        }

        for(size_t i = 0;i<m_joysticks.size();i++)
        {
            if(m_joysticks[i] == nullptr)
            {
                LogManager::log("Added Joystick as nullptr");
                m_joysticks[i] = j;
                return;
            }
        }

        m_joysticks.push_back(j);
    }

    void JoystickManager::removeJoystick(SDL_Joystick* j)
    {
        for(size_t i = 0;i<m_joysticks.size();i++)
        {
            if(equal(SDL_JoystickGetGUID(j).data,SDL_JoystickGetGUID(m_joysticks[i]).data))
            {
                SDL_JoystickClose(m_joysticks[i]);
                m_joysticks[i] = nullptr;
                break;
            }

        }
    }
    void JoystickManager::removeJoystick(int which)
    {
        for(size_t i = 0;i<m_joysticks.size();i++)
        {
            if(SDL_JoystickInstanceID(m_joysticks[i]) == which)
            {
                SDL_JoystickClose(m_joysticks[i]);
                m_joysticks[i] = nullptr;
                break;
            }
        }
    }

    void JoystickManager::update()
    {
        if(m_listener == nullptr)
        {
            LogManager::log("Listener of JoystickManager is nullptr! ");
            return;
        }

        for(size_t i = 0;i<m_joysticks.size();i++)
        {
            if(m_joysticks[i] == nullptr)
                continue;

            if(SDL_JoystickGetAttached(m_joysticks[i]) == SDL_FALSE)
            {
                SDL_JoystickClose(m_joysticks[i]);
                m_joysticks[i] = nullptr;
            }
        }

    }

}
