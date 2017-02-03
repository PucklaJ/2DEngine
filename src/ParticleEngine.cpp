#include "ParticleEngine.h"
#include "MainClass.h"
#include "ResourceManager.h"
#include "mathematics.h"
#include <iostream>
#include "TextureHandle.h"

namespace SDL
{
    bool ParticleEngine::defaultUpdate(Particle& p,ParticleEngine* a)
    {
        p.velocity += (Vector2)a->getGravity() * a->getMainClass()->getDeltaTimeInSeconds();
        p.position += p.velocity * a->getMainClass()->getDeltaTimeInSeconds();
        
        return true;
    }
    
    ParticleEngine::ParticleEngine(TextureHandle* texture,const Vector2& gravity) : 
        m_texture(texture),
        m_gravity(gravity)
    {
        
    }
    
    ParticleEngine::~ParticleEngine()
    {
        
    }
    
    bool ParticleEngine::init()
    {
        if(m_texture)
        {
            int w,h;
            w = m_texture->getWidth();
            h = m_texture->getHeight();
            m_size = Vector2(w,h);
        }
        
        if(!m_particleUpdate)
            m_particleUpdate = &defaultUpdate;
        
        return true;
    }
    
    void ParticleEngine::setTexture(TextureHandle* tex)
    {
        m_texture = tex;
        
        int w,h;
        
        w = m_texture->getWidth();
        h = m_texture->getHeight();
        
        m_size = Vector2(w,h);
    }
    
    bool ParticleEngine::update()
    {
        for(size_t i = m_particles.size()-1;i>=0 && i<m_particles.size();i--)
        {
            if(!m_particleUpdate(m_particles[i],this) || m_particles[i].lifeTime <= 0.0)
            {
                m_particles[i] = m_particles.back();
                m_particles.pop_back();
                i++;
                continue;
            }
            
            m_particles[i].lifeTime -= m_mainClass->getDeltaTimeInSeconds();
        }
        
        return true;
    }
    bool ParticleEngine::render()
    {
        SDL_Rect dst = {0,0,(int)m_size.getX(),(int)m_size.getY()};
        Vector2 rel;
        
        for(size_t i = 0;i<m_particles.size();i++)
        {
            rel = m_position + m_particles[i].position;
            dst.x = (int)rel.getX();
            dst.y = (int)rel.getY();
            
            m_texture->setAlphaMod(m_particles[i].alpha);
            m_texture->renderCopy(m_mainClass->getRenderer(),&dst);
            m_texture->setAlphaMod(255);
        }
        
        return true;
    }
    void ParticleEngine::quit()
    {
        if(!m_mainClass->getResourceManager()->isLoaded(m_texture))
        {
            delete m_texture;
            m_texture = nullptr;
        }
    }
    
    void ParticleEngine::emit(int amount,const Vector2& minPos,const Vector2& maxPos,const Vector2& dir,double minVel,double maxVel,double life,bool randDir)
    {
        Vector2 pos;
        Vector2 vel = dir;
        double velMul = 1.0;
        
        for(int i = 0;i<amount;i++)
        {
            pos.setX((double)(getRand(minPos.getX()*100.0,maxPos.getX()*100.0)) / 100.0);
            pos.setY((double)(getRand(minPos.getY()*100.0,maxPos.getY()*100.0)) / 100.0);
            
            vel /= velMul;
            
            velMul = (double)(getRand(minVel*100.0,maxVel*100.0)) / 100.0;
            
            if(randDir)
            {
                double angle = ((double)getRand(0,MATH_PI*2*1000))/1000.0;
                
                vel.setX(cos(angle));
                vel.setY(sin(angle));
            }
            
            vel *= velMul;
            
            m_particles.push_back(Particle(pos,vel,life));
        }
    }
}