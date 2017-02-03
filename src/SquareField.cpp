#include "SquareField.h"
#include <MainClass.h>
#include <iostream>
#include <Colors.h>
#include <ctime>
#include <LogManager.h>
#include <operators.h>
#define SQUARES_X (NORM_W)/SQUARE_SCALE
#define SQUARES_Y (NORM_H)/SQUARE_SCALE

SquareField::SquareField()
{
    
}

SquareField::~SquareField()
{
    
}

bool SquareField::init()
{
    srand((unsigned)time(nullptr));
    
    int squaresX = SQUARES_X;
    int squaresY = SQUARES_Y;
    
    std::cout << "Squares X: " << squaresX << std::endl;
    std::cout << "Squares Y: " << squaresY << std::endl;
    std::cout << "Num Squares: " << squaresX*squaresY << std::endl;
    
    for(int y = 0;y<squaresY;y++)
    {
        for(int x = 0;x<squaresX;x++)
        {
            m_squares.push_back(SDL::A_Square(x,y,SDL::getRand(1,10),SDL::luck(10)));
        }
    }
    
    do
    {
        m_start = &m_squares[SDL::getRand(0,(int)m_squares.size()-1)];
    }while(m_start->solid);
    
    do
    {
        m_target = &m_squares[SDL::getRand(0,(int)m_squares.size()-1)];
    }while(m_target->solid || (m_target->x == m_start->x && m_target->y == m_start->y));
    
    std::cout << "Target: " << m_target->x << "; " << m_target->y << std::endl;
    std::cout << "Start: " << m_start->x << "; " << m_start->y << std::endl;
    
    m_path = SDL::a_stern(m_squares,*m_start,*m_target,squaresX);
    
    std::cout << "A Stern Calculated" << std::endl;
    
    return true;
}

bool SquareField::update()
{
    if(m_mainClass->getInputManager()->justPressed(SDL_BUTTON_LEFT))
    {
        SDL::MouseCoords ms = m_mainClass->getInputManager()->getMouseCoords();
        int x = ms.x / SQUARE_SCALE;
        int y = ms.y / SQUARE_SCALE;
        
        SDL::A_Square s = SDL::get(m_squares,x,y,SQUARES_X);
        if(!s.solid)
        {
            m_start = new SDL::A_Square(s.x,s.y,s.g,s.solid);
        }
        
        m_path.clear();
        m_path = SDL::a_stern(m_squares,*m_start,*m_target,SQUARES_X);
        std::cout << "A Stern calculated" << std::endl;
    }
    else if(m_mainClass->getInputManager()->justPressed(SDL_BUTTON_RIGHT))
    {
        SDL::MouseCoords ms = m_mainClass->getInputManager()->getMouseCoords();
        int x = ms.x / SQUARE_SCALE;
        int y = ms.y / SQUARE_SCALE;
        
        SDL::A_Square s = SDL::get(m_squares,x,y,SQUARES_X);
        if(!s.solid)
        {
            m_target = new SDL::A_Square(s.x,s.y,s.g,s.solid);
        }
        
        m_path.clear();
        m_path = SDL::a_stern(m_squares,*m_start,*m_target,SQUARES_X);
        std::cout << "A Stern calculated" << std::endl;
    }
    else if(m_mainClass->getInputManager()->justPressed(SDL_BUTTON_MIDDLE))
    {
        SDL::MouseCoords ms = m_mainClass->getInputManager()->getMouseCoords();
        int x = ms.x / SQUARE_SCALE;
        int y = ms.y / SQUARE_SCALE;
        
        int index = y * SQUARES_X + x;
        m_squares[index].solid = !m_squares[index].solid;
        
        m_path.clear();
        m_path = SDL::a_stern(m_squares,*m_start,*m_target,SQUARES_X);
        std::cout << "A Stern calculated" << std::endl;
    }
    
    return true;
}

bool SquareField::render()
{
    SDL_Rect draw = {0,0,SQUARE_SCALE,SQUARE_SCALE};
    
    for(size_t i = 0;i<m_squares.size();i++)
    {
        if(m_squares[i].solid)
        {
            SDL_SetRenderDrawColor(m_mainClass->getRenderer(),0,0,255,255);
        }
        else if(m_start->x == m_squares[i].x && m_start->y == m_squares[i].y)
        {
            SDL_SetRenderDrawColor(m_mainClass->getRenderer(),0,255,0,255);
        }
        else if(m_target->x == m_squares[i].x && m_target->y == m_squares[i].y)
        {
            SDL_SetRenderDrawColor(m_mainClass->getRenderer(),255,0,0,255);
        }
        else
        {
            SDL_SetRenderDrawColor(m_mainClass->getRenderer(),(255.0*(((double)m_squares[i].g-1.0)/9.0)),(255.0*(((double)m_squares[i].g-1.0)/9.0)),(255.0*(((double)m_squares[i].g-1.0)/9.0)),255);
        }
        
        draw.x = m_squares[i].x * SQUARE_SCALE;
        draw.y = m_squares[i].y * SQUARE_SCALE;
        
        SDL_RenderFillRect(m_mainClass->getRenderer(),&draw);
    }
    
    SDL_SetRenderDrawColor(m_mainClass->getRenderer(),SDL::Colors::YELLOW.r,SDL::Colors::YELLOW.g,SDL::Colors::YELLOW.b,SDL::Colors::YELLOW.a);
    
    for(size_t i = 0;i<m_path.size();i++)
    {
        if(m_target->x == m_path[i].x && m_target->y == m_path[i].y)
            continue;
        if(m_start->x == m_path[i].x && m_start->y == m_path[i].y)
            continue;
        
        draw.x = m_path[i].x * SQUARE_SCALE;
        draw.y = m_path[i].y * SQUARE_SCALE;
        
        SDL_RenderFillRect(m_mainClass->getRenderer(),&draw);
    }
    
    
    SDL_SetRenderDrawColor(m_mainClass->getRenderer(),0,0,0,0);
    
    return true;
}

void SquareField::quit()
{
    
}