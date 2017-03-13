#include "Light.h"
#include "Colors.h"
#include "MainClass.h"
#include <iostream>

namespace SDL
{
    Light::Light(const SDL_Color& color,double radius,const char* name) : Sprite(),
        m_color(color),
        m_radius(radius)
    {
        setName(name);
    }
    
    Light::~Light()
    {
        
    }
    
    bool Light::init()
    {
        m_texture = Textures::CIRCLE(m_mainClass,m_radius,Colors::WHITE);
        m_texture->setColorMod(m_color);
        m_texture->setAlphaMod(m_color.a);
        m_size.set(m_radius,m_radius);
        
        return true;
    }
    
    bool Light::update()
    {
        return true;
    }
    
    bool Light::m_render()
    {
        if(!m_renderChildrenAfter && !Actor::m_render())return false;
        
        SDL_SetRenderTarget(m_mainClass->getRenderer(),m_mainClass->getAmbientSprite()->getTexture()->getTexture());
        SDL_SetRenderDrawColor(m_mainClass->getRenderer(),m_color.r,m_color.g,m_color.b,m_color.a);
        m_texture->renderCopy(m_mainClass->getRenderer(),&m_dstRect,nullptr);
        SDL_SetRenderTarget(m_mainClass->getRenderer(),m_mainClass->getBackBuffer());
        
        if(m_renderChildrenAfter)return Actor::m_render();
        
        return true;
    }
    
    bool Light::render()
    {
        return true;
    }
    
    void Light::setRadius(double radius)
    {
        if(radius <= 0.0)
            return;
        
        m_radius = radius;
        m_texture->destroy();
        
        m_texture = Textures::CIRCLE(m_mainClass,m_radius,Colors::WHITE);
        m_texture->setColorMod(m_color);
        m_texture->setAlphaMod(m_color.a);
        m_size.set(m_radius,m_radius);
    }
    
    void Light::setColor(const SDL_Color& color)
    {
        m_color = color;
        m_texture->setColorMod(m_color);
        m_texture->setAlphaMod(m_color.a);
    }
}

