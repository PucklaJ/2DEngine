#ifndef TILED_MAP_H
#define TILED_MAP_H
#include <Sprite.h>
#include <TMX/Tmx.h>
#include <string>
#include <vector>
#include "Physics.h"
#include <map>

namespace SDL
{
	struct AnimationTile
	{
		int x,y;
		int animID;
		int layer;
	};

	struct TileAnimation
	{
		std::vector<SDL_Rect> rects;
		std::vector<int> times;
		TextureHandle* texture;
		int curRect;
		int curTime;
	};

    class TiledMap : public Sprite, public Tmx::Map
    {
    public:
        TiledMap(const std::string& file);
        ~TiledMap();
        
        bool init() override;
        bool update() override;
        bool render() override;
        void quit() override;
        
        std::vector<b2Body*>* getBodies() {return &m_bodies;}
        
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
        
        void loadTileLayer(const Tmx::TileLayer*,int);
        void loadObjectGroup(const Tmx::ObjectGroup*);
        void loadImageLayer(const Tmx::ImageLayer*);
        
        void loadCollisionObjects(const Tmx::Tile*,int,int);
        void loadCollisionEllipse(const Tmx::Object*,int,int);
        void loadCollisionPolygon(const Tmx::Object*,int,int);
        void loadCollisionPolyline(const Tmx::Object*,int,int);
        void loadCollisionRectangle(const Tmx::Object*,int,int);
        
        void loadAnimations(const Tmx::TileLayer*,int);
        void updateAnimations();
        void renderAnimations();

        void addTexture();
        void renderLayers();

        bool m_works = true;
        int* m_tilesX = nullptr;
        SDL_Color m_background;
        std::vector<b2Body*> m_bodies;
        std::map<int,TileAnimation> m_animations;
        std::vector<AnimationTile> m_animTiles;
        std::vector<TextureHandle*> m_textures;
        
    };
}

#endif
