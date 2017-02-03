#ifndef ACTOR_H
#define ACTOR_H
#include <vector>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include "mathematics.h"
#include "Tween.h"

#define NORM_W 1280.0
#define NORM_H 720.0

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif
#define IPMGR_ORDER INT_MAX
#define GUI_ORDER IPMGR_ORDER-1
#define CAM_ORDER GUI_ORDER-1
#define B2DEBUG_ORDER CAM_ORDER-1

namespace SDL
{
    class TextureHandle;
    
    extern TextureHandle* createTexture(SDL_Renderer*,const SDL_Rect&,const SDL_Color&);
    extern TextureHandle* createTexture(SDL_Renderer*,const Vector2&,const SDL_Color&);

    class MainClass;

    class Actor
    {
    public:
            Actor(int order = 0,const char* name = "Actor");
            virtual ~Actor();

            virtual bool init();
            virtual bool update()=0;
            virtual bool m_update();
            virtual bool m_render();
            virtual bool render();
            virtual void quit();
            virtual void m_quit();

            bool addChild(Actor*,bool instantAdd = true);
            void addTween(Tween*);
            void removeChild(Actor*,bool del = true,bool instant = false);

            void setParent(Actor*);
            void setPosition(const Vector2&);
            virtual void setPosition(const float&,const float&);
            void addPosition(const Vector2&);
            virtual void addPosition(const float&,const float&);
            void setSize(const Vector2&);
            virtual void setSize(const float&,const float&);
            void addSize(const Vector2&);
            virtual void addSize(const float&,const float&);
            void setMainClass(MainClass*);
            void setAffectedByCamera(const bool);
            void setVisible(const bool);
            void setScaledPos(const Vector2& r){m_scaledPos = r;}
            void setScaledSize(const Vector2& r){m_scaledSize = r;}
            void setRenderer(SDL_Renderer* r) {m_renderer = r;}
            void setPriority(int);
            void setName(const char* name){m_name = name;}


            MainClass* getMainClass();
            const Vector2& getPosition() const;
            const Vector2& getSize() const{return m_size;}
            Actor* getParent(){return m_parent;}
            std::vector<Actor*>& getChildren() {return m_children;}
            bool isAffectedByCamera() const {return m_isAffectedByCamera;}
            const char* getName() const {return m_name;}
            const bool isVisible() const {return m_visible;}
            const Vector2& getScaledPos() const {return m_scaledPos;}
            const Vector2& getScaledSize() const {return m_scaledSize;}
            SDL_Renderer* getRenderer() const {return m_renderer;}
            const int getID() const {return m_id;}
            const int getPriority() const {return m_priority;}
            std::vector<Tween*>& getTweens() {return m_tweens;}

            bool isChild(Actor*);

            void m_all_quit();
            bool m_all_render();
            bool m_all_update();
            bool m_render_render();

            virtual const Vector2 customCameraScalingBeforeRendering(){Vector2 r; return r;}
            virtual void customCameraScalingAfterRendering(const Vector2&){}

            void doThisWithAllChildren(void(*)(Actor*));
            void reorderChildren();
            void clearTweens();
            
            virtual bool isOnScreen();
    protected:

            MainClass* m_mainClass = nullptr;
            bool m_isAffectedByCamera = true;
            bool m_visible = true;
            bool m_affectedByScaling = true;
            bool m_renderChildrenAfter = false;
            Vector2 m_position;
            Vector2 m_size;
            Vector2 m_scaledPos;
            Vector2 m_scaledSize;
            SDL_Renderer* m_renderer = nullptr;
            SDL_Rect m_dstRect;

            const char* m_name = "";

    private:
            
            void removeChildrenAfterLoops();
            bool addChildrenBeforeLoops();

            std::vector<int> m_idsToRemove;
            std::vector<bool> m_ifDeleteToRemove;
            std::vector<Actor*> m_childrenToAdd;
            static std::vector<int> m_ids;
            int m_id;

            Actor* m_parent;
            std::vector<Actor*> m_children;
            std::vector<Tween*> m_tweens;

            int m_priority;
            bool m_initialised = false;
            bool m_needsChildReorder = false;

    };
}


#endif // ACTOR_H
