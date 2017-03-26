#include <GUI_Element.h>
#include <GUI.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>

namespace SDL
{
    typedef std::map<GUI_Event,EventFunction>::iterator EventIterator;

    std::vector<int> GUI_Element::m_ids;

    GUI_Element::GUI_Element(const Vector2& pos,const char* name) : Actor(0,name),
        m_gui(nullptr),
        hover(false),
        clicked(false)
    {
        setPosition(pos);
        do
        {
            m_id = rand();
        }while(std::find(m_ids.begin(),m_ids.end(),m_id) != m_ids.end());
    }

    GUI_Element::~GUI_Element()
    {
        for(std::size_t i = 0;i<m_ids.size();i++)
        {
            if(m_ids[i] == m_id)
            {
                m_ids[i] = m_ids.back();
                m_ids.pop_back();
                break;
            }
        }
    }

    bool GUI_Element::callEvent(GUI_Event e)
    {
        if(m_eventFunctions.empty())
        {
            return true;
        }

        for(EventIterator it = m_eventFunctions.begin();it!=m_eventFunctions.end();it++)
        {
            if(it->first == e)
            {
                EventFunction func = it->second;
                return (*func)(this);
            }
        }

        return true;
    }

    void GUI_Element::addEvent(GUI_Event e,EventFunction func)
    {
        bool isThere = false;

        for(EventIterator it = m_eventFunctions.begin();it!=m_eventFunctions.end();it++)
        {
            if(it->first == e)
            {
                isThere = true;
                break;
            }
        }

        if(isThere)
        {
            m_eventFunctions[e] = func;
        }
        else
        {
            m_eventFunctions.insert(std::pair<GUI_Event,EventFunction>(e,func));
        }
    }

    void GUI_Element::insertText(const std::string& str)
    {

    }

    GUI* GUI_Element::getGUI()
    {
        return m_gui;
    }

    void GUI_Element::setGUI(GUI* g)
    {
        m_gui = g;
    }
}
