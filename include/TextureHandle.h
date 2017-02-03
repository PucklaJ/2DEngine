#ifndef TEXTURE_HANDLE_H
#define TEXTURE_HANDLE_H
#include <SDL2/SDL.h>

namespace SDL
{
    class TextureHandle
    {
    public:
        TextureHandle(SDL_Texture*);
        TextureHandle(SDL_Renderer*,Uint32,int,int,int);
        TextureHandle(SDL_Renderer*,SDL_Surface*);
        ~TextureHandle();
        
        SDL_Texture* getTexture() {return m_texture;}
        int getWidth();
        int getHeight();
        Uint32 getPixelFormat();
        int getAccess();
        Uint8 getAlphaMod();
        SDL_Color getColorMod();
        SDL_BlendMode getBlendMode();
        
        void setAlphaMod(Uint8);
        void setColorMod(const SDL_Color&);
        void setBlendMode(SDL_BlendMode);
        int setRenderTarget(SDL_Renderer*);
        
        int renderCopy(SDL_Renderer*,SDL_Rect* dst = nullptr,SDL_Rect* src = nullptr);
        int renderCopyEx(SDL_Renderer*,double,SDL_Rect* dst = nullptr,SDL_Rect* src = nullptr,SDL_Point* center = nullptr,SDL_RendererFlip flip = SDL_FLIP_NONE);
        
        int lock(SDL_Rect* r = nullptr,void* pixels = nullptr,int* pitch = nullptr);
        void unlock();
        int getPitch() {return m_pitch;}
        
        int loadPixels(SDL_Renderer*,void* pixels = nullptr,const SDL_Rect* r = nullptr,int bytesPerPixel = 32);
        void unloadPixels();
        SDL_Color getPixel(int x,int y);
        
        void setPixels(void*,int size = -1);
        
        void destroy();
        
    private:
        SDL_Texture* m_texture;
        void* m_pixels = nullptr;
        int m_pitch;
        
    };
}

extern bool operator==(SDL::TextureHandle,SDL::TextureHandle*);
extern bool operator==(SDL::TextureHandle*,SDL::TextureHandle);

#endif