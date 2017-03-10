#ifndef TILED_MAP_H
#define TILED_MAP_H
#include <Sprite.h>
#include <TMX/Tmx.h>
#include <string>
#include <vector>
#include "Physics.h"

namespace SDL
{
    class TiledMap : public Sprite
    {
    public:
        TiledMap(const std::string& file);
        ~TiledMap();
        
        bool init() override;
        bool update() override;
        void quit() override;
        
        Tmx::Map* getTmxMap() {return m_tmxMap;}
        
    private:
        int getIDofTileset(const Tmx::Tileset*);
        const Tmx::Tileset* getTileset(int);
        void getSrcRectForTileGID(SDL_Rect*,int);
        void getSrcRectForTileID(SDL_Rect*,int,int,int);
        int getTileIDForTileGID(int);
        SDL_Color colorStringToColor(const std::string&);
        
        void loadTexture();
        void loadTilesets();
        void loadLayers();
        
        void loadTileLayer(const Tmx::TileLayer*);
        void loadObjectGroup(const Tmx::ObjectGroup*);
        void loadImageLayer(const Tmx::ImageLayer*);
        
        void loadCollisionObjects(const Tmx::Tile*,int,int);
        void loadCollisionEllipse(const Tmx::Object*,int,int);
        void loadCollisionPolygon(const Tmx::Object*,int,int);
        void loadCollisionPolyline(const Tmx::Object*,int,int);
        void loadCollisionRectangle(const Tmx::Object*,int,int);
        
        Tmx::Map* m_tmxMap = nullptr;
        int* m_tilesX = nullptr;
        SDL_Color m_background;
        std::vector<b2Body*> m_bodies;
        
    };
}

#endif