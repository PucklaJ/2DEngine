#include "Physics.h"
#include "MainClass.h"
#include "Box2DDebugDrawer.h"

namespace SDL
{
    Physics::Physics(MainClass* mainClass,const Vector2& gravity,const Vector2& worldSize)
    {
        m_world = new b2World(b2Vec2(gravity.getX(),gravity.getY()));
        m_mainClass = mainClass;
        m_worldSize = worldSize;
        m_world->SetDebugDraw(new Box2DDebugDrawer(m_mainClass));
    }

    Physics::~Physics()
    {

    }

    void Physics::update()
    {
        m_world->Step(m_mainClass->getDeltaTimeInSeconds(),6,2);
    }

    void Physics::quit()
    {
        delete m_world;
    }
    
    b2Vec2 Physics::coordsPixelToWorld(const Vector2& pos)
    {
        b2Vec2 worldPos;
        
        worldPos.x = pos.getX();
        worldPos.y = pos.getY();
        
        worldPos.x -= m_worldSize.getX()/2.0;
        worldPos.y -= m_worldSize.getY()/2.0;
        
        worldPos.y *= -1.0;
        
        worldPos.x /= m_pixelsPerMetre;
        worldPos.y /= m_pixelsPerMetre;
        
        return worldPos;
    }
    
    Vector2 Physics::coordsWorldToPixel(const b2Vec2& pos)
    {
        Vector2 pixelPos(pos.x,pos.y);
        
        pixelPos *= m_pixelsPerMetre;
        
        pixelPos.setY(pixelPos.getY()*-1.0);
        
        pixelPos += m_worldSize/2.0;
        
        return pixelPos;
    }
    
    Vector2 Physics::toVector2(const b2Vec2& vec)
    {
        return Vector2(vec.x,vec.y);
    }
    
    double Physics::scalarPixelToWorld(double d)
    {
        return d/m_pixelsPerMetre;
    }
    
    double Physics::scalarWorldToPixel(double d)
    {
        return d * m_pixelsPerMetre;
    }
}
