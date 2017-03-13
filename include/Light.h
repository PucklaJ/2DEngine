#ifndef LIGHT_H
#define LIGHT_H
#include <Sprite.h>

namespace SDL
{
    class Light : public Sprite
    {
    public:
        Light(const SDL_Color&,double,const char* name = "Light");
        ~Light();
        
        bool init() override;
        bool update() override;
        bool m_render() override;
        bool render() override;
        
        void setRadius(double);
        void setColor(const SDL_Color&);
        
        double getRadius() const {return m_radius;}
        const SDL_Color& getColor() const {return m_color;}
        
    private:
        SDL_Color m_color;
        double m_radius;
    };
}



#endif