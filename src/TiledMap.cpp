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
                
                tilesX[i] = 0;
                
                while(tilesX[i]*(tileSet->GetTileWidth() + tileSet->GetSpacing()) + tileSet->GetMargin() <= texture->getWidth())
                {
                    tilesX[i]++;
                }
                
                tilesX[i]--;
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
                        
                        
                        textureX = tileID % tilesX[index];
                        textureY = (tileID - textureX) / tilesX[index];
                        texture = m_mainClass->getResourceManager()->loadTexture(imageFile);
                        
                        textureX = textureX * (tileSet->GetTileWidth()+tileSet->GetSpacing())+tileSet->GetMargin();
                        textureY = textureY * (tileSet->GetTileHeight()+tileSet->GetSpacing())+tileSet->GetMargin();
                        
                        dst.x = x*m_tmxMap->GetTileWidth();
                        dst.y = y*m_tmxMap->GetTileHeight();
                        dst.w = tileSet->GetTileWidth();
                        dst.h = tileSet->GetTileHeight();
                        
                        src.x = textureX;
                        src.y = textureY;
                        src.w = tileSet->GetTileWidth();
                        src.h = tileSet->GetTileHeight();
                        
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
                
                if(objectGroup->GetName().compare(std::string("Collision")) == 0)
                {
                    LogManager::log("Found Collision Layer");
                    
                    if(!m_mainClass->getPhysics())
                    {
                        m_mainClass->activatePhysics();
                    }
                    
                    for(int j = 0;j<objectGroup->GetNumObjects();j++)
                    {
                        const Tmx::Object* object = objectGroup->GetObject(j);
                        b2Body* body = nullptr;
                        b2BodyDef bdef;
                        bdef.position = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2(object->GetX(),object->GetY()));
                        bdef.type = b2_staticBody;
                        
                        body = m_mainClass->getPhysics()->getWorld()->CreateBody(&bdef);
                        LogManager::log("Created new Body");
                        
                        b2FixtureDef fdef;
                        fdef.density = 6;
                        fdef.friction = 0.5;
                        fdef.restitution = 0.5;
                        
                        if(object->GetEllipse())
                        {
                            const Tmx::Ellipse* ellipse = object->GetEllipse();
                            
                            b2FixtureDef fdef;
                            fdef.density = 6;
                            fdef.friction = 0.5;
                            fdef.restitution = 0.5;
                            
                            b2CircleShape shape;
                            shape.m_radius = m_mainClass->getPhysics()->scalarPixelToWorld(ellipse->GetRadiusX());
                            
                            fdef.shape = &shape;
                            
                            body->CreateFixture(&fdef);
                            LogManager::log("Created Ellipse");
                        }
                        if(object->GetPolygon())
                        {
                            const Tmx::Polygon* polygon = object->GetPolygon();
                            
                            b2FixtureDef fdef;
                            fdef.density = 6;
                            fdef.friction = 0.5;
                            fdef.restitution = 0.5;
                            
                            b2PolygonShape shape;
                            b2Vec2 vertecis[100];
                            
                            for(int k;k<polygon->GetNumPoints();k++)
                            {
                                const Tmx::Point& point = polygon->GetPoint(k);
                                vertecis[k] = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2(point.x,point.y));
                            }
                            
                            shape.Set(vertecis, polygon->GetNumPoints());
                            
                            fdef.shape = &shape;
                            
                            body->CreateFixture(&fdef);
                            LogManager::log("Created Polygon");
                        }
                        
                        
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