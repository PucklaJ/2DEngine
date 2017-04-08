#ifndef PHYSICS_H
#define PHYSICS_H
#include <Box2D/Box2D.h>
#include "mathematics.h"
#include "Actor.h"

#define DEF_FRICTION 1.0f
#define DEF_RESTITUTION 0.0f
#define DEF_DENSITY 1.0f

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
            
            b2Vec2 vectorPixelToWorld(const Vector2&);
            Vector2 vectorWorldToPixel(const b2Vec2&);

            double scalarPixelToWorld(double);
            double scalarWorldToPixel(double);
            
            static Vector2 toVector2(const b2Vec2&);
            
            void setDebug(bool b){m_debug = b;}
            bool getDebug() {return m_debug;}
            
            // Creator methods
            
            b2Body* createBOX(const Vector2&,double,double,float32 friction = DEF_FRICTION,float32 restitution = DEF_RESTITUTION,float32 density = DEF_DENSITY,b2BodyType type = b2_dynamicBody);
            b2Body* createBOX(const Vector2&,double,double,b2BodyDef&,b2FixtureDef&);
            b2Body* createCIRCLE(const Vector2&,double,float32 friction = DEF_FRICTION,float32 restitution = DEF_RESTITUTION,float32 density = DEF_DENSITY,b2BodyType type = b2_dynamicBody);
            b2Body* createCIRCLE(const Vector2&,double,b2BodyDef&,b2FixtureDef&);
            b2Body* createCAPSULE(const Vector2&,double,double,float32 friction = DEF_FRICTION,float32 restitution = DEF_RESTITUTION,float32 density = DEF_DENSITY,b2BodyType type = b2_dynamicBody);
            b2Body* createCAPSULE(const Vector2&,double,double,b2BodyDef&,b2FixtureDef&);
            
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
