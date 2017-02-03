#ifndef PHYSICS_SPRITE_H
#define PHYSICS_SPRITE_H
#include <Sprite.h>
#include <Box2D/Box2D.h>

namespace SDL
{
    class TextureHandle;
    
    class PhysicsSprite : public SDL::Sprite
    {
    public:
        PhysicsSprite(b2Body* body = nullptr,int order = 0);
        PhysicsSprite(const char* file,b2Body* body = nullptr,int order = 0);
        PhysicsSprite(TextureHandle*,b2Body* body = nullptr,int order = 0);
        virtual ~PhysicsSprite();
        
        virtual bool init();
        virtual bool m_update();
        virtual void m_quit();
        
        b2Body* getBody() {return m_body;}
        bool getAutomaticDestroy() {return m_automaticDestroy;}
        
        void setBody(b2Body* body) {m_body = body;}
        void setAutomaticDestroy(bool b) {m_automaticDestroy = b;}
    protected:
        bool m_automaticDestroy = true;
        
    private:
        b2Body* m_body = nullptr;
    };
}


#endif