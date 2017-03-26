#include <GUI.h>
#include <GUI_Element.h>
#include <MainClass.h>
#include <iostream>
#include <LogManager.h>
#include <Camera.h>

namespace SDL
{
    GUI::GUI(int order) : Actor(order,"GUI")
    {
        m_affectedByScaling = false;
        m_isAffectedByCamera = false;
        m_size = Vector2(10000,10000);
    }

    GUI::GUI(const std::vector<GUI_Element*>& elements,int order) : Actor(order,"GUI")
    {
        for(std::size_t i = 0;i<elements.size();i++)
        {
            addGUIElement(elements[i]);
        }
        m_affectedByScaling = false;
        m_isAffectedByCamera = false;
        m_size = Vector2(10000,10000);
    }

    GUI::~GUI()
    {
        //dtor
    }

    bool GUI::update()
    {
        for(std::size_t i = 0;i<m_guiElements.size();i++)
        {
            if(!m_guiElements[i]->m_all_update())
                return false;
        }


        return true;
    }

    bool GUI::render()
    {
        if(m_isAffectedByCamera)
        {
            for(std::size_t i = 0;i<m_guiElements.size();i++)
            {
                if(!m_mainClass->getCamera()->renderActor(m_guiElements[i]))
                {
                    return false;
                }

            }
        }
        else
        {
            for(std::size_t i = 0;i<m_guiElements.size();i++)
            {
                if(!m_guiElements[i]->m_render_render())
                {
                    return false;
                }

            }
        }

        return true;
    }

    void GUI::quit()
    {
        for(std::size_t i = 0;i<m_guiElements.size();i++)
        {
            m_guiElements[i]->quit();
        }

        m_guiElements.clear();
    }

    void GUI::insertText(const std::string& str)
    {
        for(std::size_t i = 0;i<m_guiElements.size();i++)
        {
            m_guiElements[i]->insertText(str);
        }
    }

    void GUI::removeGUIElement(GUI_Element* ge,bool del)
    {
        for(std::size_t i = 0;i<m_guiElements.size();i++)
        {
            if(m_guiElements[i]->getID() == ge->getID())
            {
                m_guiElements[i]->setGUI(nullptr);
                if(del)
                {
                    ge->quit();
                    delete ge;
                }
                m_guiElements[i] = m_guiElements.back();
                m_guiElements.pop_back();
                break;
            }
        }
    }

    void GUI::addGUIElement(GUI_Element* ge)
    {
        if(ge->getGUI() != nullptr && ge->getGUI()->getID() == getID())
            return;

        if(ge->getGUI() != nullptr)
        {
            ge->getGUI()->removeGUIElement(ge,false);
        }

        ge->setGUI(this);
        ge->setMainClass(getMainClass());
        ge->init();
        m_guiElements.push_back(ge);
    }

}

