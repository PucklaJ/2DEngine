#include <DebugMovement.h>
#include <MainClass.h>
#include <InputManager.h>
#include <Camera.h>

namespace SDL
{
    DebugMovement::DebugMovement(const double& movespeed,const double& zoomspeed) : Actor(0,"DebugMovement"),
        m_movespeed(movespeed),
        m_zoomspeed(zoomspeed)
    {
        
    }
    
    bool DebugMovement::update()
    {
        InputManager* im = m_mainClass->getInputManager();
        Camera* cam = m_mainClass->getCamera();
        double dt = m_mainClass->getDeltaTimeInSeconds();
        
        if(im->isPressed(SDLK_w))
        {
            cam->addPosition(0.0,-m_movespeed * dt);
        }
        if(im->isPressed(SDLK_s))
        {
            cam->addPosition(0.0,m_movespeed * dt);
        }
        if(im->isPressed(SDLK_a))
        {
            cam->addPosition(-m_movespeed * dt,0.0);
        }
        if(im->isPressed(SDLK_d))
        {
            cam->addPosition(m_movespeed * dt,0.0);
        }
        
        cam->addScale((double)im->getMouse().wheel_y*m_zoomspeed*dt);
        
        return true;
    }
}
