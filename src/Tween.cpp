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
	std::vector<int> Tween::m_ids;

    Tween::Tween(double time) :
        m_time(time),
		m_parent(nullptr)
    {
    	do
    	{
    		m_id = rand();
    	}while(std::count(m_ids.begin(),m_ids.end(),m_id) != 0);

    	m_ids.push_back(m_id);
    }

    Tween::~Tween()
    {
        quit();
    }

    void Tween::quit()
    {
    	for(size_t i = 0;i<m_ids.size();i++)
		{
			if(m_ids[i] == m_id)
			{
				m_ids[i] = m_ids.back();
				m_ids.pop_back();
				return;
			}
		}
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
    	m_sprParent(nullptr),
		m_atlas(atlas),
        m_x(x),
        m_y(y),
        m_loop(loop),
        m_customTime(customTime)

    {
        
    }
    
    AnimationTween::AnimationTween(const std::vector<TextureHandle*>& texs,double time,bool loop,bool customTime) : Tween(time),
    	m_sprParent(nullptr),
		m_x(0),
		m_y(0),
		m_loop(loop),
		m_customTime(customTime)
    {
        m_textures = new std::vector<TextureHandle*>();
        
        for(size_t i = 0;i<texs.size();i++)
        {
            m_textures->push_back(texs[i]);
        }
        
    }
    
    AnimationTween::AnimationTween(const std::vector<SDL_Rect>& rects,double time,bool loop,bool customTime) : Tween(time),
		m_sprParent(nullptr),
		m_x(0),
		m_y(0),
		m_loop(loop),
		m_customTime(customTime)
	{
		m_rects = new std::vector<SDL_Rect>();

		for(size_t i = 0;i<rects.size();i++)
		{
			m_rects->push_back(rects[i]);
		}

	}

    AnimationTween::~AnimationTween()
    {
    	quit();

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
        else if(m_rects)
        {
			m_rects->clear();
			delete m_rects;
        }

    }
    
    void AnimationTween::init()
    {
        m_sprParent = dynamic_cast<Sprite*>(m_parent);
        
        if(!m_sprParent)
        {
            LogManager::log("AnimationTween can only be given to a Sprite!");
            return;
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
        else if(m_textures)
        {
            m_srcRect.x = 0;
            if(m_textures->empty())
            {
                LogManager::log("Texture Array of AnimationTween is empty!");
            }
            else
                m_sprParent->setTexture((*m_textures)[0]);
        }
        else
        {
        	m_srcRect.x = 0;
        	if(m_rects->empty())
        	{
        		LogManager::log("SDL_Rect Array of AnimationTween is empty");
        	}
        	else
        	{
        		m_sprParent->setSrcRect((*m_rects)[0]);
        		m_sprParent->setSize((*m_rects)[0].w,(*m_rects)[0].h);
        	}

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
                else if(m_textures)
                {
                    if(++m_srcRect.x == (int)m_textures->size())
                    {
                    	m_srcRect.x = 0;

                        if(!m_loop)
                        {
                        	return true;
                        }
                    }
                    
                    m_sprParent->setTexture((*m_textures)[m_srcRect.x]);
                }
                else
                {
                	if(++m_srcRect.x == (int)m_rects->size())
                	{
                		m_srcRect.x = 0;

                		if(!m_loop)
                		{
                			return true;
                		}
                	}

                	m_sprParent->setSrcRect((*m_rects)[m_srcRect.x]);
                	m_sprParent->setSize((*m_rects)[m_srcRect.x].w,(*m_rects)[m_srcRect.x].h);
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

