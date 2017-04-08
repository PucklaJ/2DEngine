#include <ResourceManager.h>
#ifndef __APPLE__
#include <SDL2/SDL_image.h>
#else
#include <SDL2_image/SDL_image.h>
#endif
#include <iostream>
#include "MainClass.h"
#include <cstdio>
#include <LogManager.h>
#include <TextureHandle.h>
#include <mathematics.h>

namespace SDL
{
    typedef map<std::string,TextureHandle*>::iterator TextureIterator;
    typedef map<pair<std::string,int>,TTF_Font*>::iterator FontIterator;

    ResourceManager::ResourceManager(SDL_Renderer* renderer) : m_renderer(renderer)
    {

    }

    ResourceManager::~ResourceManager()
    {

    }

    TextureHandle* ResourceManager::loadTexture(const std::string& file,const SDL_Color* colorKey)
    {
        TextureHandle* tex = nullptr;

        TextureIterator it;
        
        for(it = m_textures.begin();it!=m_textures.end();it++)
        {
            if(it->first.compare(file) == 0)
            {
                break;
            }
        }

        if(it != m_textures.end())
        {
            tex = it->second;
        }
        else
        {
        	SDL_Surface* sur = nullptr;
        	if(colorKey)
        	{
        		sur = IMG_Load(file.c_str());
        		if(!sur)
				{
					LogManager::log(std::string("Error: ") + SDL_GetError());
					return nullptr;
				}

        		SDL_SetColorKey(sur,SDL_TRUE,SDL_MapRGBA(sur->format,colorKey->r,colorKey->g,colorKey->b,colorKey->a));
        	}

        	if(colorKey)
        	{
        		tex = new TextureHandle(m_renderer,sur);
        	}
        	else
        	{
        		tex = new TextureHandle(IMG_LoadTexture(m_renderer,file.c_str()));
        	}

			if(tex->getTexture() == nullptr)
			{
				LogManager::log(std::string("Error: ") + SDL_GetError());
			}
			else
			{
				m_textures[file] = tex;
			}

        }

        return tex;
    }

    TTF_Font* ResourceManager::loadFont(const std::string& file, const int size)
    {
        TTF_Font* font = nullptr;

        for(FontIterator it = m_fonts.begin();it!=m_fonts.end();it++)
        {
            if(it->first.first.compare(file) == 0 && it->first.second == size)
            {
                font = it->second;
            }
        }

        if(!font)
        {
            font = TTF_OpenFont(file.c_str(),size);

            if(font == nullptr)
            {
                LogManager::log(std::string("Error: ") + SDL_GetError());
            }
            else
            {
                m_fonts[pair<std::string,int>(file,size)] = font;
            }
        }

        return font;
    }
    
    void ResourceManager::clearFonts()
    {
        for(FontIterator it = m_fonts.begin();it!=m_fonts.end();it++)
        {
            if(it->second)
                TTF_CloseFont(it->second);
        }
        
        m_fonts.clear();
    }
    
    void ResourceManager::deleteFont(TTF_Font* font)
    {
        for(FontIterator it = m_fonts.begin();it!=m_fonts.end();it++)
        {
            if(it->second == font)
            {
                TTF_CloseFont(it->second);
                m_fonts.erase(it);
            }
                
        }
    }
    
    void ResourceManager::clearTextures()
    {
        for(TextureIterator it = m_textures.begin();it!=m_textures.end();it++)
        {
            if(it->second)
                it->second->destroy();
        }
        
        m_textures.clear();
    }
    
    void ResourceManager::deleteTexture(TextureHandle* tex)
    {
        for(TextureIterator it = m_textures.begin();it!=m_textures.end();it++)
        {
            if(it->second == tex)
            {
                delete it->second;
                m_textures.erase(it);
                break;
            }
        }
    }
    
    void ResourceManager::clear()
    {
        clearTextures();
        clearFonts();
    }
    
    bool ResourceManager::isLoaded(TextureHandle* tex)
    {
        for(TextureIterator it = m_textures.begin();it!=m_textures.end();it++)
        {
            if(it->second && it->second == tex)
                return true;
        }
        
        return false;
    }   
    
    bool ResourceManager::isLoaded(TTF_Font* font)
    {
        for(FontIterator it = m_fonts.begin();it!=m_fonts.end();it++)
        {
            if(it->second == font)
                return true;
        }
        
        return false;
    }
}



