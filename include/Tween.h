#ifndef TWEEN_H
#define TWEEN_H
#include "mathematics.h"
#include <vector>
#include <SDL2/SDL_render.h>

namespace SDL
{
    class Actor;
    class Sprite;
    class TextureHandle;
    
    class Tween
    {
        public:
            Tween(double);
            virtual ~Tween();

            virtual bool update()=0;
            virtual void init();
            void setParent(Actor*);
            
            void addTime(double);

        protected:
            double m_time;
            double m_passedTime = 0.0;
            Actor* m_parent;

        private:

    };

    class PositionTween : public Tween
    {
    public:
        PositionTween(const Vector2&,double);
        virtual ~PositionTween();

        void init() override;
        bool update() override;

    private:
        Vector2 m_dstPos;
        Vector2 m_vel;
    };
    
    class AnimationTween : public Tween
    {
    public:
        AnimationTween(TextureHandle* atlas,int x,int y,double time,bool loop = true,bool customTime = false);
        AnimationTween(const std::vector<TextureHandle*>&,double time,bool loop = true,bool customTime = false);
        ~AnimationTween();
        
        void init() override;
        bool update() override;
    private:
        Sprite* m_sprParent;
        std::vector<TextureHandle*>* m_textures = nullptr;
        TextureHandle* m_atlas = nullptr;
        int m_x;
        int m_y;
        SDL_Rect m_srcRect;
        bool m_loop;
        bool m_customTime;
    };
    
    class PulsateTween : public Tween
    {
    public:
        PulsateTween(double,double speed = 6.0,bool loop = true);
        ~PulsateTween();
        
        void init() override;
        bool update() override;
    private:
        bool m_loop;
        double m_length;
        double m_speed;
        Vector2 m_startSize,m_startPos;
    };
}


#endif // TWEEN_H
