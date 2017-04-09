#include <TiledMap.h>
#include <LogManager.h>
#include <operators.h>
#include <MainClass.h>
#include <ResourceManager.h>
#include <iostream>
#include <mathematics.h>
#include <Box2D/Box2D.h>
#include <SDL2_gfxPrimitives.h>
//#define DRAW_DEBUG

#define GETGID(obj) (obj->GetGid() < 0 ? obj->GetGid()+INT_MAX : obj->GetGid())

namespace SDL
{
    TiledMap::TiledMap(const std::string& file) : Sprite(0),Tmx::Map()
    {
        ParseFile(file);
        
        if(HasError())
        {
            LogManager::log(std::string("TiledMap Error (") + GetErrorCode() + ") : " + GetErrorText());
            m_works = false;
        }

        m_renderChildrenAfter = true;
    }
    
    TiledMap::~TiledMap()
    {
        
    }
    
    bool TiledMap::init()
    {
        colorStringToColor("#ffaadd");
        
        if(m_works)
        {
            LogManager::log("Loading Map...");
            
            if(GetOrientation() != Tmx::MapOrientation::TMX_MO_ORTHOGONAL)
            {
                LogManager::log(std::string("This Orientation is not supported (") + GetOrientation() + ")");
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
        m_background = colorStringToColor(GetBackgroundColor());
        
        m_texture = new TextureHandle(m_mainClass->getRenderer(),m_mainClass->getWindow()->getPixelFormat(),SDL_TEXTUREACCESS_TARGET,GetWidth() * GetTileWidth(),GetHeight() * GetTileHeight());
        m_texture->setBlendMode(SDL_BLENDMODE_BLEND);
        m_texture->setRenderTarget(m_mainClass->getRenderer());
        SDL_SetRenderDrawBlendMode(m_mainClass->getRenderer(),SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(m_mainClass->getRenderer(),m_background.r,m_background.g,m_background.b,m_background.a);
        SDL_RenderClear(m_mainClass->getRenderer());
    }
    
    void TiledMap::loadTilesets()
    {
        LogManager::log("Loading Tilesets...");
        
        m_tilesX = new int[GetNumTilesets()];
            
        for(int i = 0;i<GetNumTilesets();i++)
        {
            const Tmx::Tileset* tileSet = GetTileset((int)i);
            std::string imageFile = GetFilepath() + GetTileset(i)->GetImage()->GetSource();
            
            m_tilesX[i] = (tileSet->GetImage()->GetWidth()-tileSet->GetMargin()*2+tileSet->GetSpacing())/(tileSet->GetTileWidth()+tileSet->GetSpacing());
        }
    }
    
    void TiledMap::addTexture()
    {
    	SDL_Color nothing = {0,0,0,0};
    	TextureHandle* texture = Textures::BOX(m_mainClass,Vector2(GetWidth()*GetTileWidth(),GetHeight()*GetTileHeight()),nothing);
    	m_textures.push_back(texture);
    }

    void TiledMap::loadLayers()
    {
        LogManager::log("Loading Layers..");
            
        for(int i = 0;i<GetNumLayers();i++)
        {
        	addTexture();

            switch(GetLayer(i)->GetLayerType())
            {
                case Tmx::LayerType::TMX_LAYERTYPE_TILE:
                    loadTileLayer(static_cast<const Tmx::TileLayer*>(GetLayer(i)),i);
                    break;
                case Tmx::LayerType::TMX_LAYERTYPE_OBJECTGROUP:
                    loadObjectGroup(static_cast<const Tmx::ObjectGroup*>(GetLayer(i)));
                    break;
                case Tmx::LayerType::TMX_LAYERTYPE_IMAGE_LAYER:
                    loadImageLayer(static_cast<const Tmx::ImageLayer*>(GetLayer(i)));
                    break;
            }
        }

        SDL_SetRenderTarget(m_mainClass->getRenderer(),m_mainClass->getBackBuffer());
    }
    
    void TiledMap::loadTileLayer(const Tmx::TileLayer* tileLayer,int Lindex)
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
        
        m_textures.back()->setRenderTarget(m_mainClass->getRenderer());

        for(int y = 0;y < tileLayer->GetHeight();y++)
        {
            for(int x = 0;x < tileLayer->GetWidth();x++)
            {
                if(tileLayer->GetTileTilesetIndex(x,y) == -1)
                    continue;
                
                
                index = tileLayer->GetTileTilesetIndex(x,y);
                tileID = tileLayer->GetTileId(x,y);
                
                const Tmx::Tileset* tileSet = GetTileset(index);
                imageFile = GetFilepath() + tileSet->GetImage()->GetSource();
                const Tmx::MapTile& tile = tileLayer->GetTile(x, y);
                
                texture = m_mainClass->getResourceManager()->loadTexture(imageFile);
                
                getSrcRectForTileID(&src,m_tilesX[index],index,tileID);
                
                dst.x = x*GetTileWidth();
                dst.y = y*GetTileHeight();
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
                    loadCollisionObjects(ttile,x*GetTileWidth(),y*GetTileHeight());
            }
        }

        loadAnimations(tileLayer,Lindex);
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
        
        m_textures.back()->setRenderTarget(m_mainClass->getRenderer());

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
            if(obj->GetEllipse() && m_renderObjects)
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
            else if(obj->GetPolygon() && m_renderObjects)
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
            else if(obj->GetPolyline() && m_renderObjects)
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
                
                texture = m_mainClass->getResourceManager()->loadTexture(GetFilepath() + gidTileSet->GetImage()->GetSource());
                texture->setAlphaMod((Uint8)(objectGroup->GetOpacity()*255.0f));
                texture->renderCopy(m_mainClass->getRenderer(),&dst,&src);
                texture->setAlphaMod(255);
                
                int tileID = getTileIDForTileGID(GID);
                const Tmx::Tile* ttile = gidTileSet->GetTile(tileID);
                if(ttile && ttile->GetNumObjects() > 0)
                    loadCollisionObjects(ttile,obj->GetX(),obj->GetY()-obj->GetHeight());
                
                continue;
            }
            
            if(!m_renderObjects)
            	continue;

            // Render Rectangle
            
            SDL_Color color = colorStringToColor(objectGroup->GetColor());
            
            rectangleRGBA(m_mainClass->getRenderer(),obj->GetX(),obj->GetY(),obj->GetX()+obj->GetWidth(),obj->GetY()+obj->GetHeight(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f));
            boxRGBA(m_mainClass->getRenderer(),obj->GetX(),obj->GetY(),obj->GetX()+obj->GetWidth(),obj->GetY()+obj->GetHeight(),color.r,color.g,color.b,(Uint8)(objectGroup->GetOpacity()*255.0f/4.0f));
        
        }
        
        SDL_SetRenderDrawColor(m_mainClass->getRenderer(),m_background.r,m_background.g,m_background.b,255);
    }
    
