#include "Camera.h"
#include "LogManager.h"
#include "MainClass.h"
#include "GUI_Element.h"
#include <iostream>
namespace SDL
{
    Camera::Camera(int order) : Actor(order,"Camera")
    {
        m_isAffectedByCamera = false;
        
    }

    bool Camera::init()
    {
        setPosition(NATIVE_W/2.0,NATIVE_H/2.0);
        
        m_size.setX(NATIVE_W);
        m_size.setY(NATIVE_H);

        return true;
    }

    Camera::~Camera()
    {
        //dtor
    }

    void Camera::scalePos(Vector2& p)
    {
        p.setX((p.getX() - m_position.getX())*m_scale);
        p.setY((p.getY() - m_position.getY())*m_scale);
    }

    void Camera::rescalePos(Vector2& p, const Vector2& temp)
    {
        p = temp;
    }

    void Camera::scaleSize(Vector2& p)
    {
        p.setX(m_scale * p.getX());
        p.setY(m_scale * p.getY());
    }

    void Camera::rescaleSize(Vector2& p, const Vector2& temp)
    {
        p = temp;
    }

    bool Camera::renderActor(Actor* a)
    {
        Vector2 tempPos = a->getPosition();
        Vector2 pos = a->getPosition();
        Vector2 tempSize = a->getSize();
        Vector2 size = a->getSize();

        scalePos(pos);
        scaleSize(size);
        a->setPosition(pos);
        a->setSize(size);
        Vector2 temp1 = a->customCameraScalingBeforeRendering();

        if(!a->m_render_render())
            return false;

        rescalePos(pos,tempPos);
        rescaleSize(size,tempSize);
        a->setPosition(pos);
        a->setSize(size);
        a->customCameraScalingAfterRendering(temp1);

        return true;
    }

    void Camera::setScale(float f)
    {
        m_scale=f;
    }

    bool Camera::update()
    {
        if(m_follow)
        {
            m_position = (Vector2)m_follow->getPosition() - Vector2(NATIVE_W/2.0,NATIVE_H/2.0)/(double)m_scale;
        }
        else
        {
            m_position = m_helpPos - Vector2(NATIVE_W/2.0,NATIVE_H/2.0)/(double)m_scale;
        }
        
        return true;
    }

    void Camera::quit()
    {

    }
    
    SDL::Vector2 Camera::getWorldPosition(const Vector2& screenPos)
    {
        SDL::Vector2 worldPos(screenPos.getX(),screenPos.getY());
        
        worldPos.set((worldPos.getX() + m_position.getX()*m_scale)/m_scale,(worldPos.getY() + m_position.getY()*m_scale)/m_scale);
        
        return worldPos;
    }
    
    Vector2 Camera::getWorldPosition(const Mouse& ms)
    {
        return getWorldPosition(Vector2(ms.x,ms.y));
    }
    
    Vector2 Camera::getScreenPosition(const Vector2& worldPos)
    {
        Vector2 screenPos(worldPos.getX(),worldPos.getY());
        
        screenPos.set(screenPos.getX() * m_scale - m_position.getX()*m_scale,screenPos.getY() * m_scale - m_position.getY()*m_scale);
        
        return screenPos;
    }

    void Camera::follow(Actor* a)
    {
        m_follow = a;
        if(!a)
        {
            m_helpPos = m_position + Vector2(NATIVE_W/2.0,NATIVE_H/2.0)/(double)m_scale;
        }
    }

    void Camera::setPosition(const float& x,const float& y)
    {
        Actor::setPosition(x,y);
        m_helpPos = m_position;
    }
    
    void Camera::addPosition(const float& x, const float& y)
    {
        Actor::addPosition(x,y);
        m_helpPos = m_position + Vector2(NATIVE_W/2.0,NATIVE_H/2.0)/(double)m_scale;
    }
}



