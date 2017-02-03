#include "JoystickListener.h"
#include "JoystickManager.h"
#include "LogManager.h"
#include "operators.h"

namespace SDL
{
    JoystickListener::JoystickListener()
    {
        //ctor
    }

    JoystickListener::~JoystickListener()
    {
        //dtor
    }

    void JoystickListener::setManager(JoystickManager* jm)
    {
        m_manager = jm;
    }

    void JoystickListener::onAxisMotion(const SDL_JoyAxisEvent& e)
    {
        int index = -1;
        for(size_t i = 0;i<m_manager->getJoysticks().size();i++)
        {
            if(m_manager->getJoysticks()[i] == nullptr)
               continue;

            if(SDL_JoystickInstanceID(m_manager->getJoysticks()[i]) == e.which)
            {
                index = i;
                break;
            }
        }

        if(index == -1)
        {
            return;
        }

        for(AbIterator it = m_abLis.begin();it!=m_abLis.end();it++)
        {
            if(it->first == index)
            {
                for(size_t i = 0;i<it->second.size();i++)
                {
                    it->second[i]->onAxisMotion(e);
                }
            }
        }
    }

    int Uint8ToInt(Uint8 u)
    {
        int n = 0;
        for(int i = 0;i<8;i++)
        {
            if(u&(1<<i))
            {
                n |= (1<<i);
            }
        }

        return n;
    }

    void JoystickListener::onButtonDown(const SDL_JoyButtonEvent& e)
    {
        int index = -1;

        for(size_t i = 0;i<m_manager->getJoysticks().size();i++)
        {
            if(m_manager->getJoysticks()[i] == nullptr)
               continue;

            if(SDL_JoystickInstanceID(m_manager->getJoysticks()[i]) == e.which)
            {
                index = i;
                break;
            }
        }

        if(index == -1)
        {
            LogManager::log("Index = -1");
            return;
        }


        for(AbIterator it = m_abLis.begin();it!=m_abLis.end();it++)
        {
            if(it->first == index)
            {
                for(size_t i = 0;i<it->second.size();i++)
                {
                    it->second[i]->onButtonDown(e);
                }
            }
        }
    }

    void JoystickListener::onButtonUp(const SDL_JoyButtonEvent& e)
    {
        int index = -1;

        for(size_t i = 0;i<m_manager->getJoysticks().size();i++)
        {
            if(m_manager->getJoysticks()[i] == nullptr)
                continue;

            if(SDL_JoystickInstanceID(m_manager->getJoysticks()[i]) == e.which)
            {
                index = i;
                break;
            }
        }

        if(index == -1)
        {
            return;
        }

        for(AbIterator it = m_abLis.begin();it!=m_abLis.end();it++)
        {
            if(it->first == index)
            {
                for(size_t i = 0;i<it->second.size();i++)
                {
                    it->second[i]->onButtonUp(e);
                }
            }
        }
    }

    void JoystickListener::onConnect(const SDL_JoyDeviceEvent& e)
    {
        LogManager::log("Joystick connected");
        m_manager->addJoystick(SDL_JoystickOpen(e.which));

        for(size_t i = 0;i<m_conLis.size();i++)
        {
            m_conLis[i]->onConnect(e);
        }
    }

    void JoystickListener::onDisconnect(const SDL_JoyDeviceEvent& e)
    {
        LogManager::log("Joystick disconnected");

        for(size_t i = 0;i<m_conLis.size();i++)
        {
            m_conLis[i]->onDisconnect(e);
        }
    }

    void JoystickListener::addListener(AxisButtonListener* a, int i)
    {
        for(AbIterator it = m_abLis.begin();it!=m_abLis.end();it++)
        {
            if(it->first == i)
            {
                it->second.push_back(a);
                return;
            }
        }

        std::vector<AxisButtonListener*> vec;
        vec.push_back(a);

        m_abLis.insert(std::pair<int,std::vector<AxisButtonListener*>>(i,vec));

        a->setListener(this);
    }

    void JoystickListener::addListener(ConnectionListener* a, int)
    {
       m_conLis.push_back(a);
       a->setListener(this);
    }

    void AxisButtonListener::setListener(JoystickListener* l)
    {
        m_listener = l;
    }

    void ConnectionListener::setListener(JoystickListener* l)
    {
        m_listener = l;
    }


}