    void TiledMap::loadImageLayer(const Tmx::ImageLayer* imageLayer)
    {
        m_textures.back()->setRenderTarget(m_mainClass->getRenderer());
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
#ifdef DRAW_DEBUG
                //std::cout << "+++++++++++++++ Ellipse" << std::endl;
                const Tmx::Ellipse* ellipse = obj->GetEllipse();
                SDL_Color color = {255,255,255,255};
                /*std::cout << "++++++++++++++++ Center: " << ellipse->GetCenterX() << "; " << ellipse->GetCenterY() << std::endl;
                std::cout << "++++++++++++++++ Radius: " << ellipse->GetRadiusX() << "; " << ellipse->GetRadiusY() << std::endl;*/
                

                if(obj->IsVisible())
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
#endif
                
                loadCollisionEllipse(obj,x,y);
                
            }
            else if(obj->GetPolygon())
            {
                //std::cout << "+++++++++++++++ Polygon" << std::endl;
                const Tmx::Polygon* polygon = obj->GetPolygon();
#ifdef DRAW_DEBUG
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
                if(obj->IsVisible())
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
#endif
                
                loadCollisionPolygon(obj,x,y);
            }
            else if(obj->GetPolyline())
            {
#ifdef DRAW_DEBUG//std::cout << "+++++++++++++++ Polyline" << std::endl;
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

                        if(obj->IsVisible())
                            aalineRGBA(m_mainClass->getRenderer(),
                                       point1.x+obj->GetX()+x,
                                       point1.y+obj->GetY()+y,
                                       point2.x+obj->GetX()+x,
                                       point2.y+obj->GetY()+y,
                                       color.r,color.g,color.b,255);

                    }
                }
