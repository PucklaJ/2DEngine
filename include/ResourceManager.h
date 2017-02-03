#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include <map>
#include <vector>
#ifdef __linux__
    #include <SDL2/SDL_ttf.h>
#else
#ifndef __APPLE__
    #include <TTF/SDL_ttf.h>
#else
    #include <SDL2_ttf/SDL_ttf.h>
#endif
#endif

#include <string>

using namespace std;

    class SDL_Texture;
    class SDL_Rect;
    class SDL_Renderer;

namespace SDL
{
    class TextureHandle;

    class ResourceManager
    {
        public:
            ResourceManager(SDL_Renderer* renderer);
            virtual ~ResourceManager();

            TextureHandle* loadTexture(const std::string& file);
            TTF_Font* loadFont(const std::string& file,const int size);
            
            void clearFonts();
            void deleteFont(TTF_Font*);
            void clearTextures();
            void deleteTexture(TextureHandle*);
            
            bool isLoaded(TextureHandle*);
            bool isLoaded(TTF_Font*);
            
            void clear();


        protected:

        private:
            map<std::string,TextureHandle*> m_textures;
            map<pair<std::string,int>,TTF_Font*> m_fonts;
            SDL_Renderer* m_renderer = nullptr;

    };
}



#endif // RESOURCEMANAGER_H
