#include "Button.h"
#include "GUI.h"
#include <iostream>
#include "MainClass.h"
#include "LogManager.h"
#include "Camera.h"

namespace SDL
{
    Button::Button(const Vector2& pos) : GUI_Element(pos,"Button"),
        m_backGround(nullptr)
    {
        addEvent(GUI_Event::HOVER_ENTER,&Button::defaultHoverEnter);
        addEvent(GUI_Event::HOVER_LEAVE,&Button::defaultHoverLeave);
    }

    Button::~Button()
    {
        //dtor
    }

    bool Button::defaultHoverEnter(GUI_Element* e)
    {
        Button* btn = static_cast<Button*>(e);
        SDL_SetTextureColorMod(btn->getBackground(),
                               190,
                               190,
                               190);

        return true;
    }

    bool Button::defaultHoverLeave(GUI_Element* e)
    {
        Button* btn = static_cast<Button*>(e);
        SDL_SetTextureColorMod(btn->getBackground(),
                               255,
                               255,
                               255);

        return true;
    }

    void Button::setBackground(SDL_Texture* tex)
    {
        m_backGround = tex;
    }

    bool Button::render()
    {
        if(m_backGround)
            SDL_RenderCopy(m_gui->getMainClass()->getRenderer(),m_backGround,nullptr,&m_dstRect);

        return true;
    }

    bool Button::update()
    {
        if(!m_gui->getMainClass()->getInputManager()->isPressed(SDL_BUTTON_LEFT) && clicked)
        {
            if(!callEvent(GUI_Event::CLICK_UP))
                return false;
            clicked = false;
        }

        if(MOUSE_HOVERS_OVER)
        {
            if(m_gui->getMainClass()->getInputManager()->justPressed(SDL_BUTTON_LEFT))
            {
                    if(!callEvent(GUI_Event::CLICK_DOWN))
                        return false;
                    clicked = true;
            }
            else
            {
                if(!hover)
                {
                    if(!callEvent(GUI_Event::HOVER_ENTER))
                        return false;
                    hover = true;
                }

            }
        }
        else
        {
            if(hover)
            {
                if(!callEvent(GUI_Event::HOVER_LEAVE))
                    return false;
                hover = false;
            }
        }
        return true;
    }

    void Button::quit()
    {
        SDL_DestroyTexture(m_backGround);
        m_backGround = nullptr;
    }
}

