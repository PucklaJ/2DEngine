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
#include <Music.h>
#include <Sound.h>

namespace SDL
{
    typedef map<std::string,TextureHandle*>::iterator TextureIterator;
    typedef map<pair<std::string,int>,TTF_Font*>::iterator FontIterator;
    typedef map<std::string,Music*>::iterator MusicIterator;
    typedef map<std::string,Sound*>::iterator SoundIterator;

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
        clearMusics();
        clearSounds();
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

    Music* ResourceManager::loadMusic(const std::string& file)
    {
    	Music* music = nullptr;

		MusicIterator it;

		for(it = m_musics.begin();it!=m_musics.end();it++)
		{
			if(it->first.compare(file) == 0)
			{
				break;
			}
		}

		if(it != m_musics.end())
		{
			music = it->second;
		}
		else
		{
			music = new Music(Mix_LoadMUS(file.c_str()));

			if(music->getMusic() == nullptr)
			{
				LogManager::log(std::string("Error: ") + Mix_GetError());
			}
			else
			{
				m_musics[file] = music;
			}

		}

		return music;
    }
    Sound* ResourceManager::loadSound(const std::string& file)
    {
    	Sound* sound = nullptr;

		SoundIterator it;

		for(it = m_sounds.begin();it!=m_sounds.end();it++)
		{
			if(it->first.compare(file) == 0)
			{
				break;
			}
		}

		if(it != m_sounds.end())
		{
			sound = it->second;
		}
		else
		{
			sound = new Sound(Mix_LoadWAV(file.c_str()));

			if(sound->getChunk() == nullptr)
			{
				LogManager::log(std::string("Error: ") + Mix_GetError());
			}
			else
			{
				m_sounds[file] = sound;
			}

		}

		return sound;
    }

    void ResourceManager::clearMusics()
    {
    	for(MusicIterator it = m_musics.begin();it!=m_musics.end();it++)
		{
			if(it->second)
				delete it->second;
		}

		m_musics.clear();
    }
	void ResourceManager::deleteMusic(Music* m)
	{
		for(MusicIterator it = m_musics.begin();it!=m_musics.end();it++)
		{
			if(it->second == m)
			{
				delete it->second;
				m_musics.erase(it);
				break;
			}
		}
	}
	void ResourceManager::clearSounds()
	{
		for(SoundIterator it = m_sounds.begin();it!=m_sounds.end();it++)
		{
			if(it->second)
				delete it->second;
		}

		m_musics.clear();
	}
	void ResourceManager::deleteSound(Sound* c)
	{
		for(SoundIterator it = m_sounds.begin();it!=m_sounds.end();it++)
		{
			if(it->second == c)
			{
				delete it->second;
				m_sounds.erase(it);
				break;
			}
		}
	}

	bool ResourceManager::isLoaded(Music* m)
	{
		for(MusicIterator it = m_musics.begin();it!=m_musics.end();it++)
		{
			if(it->second == m)
				return true;
		}

		return false;
	}
	bool ResourceManager::isLoaded(Sound* c)
	{
		for(SoundIterator it = m_sounds.begin();it!=m_sounds.end();it++)
		{
			if(it->second == c)
				return true;
		}

		return false;
	}
}



