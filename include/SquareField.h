#ifndef SQUARE_FIELD_H
#define SQUARE_FIELD_H
#include <Actor.h>
#include <mathematics.h>

#define SQUARE_SCALE 20

class SquareField : public SDL::Actor
{
public:
    SquareField();
    ~SquareField();
    
    bool init() override;
    bool update() override;
    bool render() override;
    void quit() override;
private:
    std::vector<SDL::A_Square> m_squares;
    SDL::A_Square* m_start = nullptr;
    SDL::A_Square* m_target = nullptr;
    std::vector<SDL::A_Square> m_path;
};


#endif