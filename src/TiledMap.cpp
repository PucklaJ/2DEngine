#include "TiledMap.h"
#include "LogManager.h"
#include "operators.h"
#include "MainClass.h"
#include "ResourceManager.h"
#include <iostream>
#include "mathematics.h"
#include <Box2D/Box2D.h>
#include <SDL2_gfxPrimitives.h>

#define GETGID(obj) (obj->GetGid() < 0 ? obj->GetGid()+INT_MAX : obj->GetGid())

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
        colorStringToColor("#ffaadd");
        
        if(m_tmxMap)
        {
            LogManager::log("Loading Map...");
            
            if(m_tmxMap->GetOrientation() != Tmx::MapOrientation::TMX_MO_ORTHOGONAL)
            {
                LogManager::log(std::string("This Orientation is not supported (") + m_tmxMap->GetOrientation() + ")");
                return true;
            }
            
            loadTexture();
            
            loadTilesets();
            
            loadLayers();
        }
        
        SDL_SetRenderTarget(m_mainClass->getRenderer(),m_mainClass->getBackBuffer());
        
        if(m_texture)
        {
            m_srcRect.x = 0;
            m_srcRect.y = 0;
            SDL_QueryTexture(m_texture->getTexture(),nullptr,nullptr,&m_srcRect.w,&m_srcRect.h);
            
            m_size.set(m_texture->getWidth(),m_texture->getHeight());
        }
        
        delete m_tilesX;
        m_tilesX = nullptr;
        
        LogManager::log("Finished Loading Map!");
       
        return true;
    }
    
    Uint8 hexCharToInt(char c)
    {
        switch(c)
        {
            case '0':
                return 0;
            case '1':
                return 1;
            case '2':
                return 2;
            case '3':
                return 3;
            case '4':
                return 4;
            case '5':
                return 5;
            case '6':
                return 6;
            case '7':
                return 7;
            case '8':
                return 8;
            case '9':
                return 9;
            case 'A':
            case 'a':
                return 10;
            case 'B':
            case 'b':
                return 11;
            case 'C':
            case 'c':
                return 12;
            case 'D':
            case 'd':
                return 13;
            case 'E':
            case 'e':
                return 14;
            case 'F':
            case 'f':
                return 15;
            default:
                return -1;
        }
    }
    
    SDL_Color TiledMap::colorStringToColor(const std::string& str)
    {
        SDL_Color col;
        
        if(str.length() == 0)
        {
            col.r = col.g = col.b = 128;
            col.a = 255;
            
            return col;
        }
        
        Uint8 num;
        
        for(unsigned int i = 1;i<6;i+=2)
        {
            num = (hexCharToInt(str[i]) << 4) + hexCharToInt(str[i+1]);
            
            switch(i)
            {
                case 1:
                    col.r = num;
                    break;
                case 3:
                    col.g = num;
                    break;
                case 5:
                    col.b = num;
                    break;
                default:
                    break;
            }
        }
        
        return col;
    }
    
    void TiledMap::loadTexture()
    {
        m_background = colorStringToColor(m_tmxMap->GetBackgroundColor());
        
        m_texture = new TextureHandle(m_mainClass->getRenderer(),m_mainClass->getWindow()->getPixelFormat(),SDL_TEXTUREACCESS_TARGET,m_tmxMap->GetWidth() * m_tmxMap->GetTileWidth(),m_tmxMap->GetHeight() * m_tmxMap->GetTileHeight());
        m_texture->setBlendMode(SDL_BLENDMODE_BLEND);
        m_texture->setRenderTarget(m_mainClass->getRenderer());
        SDL_SetRenderDrawBlendMode(m_mainClass->getRenderer(),SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(m_mainClass->getRenderer(),m_background.r,m_background.g,m_background.b,255);
        SDL_RenderClear(m_mainClass->getRenderer());
    }
    
    void TiledMap::loadTilesets()
    {
        LogManager::log("Loading Tilesets...");
        
        m_tilesX = new int[m_tmxMap->GetNumTilesets()];
            
        for(int i = 0;i<m_tmxMap->GetNumTilesets();i++)
        {
            const Tmx::Tileset* tileSet = m_tmxMap->GetTileset((int)i);
            std::string imageFile = m_tmxMap->GetFilepath() + m_tmxMap->GetTileset((int)i)->GetImage()->GetSource();
            
            m_tilesX[i] = (tileSet->GetImage()->GetWidth()-tileSet->GetMargin()*2+tileSet->GetSpacing())/(tileSet->GetTileWidth()+tileSet->GetSpacing());
        }
    }
    
    void TiledMap::loadLayers()
    {
        LogManager::log("Loading Layers..");
            
        for(int i = 0;i<m_tmxMap->GetNumLayers();i++)
        {
            switch(m_tmxMap->GetLayer(i)->GetLayerType())
            {
                case Tmx::LayerType::TMX_LAYERTYPE_TILE:
                    loadTileLayer(static_cast<const Tmx::TileLayer*>(m_tmxMap->GetLayer(i)));
                    break;
                case Tmx::LayerType::TMX_LAYERTYPE_OBJECTGROUP:
                    loadObjectGroup(static_cast<const Tmx::ObjectGroup*>(m_tmxMap->GetLayer(i)));
                    break;
                case Tmx::LayerType::TMX_LAYERTYPE_IMAGE_LAYER:
                    loadImageLayer(static_cast<const Tmx::ImageLayer*>(m_tmxMap->GetLayer(i)));
                    break;
            }
        }
    }
    
    void TiledMap::loadTileLayer(const Tmx::TileLayer* tileLayer)
    {
        if(!tileLayer->IsVisible())
            return;
        
        int index,tileID;
        std::string imageFile;
        TextureHandle* texture;
        SDL_Rect dst,src;
        
        /*std::cout << "-- TileLayer --" << std::endl;
        std::cout << "+ ZOrder: " << tileLayer->GetZOrder() << std::endl;
        std::cout << "+ Opacity: " << tileLayer->GetOpacity() << std::endl;
        std::cout << "+ X: " << tileLayer->GetX() << std::endl;
        std::cout << "+ Y: " << tileLayer->GetY() << std::endl;
        std::cout << "+ ParseOrder: " << tileLayer->GetParseOrder() << std::endl;*/
        
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
                
                getSrcRectForTileID(&src,m_tilesX[index],index,tileID);
                
                dst.x = x*m_tmxMap->GetTileWidth();
                dst.y = y*m_tmxMap->GetTileHeight();
                dst.w = tileSet->GetTileWidth();
                dst.h = tileSet->GetTileHeight();
                
                texture->setAlphaMod((Uint8)(tileLayer->GetOpacity()*255.0f));
                
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
                
                texture->setAlphaMod(255);
                
                const Tmx::Tile* ttile = tileSet->GetTile(tileID);
                if(ttile && (ttile->GetNumObjects() > 0))
                    loadCollisionObjects(ttile,x*m_tmxMap->GetTileWidth(),y*m_tmxMap->GetTileHeight());
            }
        }
    }
    
    void TiledMap::loadObjectGroup(const Tmx::ObjectGroup* objectGroup)
    {
        if(!objectGroup->IsVisible())
            return;
        
        SDL_Rect dst,src;
        TextureHandle* texture;
        
        /*std::cout << "-- ObjectGroup --" << std::endl;
        std::cout << "+ ParseOrder: " << objectGroup->GetParseOrder() << std::endl;
        std::cout << "+ Color: " << objectGroup->GetColor() << std::endl;
        std::cout << "+ X: " << objectGroup->GetX() << std::endl;
        std::cout << "+ Y: " << objectGroup->GetY() << std::endl;
        std::cout << "+ Opacity:" << objectGroup->GetOpacity() << std::endl;
        std::cout << "+ ZOrder: " << objectGroup->GetZOrder() << std::endl;*/
        
        for(int j = 0;j<objectGroup->GetNumObjects();j++)
        {
            const Tmx::Object* obj = objectGroup->GetObject(j);
            if(!obj->IsVisible())
                continue;
            
            /*std::cout << "-- Object --" << std::endl;
            std::cout << "+ Position: " << obj->GetX() << "; " << obj->GetY() << std::endl;
            std::cout << "+ Rotation: " << obj->GetRot() << std::endl;
            std::cout << "+ Height: " << obj->GetHeight() << std::endl;
            std::cout << "+ Width: " << obj->GetWidth() << std::endl;
            std::cout << "+ Name: " << obj->GetName() << std::endl;
            std::cout << "+ Typ: " << obj->GetType() << std::endl;
            std::cout << "+ GID: " << obj->GetGid() << std::endl;*/
            if(obj->GetEllipse())
            {
                //std::cout << "+ Ellipse" << std::endl;
                const Tmx::Ellipse* ellipse = obj->GetEllipse();
                SDL_Color color = colorStringToColor(objectGroup->GetColor());
                /*std::cout << "++ Center: " << ellipse->GetCenterX() << "; " << ellipse->GetCenterY() << std::endl;
                std::cout << "++ Radius: " << ellipse->GetRadiusX() << "; " << ellipse->GetRadiusY() << std::endl;*/
                
                aaellipseRGBA(m_mainClass->getRenderer(),ellipse->GetCenterX(),ellipse->GetCenterY(),ellipse->GetRadiusX(),ellipse->GetRadiusY(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f));
                filledEllipseRGBA(m_mainClass->getRenderer(),ellipse->GetCenterX(),ellipse->GetCenterY(),ellipse->GetRadiusX(),ellipse->GetRadiusY(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f/4.0f));
                
                continue;
            }
            else if(obj->GetPolygon())
            {
                //std::cout << "+ Polygon" << std::endl;
                const Tmx::Polygon* polygon = obj->GetPolygon();
                SDL_Color color = colorStringToColor(objectGroup->GetColor());
                //std::cout << "++ Points" << std::endl;
                
                Sint16 vx[polygon->GetNumPoints()];
                Sint16 vy[polygon->GetNumPoints()];
                
                for(int k = 0;k<polygon->GetNumPoints();k++)
                {
                    const Tmx::Point& point = polygon->GetPoint(k);
                    //std::cout << "+++ " << k+1 << ". " << point.x << "; " << point.y << std::endl;
                    vx[k] = obj->GetX()+point.x;
                    vy[k] = obj->GetY()+point.y;
                    
                    //std::cout << "+++ vx: " << vx[k] << "; vy: " << vy[k] << std::endl;
                }
                
                for(int k = 0;k<polygon->GetNumPoints();k++)
                {
                    if(k!=0)
                    {
                        aalineRGBA(m_mainClass->getRenderer(),vx[k-1],vy[k-1],vx[k],vy[k],color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f));
                        if(k==polygon->GetNumPoints()-1)
                        {
                            aalineRGBA(m_mainClass->getRenderer(),vx[k],vy[k],vx[0],vy[0],color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f));
                        }
                    }
                }
                
                //aapolygonRGBA(m_mainClass->getRenderer(),vx,vy,polygon->GetNumPoints(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f));
                filledPolygonRGBA(m_mainClass->getRenderer(),vx,vy,polygon->GetNumPoints(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f/4.0f));
                
                continue;
            }
            else if(obj->GetPolyline())
            {
                //std::cout << "+ Polyline" << std::endl;
                const Tmx::Polyline* polyline = obj->GetPolyline();
                SDL_Color color = colorStringToColor(objectGroup->GetColor());
                //std::cout << "++ Points" << std::endl; 
                for(int k = 0;k<polyline->GetNumPoints();k++)
                {
                    if(k!=0)
                    {
                        const Tmx::Point& point1 = polyline->GetPoint(k-1);
                        const Tmx::Point& point2 = polyline->GetPoint(k);
                        //std::cout << "+++ " << k << ". " << point1.x << "; " << point1.y << std::endl;
                        
                        aalineRGBA(m_mainClass->getRenderer(),point1.x+obj->GetX(),point1.y+obj->GetY(),point2.x+obj->GetX(),point2.y+obj->GetY(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f));
                    }
                }
                
                continue;
            }
            
            int GID = obj->GetGid() < 0 ? 282 : obj->GetGid();
            
            const Tmx::Tileset* gidTileSet = getTileset(GID);
            if(gidTileSet)
            {
                //std::cout << "+ Tile" << std::endl;
                
                getSrcRectForTileGID(&src,GID);
                dst.x = obj->GetX();
                dst.y = obj->GetY()-obj->GetHeight();
                dst.w = obj->GetWidth();
                dst.h = obj->GetHeight();
                
                texture = m_mainClass->getResourceManager()->loadTexture(m_tmxMap->GetFilepath() + gidTileSet->GetImage()->GetSource());
                texture->setAlphaMod((Uint8)(objectGroup->GetOpacity()*255.0f));
                texture->renderCopy(m_mainClass->getRenderer(),&dst,&src);
                texture->setAlphaMod(255);
                
                int tileID = getTileIDForTileGID(GID);
                const Tmx::Tile* ttile = gidTileSet->GetTile(tileID);
                if(ttile && ttile->GetNumObjects() > 0)
                    loadCollisionObjects(ttile,obj->GetX(),obj->GetY()-obj->GetHeight());
                
                continue;
            }
            
            // Render Rectangle
            
            SDL_Color color = colorStringToColor(objectGroup->GetColor());
            
            rectangleRGBA(m_mainClass->getRenderer(),obj->GetX(),obj->GetY(),obj->GetX()+obj->GetWidth(),obj->GetY()+obj->GetHeight(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f));
            boxRGBA(m_mainClass->getRenderer(),obj->GetX(),obj->GetY(),obj->GetX()+obj->GetWidth(),obj->GetY()+obj->GetHeight(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f/4.0f));
        
        }
        
        SDL_SetRenderDrawColor(m_mainClass->getRenderer(),m_background.r,m_background.g,m_background.b,255);
    }
    
    void TiledMap::loadImageLayer(const Tmx::ImageLayer* imageLayer)
    {
        
    }
    
    void TiledMap::loadCollisionObjects(const Tmx::Tile* tile,int x,int y)
    {
        //std::cout << "+++++++++++++ Collision Objects: " << tile->GetNumObjects() << std::endl;
        
        if(!m_mainClass->getPhysics())
            m_mainClass->activatePhysics();
        
        for(int i = 0;i<tile->GetNumObjects();i++)
        {
            const Tmx::Object* obj = tile->GetObject(i);
            
            /*std::cout << "--------------- Object ---" << std::endl;
            std::cout << "+++++++++++++++ Position: " << obj->GetX() << "; " << obj->GetY() << std::endl;
            std::cout << "+++++++++++++++ Rotation: " << obj->GetRot() << std::endl;
            std::cout << "+++++++++++++++ Height: " << obj->GetHeight() << std::endl;
            std::cout << "+++++++++++++++ Width: " << obj->GetWidth() << std::endl;
            std::cout << "+++++++++++++++ Name: " << obj->GetName() << std::endl;
            std::cout << "+++++++++++++++ Typ: " << obj->GetType() << std::endl;
            std::cout << "+++++++++++++++ GID: " << obj->GetGid() << std::endl;
            std::cout << "+++++++++++++++ Visible: " << obj->IsVisible() << std::endl;*/
            
            if(obj->GetEllipse())
            {
                //std::cout << "+++++++++++++++ Ellipse" << std::endl;
                const Tmx::Ellipse* ellipse = obj->GetEllipse();
                SDL_Color color = {255,255,255,255};
                /*std::cout << "++++++++++++++++ Center: " << ellipse->GetCenterX() << "; " << ellipse->GetCenterY() << std::endl;
                std::cout << "++++++++++++++++ Radius: " << ellipse->GetRadiusX() << "; " << ellipse->GetRadiusY() << std::endl;*/
                
                if(false && obj->IsVisible())
                {
                    aaellipseRGBA(m_mainClass->getRenderer(),
                                  ellipse->GetCenterX()+x,
                                  ellipse->GetCenterY()+y,
                                  ellipse->GetRadiusX(),
                                  ellipse->GetRadiusY(),
                                  color.r,color.g,color.b,255);
                    filledEllipseRGBA(m_mainClass->getRenderer(),
                                      ellipse->GetCenterX()+x,
                                      ellipse->GetCenterY()+y,
                                      ellipse->GetRadiusX(),
                                      ellipse->GetRadiusY(),
                                      color.r,color.g,color.b,64);
                }
                
                loadCollisionEllipse(obj,x,y);
                
            }
            else if(obj->GetPolygon())
            {
                //std::cout << "+++++++++++++++ Polygon" << std::endl;
                const Tmx::Polygon* polygon = obj->GetPolygon();
                SDL_Color color = {255,255,255,255};
                //std::cout << "++++++++++++++++ Points" << std::endl;
                
                Sint16 vx[polygon->GetNumPoints()];
                Sint16 vy[polygon->GetNumPoints()];
                
                for(int k = 0;k<polygon->GetNumPoints();k++)
                {
                    const Tmx::Point& point = polygon->GetPoint(k);
                    //std::cout << "+++++++++++++++++ " << k+1 << ". " << point.x << "; " << point.y << std::endl;
                    vx[k] = obj->GetX()+point.x+x;
                    vy[k] = obj->GetY()+point.y+y;
                    
                    //std::cout << "+++++++++++++++++ vx: " << vx[k] << "; vy: " << vy[k] << std::endl;
                }
                
                if(false && obj->IsVisible())
                {
                    for(int k = 0;k<polygon->GetNumPoints();k++)
                    {
                        if(k!=0)
                        {
                            aalineRGBA(m_mainClass->getRenderer(),vx[k-1],vy[k-1],vx[k],vy[k],color.r,color.g,color.b,255);
                            if(k==polygon->GetNumPoints()-1)
                            {
                                aalineRGBA(m_mainClass->getRenderer(),vx[k],vy[k],vx[0],vy[0],color.r,color.g,color.b,255);
                            }
                        }
                    }
                    
                    //aapolygonRGBA(m_mainClass->getRenderer(),vx,vy,polygon->GetNumPoints(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f));
                    filledPolygonRGBA(m_mainClass->getRenderer(),
                                      vx,
                                      vy,
                                      polygon->GetNumPoints(),
                                      color.r,color.g,color.b,64);
                }
                
                loadCollisionPolygon(obj,x,y);
            }
            else if(obj->GetPolyline())
            {
                //std::cout << "+++++++++++++++ Polyline" << std::endl;
                const Tmx::Polyline* polyline = obj->GetPolyline();
                SDL_Color color = {255,255,255,255};
                //std::cout << "++++++++++++++++ Points" << std::endl; 
                for(int k = 0;k<polyline->GetNumPoints();k++)
                {
                    if(k!=0)
                    {
                        const Tmx::Point& point1 = polyline->GetPoint(k-1);
                        const Tmx::Point& point2 = polyline->GetPoint(k);
                        //std::cout << "+++++++++++++++++ " << k << ". " << point1.x << "; " << point1.y << std::endl;
                        
                        if(false && obj->IsVisible())
                            aalineRGBA(m_mainClass->getRenderer(),
                                       point1.x+obj->GetX()+x,
                                       point1.y+obj->GetY()+y,
                                       point2.x+obj->GetX()+x,
                                       point2.y+obj->GetY()+y,
                                       color.r,color.g,color.b,255);
                    }
                }
                
                loadCollisionPolyline(obj,x,y);
            }
            else
            {
                SDL_Color color = {255,255,255,255};
            
                //std::cout << "+++++++++++++++ Rectangle" << std::endl;
                
                if(false && obj->IsVisible())
                {
                    rectangleRGBA(m_mainClass->getRenderer(),
                                  obj->GetX()+x,
                                  obj->GetY()+y,
                                  obj->GetX()+obj->GetWidth()+x,
                                  obj->GetY()+obj->GetHeight()+y,
                                  color.r,color.g,color.b,255);
                    boxRGBA(m_mainClass->getRenderer(),
                            obj->GetX()+x,
                            obj->GetY()+y,
                            obj->GetX()+obj->GetWidth()+x,
                            obj->GetY()+obj->GetHeight()+y,
                            color.r,color.g,color.b,64);
                }
                
                loadCollisionRectangle(obj,x,y);
            }
            
            
        }
    }
    
    void TiledMap::loadCollisionEllipse(const Tmx::Object* obj,int x,int y)
    {
        const Tmx::Ellipse* ellipse;
        b2Body* body;
        b2BodyDef bdef;
        b2FixtureDef fdef[3];
        b2CircleShape cshape[2];
        //b2PolygonShape pshape; TODO
        
        ellipse = obj->GetEllipse();
        
        bdef.position = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2(x+ellipse->GetCenterX()+obj->GetX(),y+ellipse->GetCenterY()+obj->GetY()));
        bdef.type = b2_staticBody;
        
        fdef[0].density = 6.0f;
        fdef[0].friction = 0.0f;
        fdef[0].restitution = 0.0f;
        fdef[0].filter.categoryBits = 1;
        
        body = m_mainClass->getPhysics()->getWorld()->CreateBody(&bdef);
        
        if(ellipse->GetRadiusX() == ellipse->GetRadiusY())
        {
            cshape[0].m_radius = m_mainClass->getPhysics()->scalarPixelToWorld(ellipse->GetRadiusX());
            fdef[0].shape = &cshape[0];
            
            body->CreateFixture(&fdef[0]);
            
            m_bodies.push_back(body);
        }
        
    }
    
    void TiledMap::loadCollisionPolygon(const Tmx::Object* obj,int x,int y)
    {
        /*const Tmx::Polygon* polygon = obj->GetPolygon();
        b2Body* body;
        b2BodyDef bdef;
        b2FixtureDef fdef;
        b2PolygonShape shape;
        b2Vec2 v[polygon->GetNumPoints()];
        
        bdef.position = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2((double)x+(double)obj->GetX()+(double)obj->GetWidth()/2.0,(double)y+(double)obj->GetY()+(double)obj->GetHeight()/2.0));
        bdef.type = b2_staticBody;
        
        fdef.density = 6.0f;
        fdef.friction = 1.0f;
        fdef.restitution = 0.0f;
        
        for(int i = polygon->GetNumPoints()-1;i>=0;i--)
        {
            const Tmx::Point& point = polygon->GetPoint(i);
            double xb = x + obj->GetX() + point.x;
            double yb = y + obj->GetY() + point.y;
            
            v[polygon->GetNumPoints()-1-i] = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2(xb,yb));
        }
        
        shape.Set(v,polygon->GetNumPoints());
        
        fdef.shape = &shape;
        
        body = m_mainClass->getPhysics()->getWorld()->CreateBody(&bdef);
        body->CreateFixture(&fdef);
        
        m_bodies.push_back(body); TODO*/
    }
    
    void TiledMap::loadCollisionPolyline(const Tmx::Object* obj,int x,int y)
    {
        /*const Tmx::Polyline* polyline = obj->GetPolyline();
        b2Body* body;
        b2BodyDef bdef;
        b2FixtureDef fdef;
        b2ChainShape shape;
        b2Vec2 v[polyline->GetNumPoints()];
        
        bdef.position = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2((double)x+(double)obj->GetX()+(double)obj->GetWidth()/2.0,(double)y+(double)obj->GetY()+(double)obj->GetHeight()/2.0));
        bdef.type = b2_staticBody;
        
        fdef.density = 6.0f;
        fdef.friction = 1.0f;
        fdef.restitution = 0.0f;
        
        for(int i = polyline->GetNumPoints()-1;i>=0;i--)
        {
            const Tmx::Point& point = polyline->GetPoint(i);
            double xb = point.x+obj->GetX();
            double yb = point.y+obj->GetY();
            
            v[polyline->GetNumPoints()-1-i] = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2(xb,yb));
        }
        
        shape.CreateChain(v,polyline->GetNumPoints());
        
        fdef.shape = &shape;
        
        body = m_mainClass->getPhysics()->getWorld()->CreateBody(&bdef);
        body->CreateFixture(&fdef);
        
        m_bodies.push_back(body); TODO*/
    }
    
    void TiledMap::loadCollisionRectangle(const Tmx::Object* obj,int x,int y)
    {
        b2Body* body;
        b2BodyDef bdef;
        b2FixtureDef fdef;
        b2PolygonShape shape;
        
        bdef.type = b2_staticBody;
        bdef.position = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2((double)x+(double)obj->GetX()+(double)obj->GetWidth()/2.0,(double)y+(double)obj->GetY()+(double)obj->GetHeight()/2.0));
        
        //std::cout << "Create Rectangle at " << Vector2((double)x+(double)obj->GetX()+(double)obj->GetWidth()/2.0,(double)y+(double)obj->GetY()+(double)obj->GetHeight()/2.0) << std::endl;
        
        fdef.density = 6.0f;
        fdef.friction = 1.0f;
        fdef.restitution = 0.0f;
        fdef.filter.categoryBits = 1;
        
        shape.SetAsBox(m_mainClass->getPhysics()->scalarPixelToWorld((double)obj->GetWidth()/2.0),m_mainClass->getPhysics()->scalarPixelToWorld((double)obj->GetHeight()/2.0));
        fdef.shape = &shape;
        
        body = m_mainClass->getPhysics()->getWorld()->CreateBody(&bdef);
        body->CreateFixture(&fdef);
        
        m_bodies.push_back(body);
    }
    
    const Tmx::Tileset* TiledMap::getTileset(int gid)
    {
        if(gid == 0)
            return nullptr;
        
        int curGID = 0;
        
        for(int i = 0;i<m_tmxMap->GetNumTilesets();i++)
        {
            curGID = m_tmxMap->GetTileset(i)->GetFirstGid();
            
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
    
    int TiledMap::getTileIDForTileGID(int gid)
    {
        const Tmx::Tileset* tileSet = getTileset(gid);
        if(!tileSet)
            return -1;
        else
        {
            return gid - tileSet->GetFirstGid();
        }
    }
    
    bool TiledMap::update()
    {
        return true;
    }
    
    void TiledMap::quit()
    {
        delete m_tmxMap;
        m_tmxMap = nullptr;
        
        for(size_t i = 0;i<m_bodies.size();i++)
        {
            m_mainClass->getPhysics()->getWorld()->DestroyBody(m_bodies[i]);
        }
        m_bodies.clear();
    }
}