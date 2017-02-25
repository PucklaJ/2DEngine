#ifndef CAMERA_H
#define CAMERA_H
#include "Actor.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_render.h>

namespace SDL
{
    class GUI_Element;
    class Camera : public Actor
    {
    public:
            using Actor::setPosition;
            using Actor::addPosition;
        
            Camera(int order = CAM_ORDER);
            virtual ~Camera();

            bool update() override;
            bool init() override;
            void quit() override;

            bool renderActor(Actor*);

            const float getScale() const {return m_scale;}
            float getRotation() const {return m_rotation;}

            void addScale(float f) {setScale(m_scale+f);}
            void setScale(float f);
            void setRotation(float r){m_rotation = r;}

            void scalePos(Vector2&);
            void rescalePos(Vector2&,const Vector2&);
            void scaleSize(Vector2&);
            void rescaleSize(Vector2&,const Vector2&);
            
            Vector2 getWorldPosition(const Vector2&);
            
            void follow(Actor*);
            void setPosition(const float&,const float&) override;
            void addPosition(const float&,const float&) override;

        protected:

        private:
            float m_scale=1.0f;
            float m_rotation=0.0f;
            Actor* m_follow = nullptr;
            Vector2 m_helpPos;
    };
}


#endif // CAMERA_H
