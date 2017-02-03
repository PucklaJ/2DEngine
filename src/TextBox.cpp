#include "TextBox.h"
#include "GUI.h"
#ifdef __linux__
    #include <SDL2/SDL_ttf.h>
#else
#ifdef _WIN32
    #include <TTF/SDL_ttf.h>
#else
    #include <SDL2_ttf/SDL_ttf.h>
#endif
#endif
#include <iostream>
#include "MainClass.h"
#include "Camera.h"

namespace SDL
{
    int* TextBox::m_currentActive = nullptr;

    TextBox::TextBox(TTF_Font* font,const Vector2& pos,SDL_Color tColor) : GUI_Element(pos,"TextBox"),
        m_text(" "),
        m_textColor(tColor),
        m_textSurface(nullptr),
        m_textTexture(nullptr),
        m_font(font)
    {
        //ctor
    }

    TextBox::~TextBox()
    {
        //dtor
    }

    const Vector2 TextBox::customCameraScalingBeforeRendering()
    {
        Vector2 temp = m_textPos;
        m_gui->getMainClass()->getCamera()->scalePos(m_textPos);
        return temp;
    }

    void TextBox::customCameraScalingAfterRendering(const Vector2& temp)
    {
        m_gui->getMainClass()->getCamera()->rescalePos(m_textPos,temp);
    }



    bool TextBox::render()
    {
        m_textDst.x = m_position.getX();
        m_textDst.y = m_position.getY();
        m_textDst.w = m_textSize.getX();
        m_textDst.h = m_textSize.getY();

        if(m_backGround)
        SDL_RenderCopy(m_gui->getMainClass()->getRenderer(),m_backGround,nullptr,&m_dstRect);
        if(m_textTexture)
        SDL_RenderCopy(m_gui->getMainClass()->getRenderer(),m_textTexture,nullptr,&m_textDst);

        return true;
    }

    bool TextBox::update()
    {
        if(m_currentActive && *m_currentActive == m_id && m_gui->getMainClass()->getInputManager()->justPressed(SDLK_BACKSPACE))
        {
            setText(m_text.substr(0,m_text.length()-1));
        }

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
                    m_currentActive = &m_id;

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

            if(m_gui->getMainClass()->getInputManager()->justPressed(SDL_BUTTON_LEFT))
            {
                if(m_currentActive && *m_currentActive == m_id)
                {
                    m_currentActive = nullptr;
                }
            }
        }

        if(m_needsTextUpdate)
            generateText();

        return true;
    }

    void TextBox::quit()
    {
        delete m_textSurface;
        m_textSurface = nullptr;
    }

    void TextBox::generateText()
    {
        if(m_textSurface) delete m_textSurface;
        if(m_textTexture){SDL_DestroyTexture(m_textTexture);}

        m_textSurface = TTF_RenderText_Solid(m_font,m_text.c_str(),m_textColor);
        m_textTexture = SDL_CreateTextureFromSurface(m_gui->getMainClass()->getRenderer(),m_textSurface);

        int w,h;
        SDL_QueryTexture(m_textTexture,nullptr,nullptr,&w,&h);
        m_textSize.setX(w);
        m_textSize.setY(h);
        m_textSize *= m_textScale;

        m_needsTextUpdate = false;
    }

    void TextBox::setText(const std::string& text)
    {
        if(m_text != text)
            m_needsTextUpdate = true;
        m_text = text;
    }

    void TextBox::setBackground(SDL_Texture* tex)
    {
        m_backGround = tex;
    }

    void TextBox::setTextColor(const SDL_Color& col)
    {
        m_textColor = col;
        setText(m_text);
    }

    void TextBox::insertText(const std::string& str)
    {
        if(m_currentActive && *m_currentActive == m_id && m_textSize.getX() < m_size.getX())
        {
            setText(m_text + str);
            if(m_needsTextUpdate)
                generateText();
        }
    }

}

