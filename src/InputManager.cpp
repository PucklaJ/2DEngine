#include "InputManager.h"
#include <iostream>
#include <LogManager.h>

using namespace std;

typedef map<unsigned int,bool>::iterator mapIt;

namespace SDL
{
    InputManager::InputManager(int order) : Actor(order,"InputManager")
    {
        m_visible = false;
    }

    InputManager::~InputManager()
    {

    }
    bool InputManager::justPressed(unsigned int kc)
    {
        //std::cout << "Checking justPressed" << std::endl;
        bool re = isPressed(kc) && !wasPressed(kc);
        return re;
    }

    bool InputManager::isPressed(unsigned int kc)
    {
        //std::cout << "Checking isPressed" << std::endl;
        mapIt it = m_keyMap.find(kc);

        if(it != m_keyMap.end())
        {
            return m_keyMap[kc];
        }
        
        //std::cout << "Checked isPressed" << std::endl;

        return false;
    }

    bool InputManager::wasPressed(unsigned int kc)
    {
        //std::cout << "Check wasPressed" << std::endl;
        mapIt it = m_previousKeyMap.find(kc);
        if(it != m_previousKeyMap.end())
        {
            return m_previousKeyMap[kc];
        }
        
        //std::cout << "Checked wasPressed" << std::endl;

        return false;
    }

    bool InputManager::update()
    {
        for(mapIt it = m_keyMap.begin();it != m_keyMap.end();it++)
        {
            m_previousKeyMap[it->first] = it->second;
        }

        return true;
    }

    void InputManager::releaseKey(unsigned int kc)
    {
        m_keyMap[kc] = false;
    }

    void InputManager::pressKey(unsigned int kc)
    {
        m_keyMap[kc] = true;
    }

    void InputManager::setMouseCoords(unsigned int x, unsigned int y)
    {
        m_mouseCords.x = x;
        m_mouseCords.y = y;
    }
}









