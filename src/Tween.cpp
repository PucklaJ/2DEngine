#include "Tween.h"
#include "Actor.h"
#include <iostream>
#include "MainClass.h"
#include "Sprite.h"
#include "LogManager.h"
#include <cmath>
#include "TextureHandle.h"
#include "ResourceManager.h"

namespace SDL
{
    Tween::Tween(double time) :
        m_time(time),
		m_parent(nullptr)
    {
        //ctor
    }

    Tween::~Tween()
    {
        //dtor
    }

    void Tween::init()
    {

    }

    void Tween::setParent(Actor* a)
    {
        m_parent = a;
    }
    
    void Tween::addTime(double time)
    {
        m_passedTime += time;
    }

    PositionTween::PositionTween(const Vector2& v,double time) : Tween(time),
        m_dstPos(v)
    {

    }

    PositionTween::~PositionTween()
    {

    }

    bool PositionTween::update()
    {
        m_parent->addPosition(m_vel*m_parent->getMainClass()->getDeltaTimeInSeconds());
        if(((m_vel.getX() < 0 && m_parent->getPosition().getX() <= m_dstPos.getX()) ||
            (m_vel.getX() >= 0 && m_parent->getPosition().getX() >= m_dstPos.getX()))&&
           ((m_vel.getY() < 0 && m_parent->getPosition().getY() <= m_dstPos.getY())||
            (m_vel.getY() >= 0 && m_parent->getPosition().getY() >= m_dstPos.getY())))
        {
            m_parent->setPosition(m_dstPos);
            return true;
        }

        return false;
    }

    void PositionTween::init()
    {
        m_vel.setX((m_dstPos.getX() - m_parent->getPosition().getX())/m_time);
        m_vel.setY((m_dstPos.getY() - m_parent->getPosition().getY())/m_time);
    }
    
    AnimationTween::AnimationTween(TextureHandle* atlas,int x, int y,double time,bool loop,bool customTime) : Tween(time),
        m_atlas(atlas),
        m_x(x),
        m_y(y),
        m_loop(loop),
        m_customTime(customTime),
		m_sprParent(nullptr)
    {
        
    }
    
    AnimationTween::AnimationTween(const std::vector<TextureHandle*>& texs,double time,bool loop,bool customTime) : Tween(time),
        m_loop(loop),
        m_customTime(customTime),
		m_x(0),
		m_y(0),
		m_sprParent(nullptr)
    {
        m_textures = new std::vector<TextureHandle*>();
        
        for(size_t i = 0;i<texs.size();i++)
        {
            m_textures->push_back(texs[i]);
        }
        
    }
    
    AnimationTween::~AnimationTween()
    {
        if(m_textures)
        {
            for(size_t i = 0;i<m_textures->size();i++)
            {
                if(!m_parent->getMainClass()->getResourceManager()->isLoaded((*m_textures)[i]))
                {
                    delete (*m_textures)[i];
                }
            }
            m_textures->clear();
            delete m_textures;
        }
    }
    
    void AnimationTween::init()
    {
        m_sprParent = static_cast<Sprite*>(m_parent);
        
        if(!m_sprParent)
        {
            LogManager::log("AnimationTween can only given to a Sprite!");
        }
        
        if(m_atlas)
        {
            m_sprParent->setTexture(m_atlas);
            
            m_srcRect.x = 0;
            m_srcRect.y = 0;
            m_srcRect.w = m_atlas->getWidth() / m_x;
            m_srcRect.h = m_atlas->getHeight() / m_y;
            
            m_sprParent->setSrcRect(m_srcRect);
            m_sprParent->setSize(m_srcRect.w,m_srcRect.h);
        }
        else
        {
            m_srcRect.x = 0;
            if(m_textures->empty())
            {
                LogManager::log("Texture Array of AnimationTween is empty!");
            }
            else
                m_sprParent->setTexture((*m_textures)[0]);
        }
    }
    
    bool AnimationTween::update()
    {
            if(!m_customTime)
                m_passedTime += m_parent->getMainClass()->getDeltaTimeInSeconds();
            if(m_passedTime >= m_time)
            {
                if(m_atlas)
                {
                    m_srcRect.x+=m_srcRect.w;
                    
                    if(m_srcRect.x == (m_srcRect.w*m_x))
                    {
                        m_srcRect.x = 0;
                        m_srcRect.y+=m_srcRect.h;
                        
                        if(m_srcRect.y == (m_srcRect.h*m_y))
                        {
                            if(!m_loop)
                                return true;
                            m_srcRect.y = 0;
                        }
                    }
                    
                    m_sprParent->setSrcRect(m_srcRect);
                
                }
                else
                {
                    if(m_srcRect.x++ == (int)m_textures->size())
                    {
                        if(!m_loop)
                            return true;
                        
                        m_srcRect.x = 0;
                    }
                    
                    m_sprParent->setTexture((*m_textures)[m_srcRect.x]);
                }
                
                m_passedTime = 0.0;
                
            }
            
            return false;
    }
    
    PulsateTween::PulsateTween(double length,double speed,bool loop) : Tween(0.0),
        m_loop(loop),
        m_length(length),
        m_speed(speed)
    {
        
    }
    
    PulsateTween::~PulsateTween()
    {
        
    }
    
    void PulsateTween::init()
    {
        m_startPos = m_parent->getPosition();
        m_startSize = m_parent->getSize();
    }
    
    bool PulsateTween::update()
    {
        m_time += m_parent->getMainClass()->getDeltaTimeInSeconds() * m_speed;
        if(m_time >= 2*MATH_PI)
        {
            if(!m_loop)
            {
                m_parent->setPosition(m_startPos);
                m_parent->setSize(m_startSize);
                return true;
            }
            m_time = 0.0;
        }
        
        Vector2 normSize = m_startSize;
        normSize.normalise();
        Vector2 normPos = m_parent->getPosition();
        normPos.normalise();
        
        m_parent->setSize(m_startSize + (normSize*sin(m_time)*m_length));
        m_parent->setPosition(m_startPos - (normPos*sin(m_time)*m_length));
        
        return false;
    }

}

