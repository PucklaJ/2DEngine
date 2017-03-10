#ifndef PHYSICS_H
#define PHYSICS_H
#include <Box2D/Box2D.h>
#include "mathematics.h"
#include "Actor.h"

namespace SDL
{
    class MainClass;
    
    class Physics
    {
        public:
            Physics(MainClass*,const Vector2& gravity = Vector2(0.0,-10.0),const Vector2& worldSize = Vector2(NORM_W,NORM_H));
            virtual ~Physics();

            void update();
            void quit();
            
            void setPixelPerMetre(double d) {m_pixelsPerMetre = d;}
            b2World* getWorld() {return m_world;}
            const Vector2& getWorldSize() const {return m_worldSize;}
            
            b2Vec2 coordsPixelToWorld(const Vector2&);
            Vector2 coordsWorldToPixel(const b2Vec2&);
            
            double scalarPixelToWorld(double);
            double scalarWorldToPixel(double);
            
            static Vector2 toVector2(const b2Vec2&);
            
            void setDebug(bool b){m_debug = b;}
            bool getDebug() {return m_debug;}
            
        protected:

        private:
            b2World* m_world;
            MainClass* m_mainClass;
            Vector2 m_worldSize;
            double m_pixelsPerMetre = 10.0;
            bool m_debug = false;
    };
}


#endif // PHYSICS_H
