#ifndef BOX2D_DEBUG_DRAWER_H
#define BOX2D_DEBUG_DRAWER_H
#include <Box2D/Box2D.h>
#include "MainClass.h"

namespace SDL
{
    class Box2DDebugDrawer : public b2Draw
    {
    public:
        Box2DDebugDrawer(MainClass*);
        ~Box2DDebugDrawer();
        
        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
        void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
        void DrawTransform(const b2Transform& xf) override;
        void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
    private:
        MainClass* m_mainClass;
    };
}


#endif