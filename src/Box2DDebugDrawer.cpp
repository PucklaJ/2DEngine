#include "Box2DDebugDrawer.h"
#include "MainClass.h"
#include "Physics.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <iostream>
#include "Camera.h"

namespace SDL
{
    Box2DDebugDrawer::Box2DDebugDrawer() : Actor(B2DEBUG_ORDER)
    {
        setName("Box2DDebugDrawer");
    }
    Box2DDebugDrawer::~Box2DDebugDrawer()
    {
        
    }
    
    void Box2DDebugDrawer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        DebugPolygon dp;
        dp.vertices = (b2Vec2*)vertices;
        dp.vertexCount = vertexCount;
        dp.color = color;
        dp.solid = false;
        
        m_debugPolygons.push_back(dp);
    }
    void Box2DDebugDrawer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        DebugPolygon dp;
        dp.vertices = (b2Vec2*)vertices;
        dp.vertexCount = vertexCount;
        dp.color = color;
        dp.solid = true;
        
        m_debugPolygons.push_back(dp);
    }
    void Box2DDebugDrawer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
    {
        DebugCircle dc;
        dc.center = center;
        dc.radius = radius;
        dc.color = color;
        dc.solid = false;
        
        m_debugCircles.push_back(dc);
    }
    void Box2DDebugDrawer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
    {
        DebugCircle dc;
        dc.center = center;
        dc.radius = radius;
        dc.color = color;
        dc.solid = true;
        
        m_debugCircles.push_back(dc);
    }
    void Box2DDebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
    {
        DebugSegment ds;
        ds.p1 = p1;
        ds.p2 = p2;
        ds.color = color;
        
        m_debugSegments.push_back(ds);
    }
    void Box2DDebugDrawer::DrawTransform(const b2Transform& xf)
    {
        DebugTransform dt;
        dt.xf = xf;
        
        m_debugTransforms.push_back(dt);
    }
    
    void Box2DDebugDrawer::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
    {
        DebugPoint dp;
        dp.p = p;
        dp.size = size;
        dp.color = color;
        
        m_debugPoints.push_back(dp);
    }
    
    bool Box2DDebugDrawer::render()
    {
        for(size_t i = 0;i<m_debugPolygons.size();i++)
        {
            //if(!m_debugPolygons[i].solid)
            {
                for(int32 j = 0;j<m_debugPolygons[i].vertexCount-1;j++)
                {
                    Physics* phys = m_mainClass->getPhysics();
                    
                    Vector2 pos1 = phys->coordsWorldToPixel(m_debugPolygons[i].vertices[j]);
                    Vector2 pos2 = phys->coordsWorldToPixel(m_debugPolygons[i].vertices[j+1]);
                    
                    m_mainClass->getCamera()->scalePos(pos1);
                    m_mainClass->getCamera()->scalePos(pos2);
                    
                    b2Color color = m_debugPolygons[i].color;
                    
                    lineRGBA(m_mainClass->getRenderer(),pos1.getX(),pos1.getY(),pos2.getX(),pos2.getY(),color.r*255.0,color.g*255.0,color.b*255.0,color.a*255.0);
                }
            }
           
        }
        
        for(size_t i = 0;i<m_debugCircles.size();i++)
        {
            b2Color color = m_debugCircles[i].color;
            Vector2 pos = m_mainClass->getPhysics()->coordsWorldToPixel(m_debugCircles[i].center);
            double rad = m_mainClass->getPhysics()->scalarWorldToPixel(m_debugCircles[i].radius) * m_mainClass->getCamera()->getScale();
            
            m_mainClass->getCamera()->scalePos(pos);
            
            if(m_debugCircles[i].solid)
            {
                filledCircleRGBA(m_mainClass->getRenderer(),pos.getX(),pos.getY(),rad,color.r*255.0,color.g*255.0,color.b*255.0,color.a*255.0);
            }
            else
            {
                circleRGBA(m_mainClass->getRenderer(),pos.getX(),pos.getY(),rad,color.r*255.0,color.g*255.0,color.b*255.0,color.a*255.0);
            }
        }
        
        for(size_t i = 0;i<m_debugSegments.size();i++)
        {
            b2Color color = m_debugSegments[i].color;
            Vector2 pos1 = m_mainClass->getPhysics()->coordsWorldToPixel(m_debugSegments[i].p1);
            Vector2 pos2 = m_mainClass->getPhysics()->coordsWorldToPixel(m_debugSegments[i].p2);
            
            m_mainClass->getCamera()->scalePos(pos1);
            m_mainClass->getCamera()->scalePos(pos2);
            
            lineRGBA(m_mainClass->getRenderer(),pos1.getX(),pos1.getY(),pos2.getX(),pos2.getY(),color.r*255.0,color.g*255.0,color.b*255.0,color.a*255.0);
        }
        
        for(size_t i = 0;i<m_debugPoints.size();i++)
        {
            b2Color color = m_debugPoints[i].color;
            
            SDL::Vector2 pos = m_mainClass->getPhysics()->coordsWorldToPixel(m_debugPoints[i].p);
            double size = m_mainClass->getPhysics()->scalarWorldToPixel(m_debugPoints[i].size);
            
            m_mainClass->getCamera()->scalePos(pos);
            
            boxRGBA(m_mainClass->getRenderer(),pos.getX()-size/2.0,pos.getY()-size/2.0,pos.getX()+size/2.0,pos.getY()+size/2.0,color.r*255.0,color.g*255.0,color.b*255.0,color.a*255.0);
        }
        
        m_debugPolygons.clear();
        m_debugCircles.clear();
        m_debugSegments.clear();
        m_debugTransforms.clear();
        m_debugPoints.clear();
        
        return true;
    }
}
