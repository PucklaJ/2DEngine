#ifndef DEBUG_MOVEMENT_H
#define DEBUG_MOVEMENT_H
#include "Actor.h"

namespace SDL
{
    class DebugMovement : public Actor
    {
    public:
        DebugMovement(const double& movespeed = 100.0,const double& zoomspeed = 5.0);
        
        bool update() override;
        
    private:
        double m_movespeed;
        double m_zoomspeed;
    };
}

#endif