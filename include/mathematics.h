#ifndef MATHEMATICS_H_INCLUDED
#define MATHEMATICS_H_INCLUDED
#include <ostream>
#include <cmath>
#include <vector>
#include <algorithm>
#define MATH_PI 3.14159265359

namespace SDL
{
    class MainClass;
    
    class Vector2
    {
    public:
        Vector2(double x = 0.0,double y = 0.0);
        virtual ~Vector2();

        double getX() const {return m_x;}
        double getY() const {return m_y;}
        double length() const;

        void setX(double x){m_x=x;}
        void setY(double y){m_y=y;}
        void set(double x,double y) {m_x=x;m_y=y;}
        
        const Vector2& normalise();
        static double distance(const Vector2&,const Vector2&,bool sqrd = false);
        
        bool isOnScreen(MainClass* c = nullptr);

        const Vector2& operator+=(const Vector2&);
        Vector2 operator+(const Vector2&);
        const Vector2& operator-=(const Vector2&);
        Vector2 operator-(const Vector2&);
        const Vector2& operator*=(float);
        Vector2 operator*(float);
        const Vector2& operator/=(float);
        Vector2 operator/(float);
        const Vector2& operator*=(double);
        Vector2 operator*(double);
        bool operator==(const Vector2&);
    protected:
    private:
        double m_x;
        double m_y;

    };
    extern double abs(const double& d);
    extern int getRand(int min,int max);
    extern bool luck(int percent);
    
    struct A_Square
    {
    A_Square(int x_,int y_,unsigned int g_, bool solid_)
    {
        x = x_;
        y = y_;
        g = g_;
        solid = solid_;
    }
        
    int x;
    int y;
    unsigned int g;
    bool solid;
    A_Square* m_previous = nullptr;
    };

    template<class T>
    inline T get(const std::vector<T>& vars,int x, int y,int w)
    {
        return vars[y * w + x];
    }
    
    template<class T,class D>
    inline bool contains(const std::vector<T>& vars,const D& var)
    {
        return std::find(vars.begin(),vars.end(),var) != vars.end();
    }
    
    template<class T>
    inline void swap(T var1,T var2)
    {
        T temp = var1;
        var1 = var2;
        var2 = temp;
    }
    
    template<class T>
    inline void reverse(const std::vector<T>& vars)
    {
        int j = 0;
        for(int i = (int)vars.size()-1;i>j;i--)
        {
            swap(vars[i],vars[j]);
            j++;
        }
    }
    
    template<class T>
    inline int sizeOfArray(T var[])
    {
        return sizeof(var)/sizeof(*var);
    }
    
    extern std::vector<A_Square> a_stern(std::vector<A_Square>&,A_Square&,A_Square&,int);

}

extern std::ostream& operator<<(std::ostream&,const SDL::Vector2&);





#endif // MATHEMATICS_H_INCLUDED
