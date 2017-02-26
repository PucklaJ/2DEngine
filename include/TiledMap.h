#ifndef TILED_MAP_H
#define TILED_MAP_H
#include <Sprite.h>
#include <TMX/Tmx.h>
#include <string>

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
        
        Tmx::Map* m_tmxMap = nullptr;
    };
}

#endif