#endif
                loadCollisionPolyline(obj,x,y);
            }
            else
            {
#ifdef DRAW_DEBUG
                SDL_Color color = {255,255,255,255};
            
                //std::cout << "+++++++++++++++ Rectangle" << std::endl;

                if(obj->IsVisible())
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
#endif
                
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
        
        const Tmx::PropertySet& props = obj->GetProperties();

        bdef.position = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2(x+ellipse->GetCenterX()+obj->GetX(),y+ellipse->GetCenterY()+obj->GetY()));
		std::string type = props.GetStringProperty("Type","static");
		bdef.type = (type == "static" ? b2_staticBody : (type == "dynamic" ? b2_dynamicBody : b2_kinematicBody));

		fdef[0].density = props.GetFloatProperty("Density",DEF_DENSITY);
		fdef[0].friction = props.GetFloatProperty("Friction",DEF_FRICTION);
		fdef[0].restitution = props.GetFloatProperty("Restitution",DEF_RESTITUTION);
		fdef[0].filter.categoryBits = props.GetIntProperty("CategoryBits",1);
		fdef[0].filter.maskBits = props.GetIntProperty("MaskBits",~0);
		fdef[0].isSensor = props.GetBoolProperty("isSensor",false);
        
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
        const Tmx::Polygon* polygon = obj->GetPolygon();

        if(polygon->GetNumPoints() < 3 || polygon->GetNumPoints() > b2_maxPolygonVertices)
        {
        	LogManager::log(std::string("Polygon (") + x + "; " + y + ") has too much or too few vertices");
        	return;
        }

        const Tmx::PropertySet& props = obj->GetProperties();

        b2Body* body;
        b2BodyDef bdef;
        b2FixtureDef fdef;
        b2PolygonShape shape;
        b2Vec2* v;
        
        v = new b2Vec2[polygon->GetNumPoints()];
        
        Vector2 pixelPos((double)x+(double)GetTileWidth()/2.0,(double)y+(double)GetTileHeight()/2.0);

        bdef.position = m_mainClass->getPhysics()->coordsPixelToWorld(pixelPos);
        std::string type = props.GetStringProperty("Type","static");
        bdef.type = (type == "static" ? b2_staticBody : (type == "dynamic" ? b2_dynamicBody : b2_kinematicBody));
        
        fdef.density = props.GetFloatProperty("Density",DEF_DENSITY);
        fdef.friction = props.GetFloatProperty("Friction",DEF_FRICTION);
        fdef.restitution = props.GetFloatProperty("Restitution",DEF_RESTITUTION);
        fdef.filter.categoryBits = props.GetIntProperty("CategoryBits",1);
        fdef.filter.maskBits = props.GetIntProperty("MaskBits",~0);
        fdef.isSensor = props.GetBoolProperty("isSensor",false);



        for(int i = polygon->GetNumPoints()-1;i>=0;i--)
        {
            const Tmx::Point& point = polygon->GetPoint(i);
            double xb = (double)obj->GetX() + (double)point.x - (double)GetTileWidth()/2.0;
            double yb = (double)obj->GetY() + (double)point.y - (double)GetTileHeight()/2.0;
            
            v[polygon->GetNumPoints()-1-i] = m_mainClass->getPhysics()->vectorPixelToWorld(Vector2(xb,yb));
         }
        
        shape.Set(v,polygon->GetNumPoints());
        
        delete[] v;
        
        fdef.shape = &shape;
        
        body = m_mainClass->getPhysics()->getWorld()->CreateBody(&bdef);
        body->CreateFixture(&fdef);
        
        m_bodies.push_back(body);
    }
    
    void TiledMap::loadCollisionPolyline(const Tmx::Object* obj,int x,int y)
    {
        const Tmx::Polyline* polyline = obj->GetPolyline();

        if(polyline->GetNumPoints() < 2 || polyline->GetNumPoints() > b2_maxPolygonVertices)
		{
			LogManager::log(std::string("Polyline (") + x + "; " + y + ") has too much or too few vertices");
			return;
		}

        b2Body* body;
        b2BodyDef bdef;
        b2FixtureDef fdef;
        b2ChainShape shape;
        b2Vec2* v;
        
        v = new b2Vec2[polyline->GetNumPoints()];

        const Tmx::PropertySet& props = obj->GetProperties();

		std::string type = props.GetStringProperty("Type","static");
		bdef.type = (type == "static" ? b2_staticBody : (type == "dynamic" ? b2_dynamicBody : b2_kinematicBody));

		fdef.density = props.GetFloatProperty("Density",DEF_DENSITY);
		fdef.friction = props.GetFloatProperty("Friction",DEF_FRICTION);
		fdef.restitution = props.GetFloatProperty("Restitution",DEF_RESTITUTION);
		fdef.filter.categoryBits = props.GetIntProperty("CategoryBits",1);
		fdef.filter.maskBits = props.GetIntProperty("MaskBits",~0);
		fdef.isSensor = props.GetBoolProperty("isSensor",false);
        
        for(int i = polyline->GetNumPoints()-1;i>=0;i--)
        {
            const Tmx::Point& point = polyline->GetPoint(i);
            double xb = x + point.x + obj->GetX();
            double yb = y + point.y+ obj->GetY();

            v[polyline->GetNumPoints()-1-i] = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2(xb,yb));
        }
        
        shape.CreateChain(v,polyline->GetNumPoints());
        delete[] v;
        
        fdef.shape = &shape;
        
        body = m_mainClass->getPhysics()->getWorld()->CreateBody(&bdef);
        body->CreateFixture(&fdef);
        
        m_bodies.push_back(body);
    }
    
    void TiledMap::loadCollisionRectangle(const Tmx::Object* obj,int x,int y)
    {
        b2Body* body;
        b2BodyDef bdef;
        b2FixtureDef fdef;
        b2PolygonShape shape;
        
        const Tmx::PropertySet& props = obj->GetProperties();

        bdef.position = m_mainClass->getPhysics()->coordsPixelToWorld(Vector2((double)x+(double)obj->GetX()+(double)obj->GetWidth()/2.0,(double)y+(double)obj->GetY()+(double)obj->GetHeight()/2.0));
		std::string type = props.GetStringProperty("Type","static");
		bdef.type = (type == "static" ? b2_staticBody : (type == "dynamic" ? b2_dynamicBody : b2_kinematicBody));

		fdef.density = props.GetFloatProperty("Density",DEF_DENSITY);
		fdef.friction = props.GetFloatProperty("Friction",DEF_FRICTION);
		fdef.restitution = props.GetFloatProperty("Restitution",DEF_RESTITUTION);
		fdef.filter.categoryBits = props.GetIntProperty("CategoryBits",1);
		fdef.filter.maskBits = props.GetIntProperty("MaskBits",~0);
		fdef.isSensor = props.GetBoolProperty("isSensor",false);
        
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
        
        for(int i = 0;i<GetNumTilesets();i++)
        {
            curGID = GetTileset(i)->GetFirstGid();
            
            if(curGID == gid)
            {
                return GetTileset(i);
            }
            else if(i != 0 && curGID > gid)
            {
                return GetTileset(i-1);
            }
            else if(i + 1 == GetNumTilesets())
            {
                return GetTileset(i);
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
        
        const Tmx::Tileset* tileSet = GetTileset(index);
        
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
    
    void TiledMap::loadAnimations(const Tmx::TileLayer* layer,int Lindex)
    {
    	LogManager::log("Loading Animations...");

    	for(int x = 0;x<layer->GetWidth();x++)
    	{
    		for(int y = 0;y<layer->GetHeight();y++)
    		{
    			int index = layer->GetTileTilesetIndex(x,y);

    			if(index == -1)
    				continue;

    			const Tmx::MapTile& mapTile = layer->GetTile(x,y);
    			const Tmx::Tileset* tileset = GetTileset(index);
    			const Tmx::Tile* tile = tileset->GetTile(mapTile.id);

    			if(!tile || tile->GetFrameCount() == 0)
    				continue;

    			if(m_animations.count(tile->GetId()) == 0)
    			{
    				const std::vector<Tmx::AnimationFrame>& anims = tile->GetFrames();
					TextureHandle* texture = m_mainClass->getResourceManager()->loadTexture(GetFilepath() + tileset->GetImage()->GetSource());
					TileAnimation tileAnim;
					tileAnim.texture = texture;
					tileAnim.curRect = 0;
					tileAnim.curTime = 0;

					for(size_t i = 0;i<anims.size();i++)
					{
						SDL_Rect rect;
						getSrcRectForTileID(&rect,m_tilesX[index],index,anims[i].GetTileID());
						tileAnim.rects.push_back(rect);
						tileAnim.times.push_back(anims[i].GetDuration());
					}

					m_animations.insert(std::pair<int,TileAnimation>(tile->GetId(),tileAnim));
    			}

    			AnimationTile animTile;
    			animTile.x = x*GetTileWidth();
    			animTile.y = y*GetTileHeight();
    			animTile.layer = Lindex;
    			animTile.animID = tile->GetId();

    			m_animTiles.push_back(animTile);
    		}
    	}
    }

    void TiledMap::updateAnimations()
    {
    	int dMil = (int)(m_mainClass->getDeltaTimeInSeconds()*1000.0);

    	for(std::map<int,TileAnimation>::iterator it = m_animations.begin();it!=m_animations.end();it++)
    	{
    		it->second.curTime += dMil;
    		if(it->second.times[it->second.curRect] <= it->second.curTime)
    		{
    			it->second.curTime = 0;
    			it->second.curRect++;
    			if(it->second.curRect == it->second.rects.size())
    				it->second.curRect = 0;
    		}
    	}
    }

    void TiledMap::renderAnimations()
    {
    	TileAnimation anim;
    	SDL_Rect dstRect = {0,0,GetTileWidth(),GetTileHeight()};

    	SDL_SetRenderDrawColor(m_mainClass->getRenderer(),m_background.r,m_background.g,m_background.b,m_background.a);
    	for(size_t i = 0;i<m_animTiles.size();i++)
    	{
    		m_textures[m_animTiles[i].layer]->setRenderTarget(m_mainClass->getRenderer());
    		anim = m_animations[m_animTiles[i].animID];
    		dstRect.x = m_animTiles[i].x;
    		dstRect.y = m_animTiles[i].y;
    		SDL_RenderFillRect(m_mainClass->getRenderer(),&dstRect);
    		anim.texture->renderCopy(m_mainClass->getRenderer(),&dstRect,&anim.rects[anim.curRect]);
    	}

    	SDL_SetRenderTarget(m_mainClass->getRenderer(),m_mainClass->getBackBuffer());
    }

    bool TiledMap::update()
    {
    	updateAnimations();

        return true;
    }
    
    void TiledMap::renderLayers()
    {
    	m_texture->setRenderTarget(m_mainClass->getRenderer());
    	SDL_SetRenderDrawColor(m_mainClass->getRenderer(),m_background.r,m_background.g,m_background.b,m_background.a);
    	SDL_RenderClear(m_mainClass->getRenderer());

    	for(size_t i = 0;i<m_textures.size();i++)
    	{
    		m_textures[i]->renderCopy(m_mainClass->getRenderer());
    	}

    	SDL_SetRenderTarget(m_mainClass->getRenderer(),m_mainClass->getBackBuffer());
    }

    bool TiledMap::render()
    {
    	Sprite::render();

    	renderAnimations();

    	renderLayers();

    	return true;
    }

    void TiledMap::quit()
    {
        for(size_t i = 0;i<m_bodies.size();i++)
        {
            m_mainClass->getPhysics()->getWorld()->DestroyBody(m_bodies[i]);
        }
        m_bodies.clear();
    }

    b2Body* TiledMap::toCollisionBody(const Tmx::Object* obj)
    {
    	if(obj->GetEllipse())
    	{
    		loadCollisionEllipse(obj,0,0);
    	}
    	else if(obj->GetPolygon())
    	{
    		loadCollisionPolygon(obj,0,0);
    	}
    	else if(obj->GetPolyline())
    	{
    		loadCollisionPolyline(obj,0,0);
    	}
    	else
    	{
    		loadCollisionRectangle(obj,0,0);
    	}

    	return m_bodies.back();
    }

    void TiledMap::destroyTile(int x,int y,int layer)
    {
    	TextureHandle* tex = m_textures[layer];

    	SDL_SetRenderDrawColor(m_mainClass->getRenderer(),0,0,0,0);
    	SDL_SetRenderDrawBlendMode(m_mainClass->getRenderer(),SDL_BLENDMODE_NONE);
    	tex->setRenderTarget(m_mainClass->getRenderer());

    	SDL_Rect rect = {x*GetTileWidth(),y*GetTileHeight(),GetTileWidth(),GetTileHeight()};

    	SDL_RenderFillRect(m_mainClass->getRenderer(),&rect);
    	SDL_SetRenderDrawBlendMode(m_mainClass->getRenderer(),SDL_BLENDMODE_BLEND);
    	SDL_SetRenderTarget(m_mainClass->getRenderer(),m_mainClass->getBackBuffer());

    	for(size_t i = 0;i<m_animTiles.size();i++)
    	{
    		if(m_animTiles[i].x == x*GetTileWidth() && m_animTiles[i].y == y*GetTileHeight() && m_animTiles[i].layer == layer)
    		{
    			m_animTiles[i] = m_animTiles.back();
    			m_animTiles.pop_back();
    			return;
    		}
    	}
    }
}
