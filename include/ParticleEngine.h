#ifndef PARTICLEENGINE_H
#define PARTICLEENGINE_H
#include "Actor.h"

namespace SDL
{
    class TextureHandle;
    
    struct Particle
    {
      Particle(const Vector2& pos,const Vector2& vel,double life)
      {
          position = pos;
          velocity = vel;
          lifeTime = life;
          alpha = 255;
      }
        
      Vector2 position;
      Vector2 velocity;
      
      Uint8 alpha;
      double lifeTime;
    };
    
   class ParticleEngine : public Actor
   {
    public:
        ParticleEngine(TextureHandle*,const Vector2& gravity = Vector2());
        ~ParticleEngine();
        
        bool init() override;
        bool update() override;
        bool render() override;
        void quit() override;
        
        void emit(int,const Vector2&,const Vector2&,const Vector2&,double,double,double,bool randDir = true);
        
        void setSize(const Vector2& size) {m_size = size;}
        void setGravity(const Vector2& gravity) {m_gravity = gravity;}
        void setTexture(TextureHandle* tex);
        void setParticleUpdate(bool (*up)(Particle&,ParticleEngine*)){m_particleUpdate = up;}
        
        const Vector2& getGravity() const {return m_gravity;}
        const Vector2& getSize() const {return m_size;}
        TextureHandle* getTexture() {return m_texture;}
        
        static bool defaultUpdate(Particle&,ParticleEngine*);
        
    private:
        std::vector<Particle> m_particles;
        
        Vector2 m_gravity;
        Vector2 m_size;
        TextureHandle* m_texture;
        
        bool (*m_particleUpdate)(Particle&,ParticleEngine*) = nullptr;
   }; 
}




#endif