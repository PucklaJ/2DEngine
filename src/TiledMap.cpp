#include "TiledMap.h"
#include "LogManager.h"
#include "operators.h"
#include "MainClass.h"
#include "ResourceManager.h"
#include <iostream>
#include "mathematics.h"
#include "Physics.h"
#include <Box2D/Box2D.h>

namespace SDL
{
    TiledMap::TiledMap(const std::string& file) : Sprite(0)
    {
        LogManager::log("TiledMap Constructor");
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
            
            if(m_tmxMap->GetOrientation() != Tmx::MapOrientation::TMX_MO_ORTHOGONAL)
            {
                LogManager::log(std::string("This Orientation is not supported (") + m_tmxMap->GetOrientation() + ")");
                return true;
            }
        
            m_texture = new TextureHandle(m_mainClass->getRenderer(),m_mainClass->getWindow()->getPixelFormat(),SDL_TEXTUREACCESS_TARGET,m_tmxMap->GetWidth() * m_tmxMap->GetTileWidth(),m_tmxMap->GetHeight() * m_tmxMap->GetTileHeight());
            m_texture->setBlendMode(SDL_BLENDMODE_BLEND);
            
            int tilesX[m_tmxMap->GetNumTilesets()];
            
            LogManager::log("Loading Tilesets...");
            
            for(size_t i = 0;i<m_tmxMap->GetNumTilesets();i++)
            {
                const Tmx::Tileset* tileSet = m_tmxMap->GetTileset((int)i);
                std::string imageFile = m_tmxMap->GetFilepath() + m_tmxMap->GetTileset((int)i)->GetImage()->GetSource();
                TextureHandle* texture = m_mainClass->getResourceManager()->loadTexture(imageFile);
                
                tilesX[i] = (tileSet->GetImage()->GetWidth()-tileSet->GetMargin()*2+tileSet->GetSpacing())/(tileSet->GetTileWidth()+tileSet->GetSpacing());
            }
            
            LogManager::log("Loading Tiles..");
            
            m_texture->setRenderTarget(m_mainClass->getRenderer());
            SDL_SetRenderDrawBlendMode(m_mainClass->getRenderer(),SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(m_mainClass->getRenderer(),0,0,0,0);
            SDL_RenderClear(m_mainClass->getRenderer());
            
            int textureX,textureY,index,tileID;
            std::string imageFile;
            TextureHandle* texture = nullptr;
            SDL_Rect dst,src;
            
            for(size_t i = 0;i<m_tmxMap->GetNumTileLayers();i++)
            {
                const Tmx::TileLayer* tileLayer = m_tmxMap->GetTileLayer((int)i);
                
                for(int y = 0;y < tileLayer->GetHeight();y++)
                {
                    for(int x = 0;x < tileLayer->GetWidth();x++)
                    {
                        if(tileLayer->GetTileTilesetIndex(x,y) == -1)
                            continue;
                        
                        index = tileLayer->GetTileTilesetIndex(x,y);
                        tileID = tileLayer->GetTileId(x,y);
                        
                        const Tmx::Tileset* tileSet = m_tmxMap->GetTileset(index);
                        imageFile = m_tmxMap->GetFilepath() + tileSet->GetImage()->GetSource();
                        const Tmx::MapTile& tile = tileLayer->GetTile(x, y);
                        
                        texture = m_mainClass->getResourceManager()->loadTexture(imageFile);
                        
                        getSrcRectForTileID(&src,tilesX[index],index,tileID);
                        
                        dst.x = x*m_tmxMap->GetTileWidth();
                        dst.y = y*m_tmxMap->GetTileHeight();
                        dst.w = tileSet->GetTileWidth();
                        dst.h = tileSet->GetTileHeight();
                        
                        if(tile.flippedHorizontally || tile.flippedVertically)
                        {
                            if(tile.flippedHorizontally)
                            {
                                texture->renderCopyEx(m_mainClass->getRenderer(), 0.0,&dst,&src,nullptr,SDL_FLIP_HORIZONTAL);
                            }
                            else
                            {
                                texture->renderCopyEx(m_mainClass->getRenderer(), 0.0,&dst,&src,nullptr,SDL_FLIP_VERTICAL);
                            }
                        }
                        else
                        {
                            texture->renderCopy(m_mainClass->getRenderer(),&dst,&src);
                        }
                        
                    }
                }
            }
            
            LogManager::log("Loading Objects...");
            
            for(int i = 0;i<m_tmxMap->GetNumObjectGroups();i++)
            {
                const Tmx::ObjectGroup* objectGroup = m_tmxMap->GetObjectGroup(i);
                
                for(int j = 0;j<objectGroup->GetNumObjects();j++)
                {
                    std::cout << "Object" << std::endl;
                    const Tmx::Object* obj = objectGroup->GetObject(j);
                    std::cout << "+ Position: " << obj->GetX() << "; " << obj->GetY() << std::endl;
                    std::cout << "+ Rotation: " << obj->GetRot() << std::endl;
                    std::cout << "+ Height: " << obj->GetHeight() << std::endl;
                    std::cout << "+ Width: " << obj->GetWidth() << std::endl;
                    std::cout << "+ Name: " << obj->GetName() << std::endl;
                    std::cout << "+ Typ: " << obj->GetType() << std::endl;
                    std::cout << "+ GID: " << obj->GetGid() << std::endl;
                    if(obj->GetEllipse())
                    {
                        //std::cout << "+ Ellipse" << std::endl;
                        const Tmx::Ellipse* elipse = obj->GetEllipse();
                        //std::cout << "++ Center: " << elipse->GetCenterX() << "; " << elipse->GetCenterY() << std::endl;
                        //std::cout << "++ Radius: " << elipse->GetRadiusX() << "; " << elipse->GetRadiusY() << std::endl;
                    }
                    if(obj->GetPolygon())
                    {
                        //std::cout << "+ Polygon" << std::endl;
                        const Tmx::Polygon* polygon = obj->GetPolygon();
                        //std::cout << "++ Points" << std::endl;
                        for(int k = 0;k<polygon->GetNumPoints();k++)
                        {
                            const Tmx::Point& point = polygon->GetPoint(k);
                            //std::cout << "+++ " << k+1 << ". " << point.x << "; " << point.y << std::endl;
                        }
                    }
                    if(obj->GetPolyline())
                    {
                        //std::cout << "+ Polyline" << std::endl;
                        const Tmx::Polyline* polyline = obj->GetPolyline();
                        //std::cout << "++ Points" << std::endl; 
                        for(int k = 0;k<polyline->GetNumPoints();k++)
                        {
                            const Tmx::Point& point = polyline->GetPoint(k);
                            //std::cout << "+++ " << k+1 << ". " << point.x << "; " << point.y << std::endl;
                        }
                    }
                    
                    const Tmx::Tileset* gidTileSet = getTileset(obj->GetGid());
                    if(gidTileSet)
                    {
                        getSrcRectForTileGID(&src,obj->GetGid());
                        dst.x = obj->GetX();
                        dst.y = obj->GetY()-obj->GetHeight();
                        dst.w = obj->GetWidth();
                        dst.h = obj->GetHeight();
                        
                        texture = m_mainClass->getResourceManager()->loadTexture(m_tmxMap->GetFilepath() + gidTileSet->GetImage()->GetSource());
                        texture->renderCopy(m_mainClass->getRenderer(),&dst,&src);
                    }
                }
            }
        }
        
        SDL_SetRenderTarget(m_mainClass->getRenderer(),m_mainClass->getBackBuffer());
        
        if(m_texture)
        {
            m_srcRect.x = 0;
            m_srcRect.y = 0;
            SDL_QueryTexture(m_texture->getTexture(),nullptr,nullptr,&m_srcRect.w,&m_srcRect.h);
            
            m_size.set(m_texture->getWidth(),m_texture->getHeight());
        }
        
        
        LogManager::log("Finished Loading Map!");
       
        return true;
    }
    
