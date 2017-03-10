
#include "Box2DDebugDrawer.h"
#include "MainClass.h"
#include "Physics.h"
#include <SDL2_gfxPrimitives.h>
#include <iostream>
#include "Camera.h"

namespace SDL
{
    Box2DDebugDrawer::Box2DDebugDrawer(MainClass* mc) :
        m_mainClass(mc)
    {
        
    }
    Box2DDebugDrawer::~Box2DDebugDrawer()
    {
        
    }
    
    void Box2DDebugDrawer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        std::cout << "Render Polygon" << std::endl;
        
        Sint16 vx[vertexCount],vy[vertexCount];
        
        for(int i = 0;i<vertexCount;i++)
        {
            Vector2 v = m_mainClass->getPhysics()->coordsWorldToPixel(vertices[i]);
            v = m_mainClass->getCamera()->getScreenPosition(v);
            vx[i] = v.getX();
            vy[i] = v.getY();
        }
        
        polygonRGBA(m_mainClass->getRenderer(),vx,vy,vertexCount,color.r*255.0f,color.g*255.0f,color.b*255.0f,color.a*255.0f);
    }
    void Box2DDebugDrawer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        std::cout << "Render SolidPolygon" << std::endl;
        
        Sint16 vx[vertexCount],vy[vertexCount];
        
        for(int i = 0;i<vertexCount;i++)
        {
            Vector2 v = m_mainClass->getPhysics()->coordsWorldToPixel(vertices[i]);
            v = m_mainClass->getCamera()->getScreenPosition(v);
            vx[i] = v.getX();
            vy[i] = v.getY();
        }
        
        filledPolygonRGBA(m_mainClass->getRenderer(),vx,vy,vertexCount,color.r*255.0f,color.g*255.0f,color.b*255.0f,color.a*255.0f);
    }
    void Box2DDebugDrawer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
    {
        
    }
    void Box2DDebugDrawer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
    {
        
    }
    void Box2DDebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
    {
        std::cout << "Render Segment" << std::endl;
        
        Vector2 v1 = m_mainClass->getPhysics()->coordsWorldToPixel(p1);
        v1 = m_mainClass->getCamera()->getScreenPosition(v1);
        Vector2 v2 = m_mainClass->getPhysics()->coordsWorldToPixel(p2);
        v2 = m_mainClass->getCamera()->getScreenPosition(v2);
        
        lineRGBA(m_mainClass->getRenderer(),v1.getX(),v1.getY(),v2.getX(),v2.getY(),color.r*255.0f,color.g*255.0f,color.b*255.0f,color.a*255.0f);
    }
    void Box2DDebugDrawer::DrawTransform(const b2Transform& xf)
    {
        
    }
    
    void Box2DDebugDrawer::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
    {
        
    }
}
