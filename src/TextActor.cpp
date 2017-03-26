#include <TextActor.h>
#include <MainClass.h>
#include <iostream>

namespace SDL
{
    TextActor::TextActor(TTF_Font* font,const SDL_Color& color,const std::string& text,int order) : Sprite(order),
        m_text(text),
        m_font(font),
        m_color(color)
    {
        setName(text.c_str());
    }
    
    TextActor::~TextActor()
    {
        
    }
    
    bool TextActor::init()
    {
        generateText();
        return true;
    }
    
    bool TextActor::update()
    {
        if(m_needsUpdate)
        {
            generateText();
            m_needsUpdate = false;
        }
        return true;
    }
    
    void TextActor::generateText()
    {
        if(m_mainClass)
        {
            if(m_texture)
            {
                delete m_texture;
                m_texture = nullptr;
            }
            
            SDL_Surface* sur = TTF_RenderText_Solid(m_font,m_text.c_str(),m_color);
            setTexture(new TextureHandle(m_mainClass->getRenderer(),sur));
            SDL_FreeSurface(sur);
            
        }
        
    }
    
}
