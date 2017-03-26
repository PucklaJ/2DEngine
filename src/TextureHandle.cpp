#include "TextureHandle.h"
#include <cstring>
#include <iostream>
#include "mathematics.h"
#include "LogManager.h"

namespace SDL
{
    TextureHandle::TextureHandle(SDL_Texture* tex) :
    m_texture(tex),
	m_pitch(0)
    {
        
    }
    
    TextureHandle::TextureHandle(SDL_Renderer* r,Uint32 f,int a,int w ,int h) :
    		m_pitch(0)
    {
        m_texture = SDL_CreateTexture(r,f,a,w,h);
    }
    
    TextureHandle::TextureHandle(SDL_Renderer* r,SDL_Surface* s) :
    		m_pitch(0)
    {
        m_texture = SDL_CreateTextureFromSurface(r,s);
    }

    TextureHandle::~TextureHandle()
    {
        if(m_texture)
        {
            destroy();
        }
    }

    int TextureHandle::getWidth()
    {
        int w;
        SDL_QueryTexture(m_texture,nullptr,nullptr,&w,nullptr);
        return w;
    }
    int TextureHandle::getHeight()
    {
        int h;
        SDL_QueryTexture(m_texture,nullptr,nullptr,nullptr,&h);
        return h;
    }
    Uint32 TextureHandle::getPixelFormat()
    {
        Uint32 f;
        SDL_QueryTexture(m_texture,&f,nullptr,nullptr,nullptr);
        return f;
    }
    int TextureHandle::getAccess()
    {
        int a;
        SDL_QueryTexture(m_texture,nullptr,&a,nullptr,nullptr);
        return a;
    }
    Uint8 TextureHandle::getAlphaMod()
    {
        Uint8 a;
        SDL_GetTextureAlphaMod(m_texture,&a);
        return a;
    }
    SDL_Color TextureHandle::getColorMod()
    {
        SDL_Color c;
        c.a = 255;
        SDL_GetTextureColorMod(m_texture,&c.r,&c.g,&c.b);
        return c;
    }
    
    void TextureHandle::setAlphaMod(Uint8 a)
    {
        SDL_SetTextureAlphaMod(m_texture,a);
    }
    void TextureHandle::setColorMod(const SDL_Color& c)
    {
        SDL_SetTextureColorMod(m_texture,c.r,c.g,c.b);
    }
    
    int TextureHandle::renderCopy(SDL_Renderer* r,SDL_Rect* dst,SDL_Rect* src)
    {
        return SDL_RenderCopy(r,m_texture,src,dst);
    }
    int TextureHandle::renderCopyEx(SDL_Renderer* r,double angle,SDL_Rect* dst,SDL_Rect* src,SDL_Point* center,SDL_RendererFlip flip)
    {
        return SDL_RenderCopyEx(r,m_texture,src,dst,angle,center,flip);
    }
    
    SDL_BlendMode TextureHandle::getBlendMode()
    {
        SDL_BlendMode b;
        SDL_GetTextureBlendMode(m_texture,&b);
        
        return b;
    }
    
    void TextureHandle::setBlendMode(SDL_BlendMode b)
    {
        SDL_SetTextureBlendMode(m_texture,b);
    }
    
    int TextureHandle::setRenderTarget(SDL_Renderer* r)
    {
        return SDL_SetRenderTarget(r,m_texture);
    }
    
    int TextureHandle::lock(SDL_Rect* r,void* pixels,int* pitch)
    {
        if(m_pixels)
        {
            delete (Uint32*)m_pixels;
            m_pixels = nullptr;
        }
        if(pixels)
        {
            m_pixels = pixels;
        }
        
        return SDL_LockTexture(m_texture,r,pixels ? &pixels : &m_pixels,pitch ? pitch : &m_pitch);
    }
    
    
    void TextureHandle::unlock()
    {
        SDL_UnlockTexture(m_texture);
    }
    
    void TextureHandle::destroy()
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

bool operator==(SDL::TextureHandle t1,SDL::TextureHandle* t2)
{
    return t1.getTexture() == t2->getTexture();
}

bool operator==(SDL::TextureHandle* t1,SDL::TextureHandle t2)
{
    return t2 == t1;
}



