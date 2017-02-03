#include "TiledMap.h"
#include "LogManager.h"
#include "operators.h"
#include "MainClass.h"
#include "ResourceManager.h"
#include <iostream>
#include "mathematics.h"

namespace SDL
{
    TiledMap::TiledMap(const std::string& file) : Sprite(0)
    {
        m_tmxMap = new Tmx::Map();
        m_tmxMap->ParseFile(file);
        
        if(m_tmxMap->HasError())
        {
            LogManager::log(std::string("TiledMap Error (") + m_tmxMap->GetErrorCode() + ") : " + m_tmxMap->GetErrorText());
            delete m_tmxMap;
            m_tmxMap = nullptr;
        }
    }
    
    TiledMap::~TiledMap()
    {
        
    }
    
    bool TiledMap::init()
    {
        if(m_tmxMap)
        {
             LogManager::log("Loading Map...");
        
            m_texture = new TextureHandle(m_mainClass->getRenderer(),m_mainClass->getWindow()->getPixelFormat(),SDL_TEXTUREACCESS_TARGET,m_tmxMap->GetWidth() * m_tmxMap->GetTileWidth(),m_tmxMap->GetHeight() * m_tmxMap->GetTileHeight());
            m_texture->setBlendMode(SDL_BLENDMODE_BLEND);
            
            int tilesX[m_tmxMap->GetNumTilesets()];
            int tilesY[m_tmxMap->GetNumTilesets()];
            
            LogManager::log("Loading Tilesets...");
            
            for(size_t i = 0;i<m_tmxMap->GetNumTilesets();i++)
            {
                const Tmx::Tileset* tileSet = m_tmxMap->GetTileset((int)i);
                std::string imageFile = m_tmxMap->GetFilepath() + m_tmxMap->GetTileset((int)i)->GetImage()->GetSource();
                TextureHandle* texture = m_mainClass->getResourceManager()->loadTexture(imageFile);
                
                tilesX[i] = 0;
                tilesY[i] = 0;
                while(tilesX[i]*tileSet->GetTileWidth() <= texture->getWidth())
                {
                    tilesX[i]++;
                }
                tilesX[i]--;
                while(tilesY[i]*tileSet->GetTileHeight() <= texture->getHeight())
                {
                    tilesY[i]++;
                }
                tilesY[i]--;
            }
            
            LogManager::log("Loading Tiles..");
            
            m_texture->setRenderTarget(m_mainClass->getRenderer());
            SDL_SetRenderDrawBlendMode(m_mainClass->getRenderer(),SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(m_mainClass->getRenderer(),0,0,0,0);
            SDL_RenderClear(m_mainClass->getRenderer());
            
            for(size_t i = 0;i<m_tmxMap->GetNumTileLayers();i++)
            {
                const Tmx::TileLayer* tileLayer = m_tmxMap->GetTileLayer((int)i);
                
                for(int y = 0;y < tileLayer->GetHeight();y++)
                {
                    for(int x = 0;x < tileLayer->GetWidth();x++)
                    {
                        if(tileLayer->GetTileTilesetIndex(x,y) == -1)
                            continue;
                        
                        int index = tileLayer->GetTileTilesetIndex(x,y);
                        int tileID = tileLayer->GetTileId(x,y);
                        
                        const Tmx::Tileset* tileSet = m_tmxMap->GetTileset(index);
                        std::string imageFile = m_tmxMap->GetFilepath() + tileSet->GetImage()->GetSource();
                        int textureX,textureY;
                        
                        textureX = tileID % tilesX[index];
                        textureY = (tileID - textureX) / tilesX[index];
                        TextureHandle* texture = m_mainClass->getResourceManager()->loadTexture(imageFile);
                        
                        textureX *= tileSet->GetTileWidth();
                        textureY *= tileSet->GetTileHeight();
                        SDL_Rect dst,src;
                        
                        dst.x = x*tileSet->GetTileWidth();
                        dst.y = y*tileSet->GetTileHeight();
                        dst.w = tileSet->GetTileWidth();
                        dst.h = tileSet->GetTileHeight();
                        
                        src.x = textureX;
                        src.y = textureY;
                        src.w = tileSet->GetTileWidth();
                        src.h = tileSet->GetTileHeight();
                        
                        texture->renderCopy(m_mainClass->getRenderer(),&dst,&src);
                    }
                }
            }
        }
        
        SDL_SetRenderTarget(m_mainClass->getRenderer(),m_mainClass->getBackBuffer());
        
        m_srcRect.x = 0;
        m_srcRect.y = 0;
        SDL_QueryTexture(m_texture->getTexture(),nullptr,nullptr,&m_srcRect.w,&m_srcRect.h);
        
        m_size.set(m_texture->getWidth(),m_texture->getHeight());
        
        LogManager::log("Finished Loading Map!");
       
        return true;
    }
    
    bool TiledMap::update()
    {
        return true;
    }
    
    void TiledMap::quit()
    {
        delete m_tmxMap;
        m_tmxMap = nullptr;
    }
}