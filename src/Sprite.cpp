#include <Sprite.h>
#include <MainClass.h>
#include <ResourceManager.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <LogManager.h>
#include <operators.h>
#include <SDL2_gfxPrimitives.h>
#include <Colors.h>

namespace SDL
{
    namespace Textures
    {
        TextureHandle* BOX(MainClass* main,const Vector2& size,const SDL_Color& color)
        {
            if(true)
            {
                TextureHandle* tex = new TextureHandle(main->getRenderer(),SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.getX(), size.getY());
            
                SDL_Color drawColor;
                
                SDL_GetRenderDrawColor(main->getRenderer(),&drawColor.r,&drawColor.g,&drawColor.b,&drawColor.a);
                
                tex->setRenderTarget(main->getRenderer());
                SDL_SetRenderDrawBlendMode(main->getRenderer(), SDL_BLENDMODE_NONE);
                SDL_SetRenderDrawColor(main->getRenderer(), color.r, color.g, color.b, color.a);
                SDL_RenderClear(main->getRenderer());
                tex->setBlendMode(SDL_BLENDMODE_BLEND);
                
                SDL_SetRenderDrawColor(main->getRenderer(),drawColor.r,drawColor.g,drawColor.b,drawColor.a);
                SDL_SetRenderTarget(main->getRenderer(),main->getBackBuffer());

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
                
                tex = new TextureHandle(main->getRenderer(),sur);
                if(tex->getTexture() == nullptr)
                {
                    LogManager::log(std::string("Couldn't create Texture: ") + SDL_GetError());
                    return nullptr;
                }
                SDL_FreeSurface(sur);
                
                return tex;
            }
            
            
        }
        
        TextureHandle* CIRCLE(MainClass* main,double rad,const SDL_Color& col,bool filled,const SDL_Color* back)
        {
            TextureHandle* tex = new TextureHandle(main->getRenderer(),SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,rad*2.0+1,rad*2.0+1);
            SDL_Color pc;
            SDL_GetRenderDrawColor(main->getRenderer(),&pc.r,&pc.g,&pc.b,&pc.a);
            SDL_SetRenderDrawBlendMode(main->getRenderer(),SDL_BLENDMODE_NONE);
            tex->setRenderTarget(main->getRenderer());
            if(back)
            {
                SDL_SetRenderDrawColor(main->getRenderer(),back->r,back->g,back->b,back->a);
            }
            else
            {
                SDL_SetRenderDrawColor(main->getRenderer(),0,0,0,0);
            }
            SDL_RenderClear(main->getRenderer());
            
            if(filled)
            {
                filledCircleRGBA(main->getRenderer(),rad,rad,rad,col.r,col.g,col.b,col.a);
            }
            else
            {
                aacircleRGBA(main->getRenderer(),rad,rad,rad,col.r,col.g,col.b,col.a);
            }
            tex->setBlendMode(SDL_BLENDMODE_BLEND);
            
            
            SDL_SetRenderDrawColor(main->getRenderer(),pc.r,pc.g,pc.b,pc.a);
            SDL_SetRenderTarget(main->getRenderer(),main->getBackBuffer());
            
            return tex;
        }
    }
    
    namespace Colors
    {
    	SDL_Color* m_COLOR_KEY = nullptr;

        SDL_Color RAND(short r,short g, short b)
        {
            SDL_Color col;
            col.r = r != -1 ? r : rand()%256;
            col.g = g != -1 ? g : rand()%256;
            col.b = b != -1 ? b : rand()%256;
            col.a = 255;
            
            return col;
        }

        SDL_Color* KEY(short r,short g,short b)
        {
        	if(!m_COLOR_KEY)
        	{
        		m_COLOR_KEY = new SDL_Color();
        		m_COLOR_KEY->a = 255;
        	}

        	m_COLOR_KEY->r = r;
        	m_COLOR_KEY->g = g;
        	m_COLOR_KEY->b = b;

        	return m_COLOR_KEY;
        }
    }
    

    Sprite::Sprite(int order) : Actor(order,"Sprite"),
        m_srcRect(SDL_Rect()),
        m_rotation(0.0)
    {
        
    }
    
    Sprite::Sprite(const char* file,const SDL_Color* colorKey,int order) : Actor(order,"Sprite"),
        m_srcRect(SDL_Rect()),
        m_rotation(0.0),
        m_file(file),
		m_colorKey(colorKey)

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
            m_size = Vector2(m_srcRect.w,m_srcRect.h);
        }
    }

    bool Sprite::init()
    {
        if(!m_texture)
        {
            ResourceManager* res = m_mainClass->getResourceManager();
            m_texture = res->loadTexture(m_file,m_colorKey);
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
        
        if(m_texture)
        {
            if(m_flip == SDL_FLIP_NONE &&  (m_rotation == 0.0 || !((int)m_rotation % 360)))
            {
                m_texture->renderCopy(m_renderer,&m_dstRect,&m_srcRect);
            }
            else
            {
                m_texture->renderCopyEx(m_renderer,m_rotation,&m_dstRect,&m_srcRect,nullptr,m_flip);
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
    {
        if(m_position.getX() + m_size.getX() > v.getX() &&
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
               m_position.getX()-m_size.getX() > NATIVE_W || m_position.getY()-m_size.getY() > NATIVE_H)
            {
                return false;
            }
        }
        
        return true;
    }
}


