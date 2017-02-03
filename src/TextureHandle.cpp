#include "TextureHandle.h"
#include <cstring>
#include <iostream>
#include "mathematics.h"
#include "LogManager.h"

namespace SDL
{
    TextureHandle::TextureHandle(SDL_Texture* tex) :
    m_texture(tex)
    {
        
    }
    
    TextureHandle::TextureHandle(SDL_Renderer* r,Uint32 f,int a,int w ,int h)
    {
        m_texture = SDL_CreateTexture(r,f,a,w,h);
    }
    
    TextureHandle::TextureHandle(SDL_Renderer* r,SDL_Surface* s)
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
    
    void TextureHandle::setPixels(void* pixels,int size)
    {
        memcpy(m_pixels,pixels,size == -1 ? ((m_pitch/4)*getHeight()) : size);
    }
    
    void TextureHandle::unlock()
    {
        SDL_UnlockTexture(m_texture);
    }
    
    SDL_Color TextureHandle::getPixel(int x,int y)
    {
        SDL_Color c = {0,0,0,0};
        if(!m_pixels)
        {
            return c;
        }
        
        int w = getWidth();
        Uint32* pixels = (Uint32*)m_pixels;
        Uint8* color = (Uint8*)pixels[y*w+x];
        int length = /*sizeOfArray(color)*/5;
        c.r = color[0];
        for(int i = 0;i<length;i++)
        {
            switch(i)
            {
                case 0:
                    c.r = color[i];
                    break;
                case 1:
                    c.g = color[i];
                    break;
                case 2:
                    c.b = color[i];
                    break;
                case 3:
                    c.a = color[i];
                    break;
            }
        }
        
        return c;
    }
    
    int TextureHandle::loadPixels(SDL_Renderer* r,void* pixels,const SDL_Rect* rect,int bytesPerPixel)
    {
        int rv;
        SDL_Texture* target = SDL_GetRenderTarget(r);
        rv = setRenderTarget(r);
        
        if(rv == 0)
            rv = SDL_RenderReadPixels(r,rect,getPixelFormat(),m_pixels,getWidth()*bytesPerPixel);
        else
            LogManager::log(std::string("Error: ") + SDL_GetError());
        
        SDL_SetRenderTarget(r,target);
        
        return rv;
    }
    void TextureHandle::unloadPixels()
    {
        if(m_pixels)
        {
            delete (Uint32*)m_pixels;
            m_pixels = nullptr;
        }
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



