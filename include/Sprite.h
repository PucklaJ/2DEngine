#ifndef SPRITE_H
#define SPRITE_H
#include "Actor.h"
#include "TextureHandle.h"

namespace SDL
{
    namespace Textures
    {
        extern TextureHandle* BOX(MainClass*,const Vector2&,const SDL_Color&);
        extern TextureHandle* CIRCLE(MainClass*,double,const SDL_Color&,bool filled = true,const SDL_Color* back = nullptr);
    }
    
    
    class Sprite : public Actor
    {
    public:
            Sprite(int order = 0);
            Sprite(const char* file,int order = 0);
            Sprite(TextureHandle*,int order = 0);
            virtual ~Sprite();

            virtual bool init();
            virtual bool update();
            virtual bool m_update();
            virtual bool m_render();
            virtual bool render();
            virtual void quit();
            virtual void m_quit();

            void setRotation(const double& rot){m_rotation=rot;}
            void setScale(const float& s){m_size/=m_scale;m_scale = s;m_needsScaleUpdate = true;}
            void setTexture(TextureHandle* tex);
            void setSrcRect(const SDL_Rect& r){m_srcRect = r;}

            const double& getRotation() const {return m_rotation;}
            TextureHandle* getTexture() {return m_texture;}
            
            bool intersects(Sprite*);
            bool intersects(const Vector2&);
            
            bool isOnScreen() override;
        protected:
            TextureHandle* m_texture = nullptr;
            SDL_Rect m_srcRect;
            float m_scale = 1.0f;


            double m_rotation;

        private:
            const char* m_file = nullptr;
            bool m_needsScaleUpdate = false;
    };
}


#endif // SPRITE_H