    const Tmx::Tileset* TiledMap::getTileset(int gid)
    {
        int curGID = 0;
        
        for(int i = 0;i<m_tmxMap->GetNumTilesets();i++)
        {
            curGID = m_tmxMap->GetTileset(i)->GetFirstGid();
            
            std::cout << "First GID: " << curGID << std::endl;
            
            if(curGID == gid)
            {
                return m_tmxMap->GetTileset(i);
            }
            else if(i != 0 && curGID > gid)
            {
                return m_tmxMap->GetTileset(i-1);
            }
            else if(i + 1 == m_tmxMap->GetNumTilesets())
            {
                return m_tmxMap->GetTileset(i);
            }
        }
        
        return nullptr;
    }
    
    void TiledMap::getSrcRectForTileGID(SDL_Rect* src,int gid)
    {
        const Tmx::Tileset* tileSet = getTileset(gid);
        int id = gid - tileSet->GetFirstGid();
        int numTilesX = (tileSet->GetImage()->GetWidth()-tileSet->GetMargin()*2+tileSet->GetSpacing())/(tileSet->GetTileWidth()+tileSet->GetSpacing());
        
        src->x = id % numTilesX;
        src->y = (id - src->x) / numTilesX;
        
        src->x = src->x * (tileSet->GetTileWidth()+tileSet->GetSpacing())+tileSet->GetMargin();
        src->y = src->y * (tileSet->GetTileHeight()+tileSet->GetSpacing())+tileSet->GetMargin();
        src->w = tileSet->GetTileWidth();
        src->h = tileSet->GetTileHeight();
    }
    
    void TiledMap::getSrcRectForTileID(SDL_Rect* src,int numTilesX,int index,int id)
    {
        src->x = id % numTilesX;
        src->y = (id - src->x) / numTilesX;
        
        const Tmx::Tileset* tileSet = m_tmxMap->GetTileset(index);
        
        src->x = src->x * (tileSet->GetTileWidth()+tileSet->GetSpacing())+tileSet->GetMargin();
        src->y = src->y * (tileSet->GetTileHeight()+tileSet->GetSpacing())+tileSet->GetMargin();
        src->w = tileSet->GetTileWidth();
        src->h = tileSet->GetTileHeight();
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