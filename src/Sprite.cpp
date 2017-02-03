#include "Sprite.h"
#include "MainClass.h"
#include "ResourceManager.h"
#include <SDL2/SDL.h>
#include <iostream>
#include "LogManager.h"
#include "operators.h"

namespace SDL
{
    TextureHandle* createTexture(SDL_Renderer* r,const SDL_Rect& size,const SDL_Color& color)
    {
        if(false)
        {
            TextureHandle* tex = new TextureHandle(r,SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.w, size.h);
        
            SDL_Color drawColor;
            
            SDL_GetRenderDrawColor(r,&drawColor.r,&drawColor.g,&drawColor.b,&drawColor.a);
            
            tex->setRenderTarget(r);
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(r, NULL);
            tex->setBlendMode(SDL_BLENDMODE_BLEND);
            
            SDL_SetRenderDrawColor(r,drawColor.r,drawColor.g,drawColor.b,drawColor.a);

            return tex;
        }
        else
        {
            TextureHandle* tex = nullptr;
            SDL_Surface* sur = SDL_CreateRGBSurface(0,size.w,size.h,32,0,0,0,0);
            if(sur == nullptr)
            {
                LogManager::log(std::string("Couldn't create Texture: ") + SDL_GetError());
                return nullptr;
            }
            if(SDL_FillRect(sur,nullptr,SDL_MapRGBA(sur->format,color.r,color.g,color.b,color.a))<0)
            {
                LogManager::log(std::string("Couldn't create Texture: ") + SDL_GetError());
                return nullptr;
            }
            
            tex = new TextureHandle(r,sur);
            if(tex->getTexture() == nullptr)
            {
                LogManager::log(std::string("Couldn't create Texture: ") + SDL_GetError());
                return nullptr;
            }
            SDL_FreeSurface(sur);
            
            return tex;
        }
        
        
    }
    
    TextureHandle* createTexture(SDL_Renderer* r,const Vector2& size,const SDL_Color& color)
    {
        if(false)
        {
            TextureHandle* tex = new TextureHandle(r,SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.getX(), size.getY());
        
            SDL_Color drawColor;
            
            SDL_GetRenderDrawColor(r,&drawColor.r,&drawColor.g,&drawColor.b,&drawColor.a);
            
            tex->setRenderTarget(r);
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(r, NULL);
            tex->setBlendMode(SDL_BLENDMODE_BLEND);
            
            SDL_SetRenderDrawColor(r,drawColor.r,drawColor.g,drawColor.b,drawColor.a);

            return tex;
        }
        else
        {
            TextureHandle* tex = nullptr;
            SDL_Surface* sur = SDL_CreateRGBSurface(0,size.getX(),size.getY(),32,0,0,0,0);
            if(sur == nullptr)
            {
                LogManager::log(std::string("Couldn't create Texture: ") + SDL_GetError());
                return nullptr;
            }
            if(SDL_FillRect(sur,nullptr,SDL_MapRGBA(sur->format,color.r,color.g,color.b,color.a))<0)
            {
                LogManager::log(std::string("Couldn't create Texture: ") + SDL_GetError());
                return nullptr;
            }
            
            tex = new TextureHandle(r,sur);
            if(tex->getTexture() == nullptr)
            {
                LogManager::log(std::string("Couldn't create Texture: ") + SDL_GetError());
                return nullptr;
            }
            SDL_FreeSurface(sur);
            
            return tex;
        }
        
        
    }

    Sprite::Sprite(int order) : Actor(order,"Sprite"),
        m_srcRect(SDL_Rect()),
        m_rotation(0.0)
    {
        
    }
    
    Sprite::Sprite(const char* file,int order) : Actor(order,"Sprite"),
        m_srcRect(SDL_Rect()),
        m_rotation(0.0),
        m_file(file)

    {

    }

    Sprite::Sprite(TextureHandle* tex,int order) : Actor(order,"Sprite"),
        m_srcRect(SDL_Rect()),
        m_rotation(0.0)

    {
        m_texture = tex;
        if(m_texture)
        {
            SDL_QueryTexture(m_texture->getTexture(),nullptr,nullptr,&m_srcRect.w,&m_srcRect.h);
            m_size = Vector2(m_srcRect.w,m_srcRect.h);        }
    }

    bool Sprite::init()
    {
        if(!m_texture)
        {
            ResourceManager* res = m_mainClass->getResourceManager();
            m_texture = res->loadTexture(m_file);
            SDL_QueryTexture(m_texture->getTexture(),nullptr,nullptr,&m_srcRect.w,&m_srcRect.h);
            m_size = Vector2(m_srcRect.w,m_srcRect.h);
        }

        return true;
    }

    Sprite::~Sprite()
    {
        //dtor
    }

    bool Sprite::update()
    {
        return true;
    }

    bool Sprite::m_update()
    {
        Actor::m_update();

        if(m_needsScaleUpdate)
        {
            m_size *= m_scale;
            m_needsScaleUpdate = false;
        }
        return true;
    }

    bool Sprite::m_render()
    {
        if(!m_renderChildrenAfter)
            Actor::m_render();
        
        if(m_rotation == 0.0 || !((int)m_rotation % 360))
        {
            if(m_texture->renderCopy(m_renderer,&m_dstRect,&m_srcRect)<0)
            {
                LogManager::log(std::string("Render Error: ") + SDL_GetError());
            }

        }
        else
        {
            if(m_texture->renderCopyEx(m_renderer,m_rotation,&m_dstRect,&m_srcRect)<0)
            {
                LogManager::log(std::string("Render Error: ") + SDL_GetError());
            }
        }
        
        if(m_renderChildrenAfter)
            Actor::m_render();
            
        return true;
    }

    bool Sprite::render()
    {
        return true;
    }

    void Sprite::quit()
    {

    }

    void Sprite::m_quit()
    {
        Actor::m_quit();
        
        if(m_texture && !m_mainClass->getResourceManager()->isLoaded(m_texture))
        {
            delete m_texture;
            m_texture = nullptr;
        }
    }
    
    void Sprite::setTexture(TextureHandle* tex)
    {
        m_texture = tex;
        
        SDL_QueryTexture(tex->getTexture(),nullptr,nullptr,&m_srcRect.w,&m_srcRect.h);
        m_size.setX(m_srcRect.w);
        m_size.setY(m_srcRect.h);
    }
    
    bool Sprite::intersects(Sprite* s)
    {
        if(m_position.getX() + m_size.getX() > s->getPosition().getX() &&
           m_position.getX() < s->getPosition().getX() + s->getSize().getX() &&
           m_position.getY() + m_size.getY() > s->getPosition().getY() &&
           m_position.getY() < s->getPosition().getY() + s->getSize().getY())
        {
           return true;
        }
        
        return false;
    }
    
    bool Sprite::intersects(const Vector2& v)
    {        if(m_position.getX() + m_size.getX() > v.getX() &&
           m_position.getX() < v.getX() &&
           m_position.getY() + m_size.getY() > v.getY() &&
           m_position.getY() < v.getY())
        {
           return true;
        }
        
        return false;
    }
    
    bool Sprite::isOnScreen()
    {
        if(m_rotation == 0.0 || !((int)m_rotation % 360))
        {
            return Actor::isOnScreen();
        }
        else
        {
            if(m_position.getX() + m_size.getX()*2.0 < 0 || m_position.getY() + m_size.getY()*2.0 < 0 ||
               m_position.getX()-m_size.getX() > NORM_W || m_position.getY()-m_size.getY() > NORM_H)
            {
                return false;
            }
        }
        
        return true;
    }
}


