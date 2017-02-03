#ifndef BOX2D_DEBUG_DRAWER_H
#define BOX2D_DEBUG_DRAWER_H
#include <Box2D/Box2D.h>
#include <Actor.h>
#include <vector>

namespace SDL
{
    struct DebugPolygon
    {
        DebugPolygon(){}
        b2Vec2* vertices;
        int32 vertexCount;
        b2Color color;
        bool solid;
    };
    
    struct DebugCircle
    {
        DebugCircle(){}
        b2Vec2 center;
        float32 radius;
        b2Color color;
        bool solid;
    };
    
    struct DebugSegment
    {
        DebugSegment(){}
        b2Vec2 p1;
        b2Vec2 p2;
        b2Color color;
    };
    
    struct DebugTransform
    {
        DebugTransform(){}
        b2Transform xf;
    };
    
    struct DebugPoint
    {
        DebugPoint(){}
        b2Vec2 p;
        float32 size;
        b2Color color;
    };
    
    class Box2DDebugDrawer : public b2Draw,public Actor
    {
    public:
        Box2DDebugDrawer();
        ~Box2DDebugDrawer();
        
        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
        void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
        void DrawTransform(const b2Transform& xf) override;
        void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
        
        bool render() override;
        bool update() {return true;}
        
    private:
        std::vector<DebugPolygon> m_debugPolygons;
        std::vector<DebugCircle> m_debugCircles;
        std::vector<DebugSegment> m_debugSegments;
        std::vector<DebugTransform> m_debugTransforms;
        std::vector<DebugPoint> m_debugPoints;
    };
}


#endif