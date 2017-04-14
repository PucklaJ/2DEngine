#include <Actor.h>
#include <cstdlib>
#include <algorithm>
#include <Camera.h>
#include <MainClass.h>
#include <LogManager.h>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdio>
//#define DEBUG_OUTPUTS


namespace SDL
{
    template<class t>
    const char* toString(t number)
    {
        std::string str;
        std::stringstream stream;

        stream << number;
        stream >> str;
        stream.clear();

        return str.c_str();
    }

    std::vector<int> Actor::m_ids;

    Actor::Actor(int order,const char* name) :
        m_name(name),
        m_parent(nullptr),
        m_priority(order)
    {
        do
        {
            m_id = rand();
        }while(std::find(m_ids.begin(),m_ids.end(),m_id) != m_ids.end());
    }

    Actor::~Actor()
    {
        //dtor
    }

    bool Actor::m_update()
    {
#ifdef DEBUG_OUTPUTS
    	std::cout << "m_update: " << m_name << std::endl;
    	std::cout << "Children: " << m_children.size() << std::endl;
#endif
        for(std::size_t i = 0;i<m_children.size();i++)
        {
#ifdef DEBUG_OUTPUTS
        	std::cout << "Children[" << i << "]: " << m_children[i]->getName() << std::endl;
#endif

            if(!m_children[i]->m_all_update())
                return false;
        }
#ifdef DEBUG_OUTPUTS
        std::cout << "Updated Children" << std::endl;
        std::cout << "Updateing Tweens" << std::endl;
#endif

        for(std::size_t i = 0;i<m_tweens.size();i++)
        {
        	if(m_tweens[i]->isPaused())
        		continue;

            if(m_tweens[i]->update())
            {
#ifdef DEBUG_OUTPUTS
            	std::cout << "Tween Finished" << std::endl;
#endif
                delete m_tweens[i];
                m_tweens[i] = m_tweens.back();
                m_tweens.pop_back();
                i--;
            }
        }
#ifdef DEBUG_OUTPUTS
        std::cout << "End of m_update: " << m_name << std::endl;
#endif

        return true;
    }

    bool Actor::m_render()
    {
        for(std::size_t i = 0;i<m_children.size();i++)
        {
            if(!m_children[i]->m_all_render())
                return false;
        }

        return true;
    }

    bool Actor::render()
    {
        return true;
    }

    void Actor::quit()
    {

    }

    void Actor::m_quit()
    {
        removeChildrenAfterLoops();

        for(size_t i = 0;i<m_tweens.size();i++)
        {
            delete m_tweens[i];
        }
        m_tweens.clear();

        m_parent = nullptr;
        for(std::size_t i = 0;i<m_children.size();i++)
        {
            m_children[i]->m_all_quit();
            delete m_children[i];
        }
        m_children.clear();
    }

    bool Actor::addChild(Actor* a,bool instantAdd)
    {
#ifdef DEBUG_OUTPUTS
    	std::cout << "AddChild: " << a->getName() << std::endl;
#endif

        if(a->getID() == getID() || isChild(a))
            return true;

        if(!instantAdd)
        {
            m_childrenToAdd.push_back(a);
        }
        else
        {
            Actor* p = a->getParent();
            if(p)
            {
                p->removeChild(a,false);
            }

            a->setParent(this);
            a->setMainClass(m_mainClass);
            a->setRenderer(m_renderer);

            m_children.push_back(a);

            m_needsChildReorder = true;

            if(!a->m_initialised)
            {
                a->m_initialised = true;
                return a->init();
            }
        }

        return true;
    }

    bool Actor::addChildrenBeforeLoops()
    {
#ifdef DEBUG_OUTPUTS
    	std::cout << "Add Children Before Loops: " << m_name << std::endl;
#endif

        if(m_childrenToAdd.empty())
        {
#ifdef DEBUG_OUTPUTS
        	std::cout << "Returned From Empty" << std::endl;
#endif
        	return true;
        }


        for(size_t i = 0;i<m_childrenToAdd.size();i++)
        {
#ifdef DEBUG_OUTPUTS
        	std::cout << "ChildrenToAdd[" << i << "]: " << m_name << std::endl;
#endif

            Actor* p = m_childrenToAdd[i]->getParent();
            if(p)
            {
#ifdef DEBUG_OUTPUTS
            	std::cout << "Removing parent" << std::endl;
#endif
                p->removeChild(m_childrenToAdd[i],false);
            }

#ifdef DEBUG_OUTPUTS
            std::cout << "setting variables" << std::endl;
#endif

            m_childrenToAdd[i]->setParent(this);
            m_childrenToAdd[i]->setMainClass(m_mainClass);
            m_childrenToAdd[i]->setRenderer(m_renderer);

#ifdef DEBUG_OUTPUTS
            std::cout << "Adding to array" << std::endl;
#endif
            m_children.push_back(m_childrenToAdd[i]);

            m_needsChildReorder = true;

            if(!m_childrenToAdd[i]->m_initialised)
            {
#ifdef DEBUG_OUTPUTS
            	std::cout << "initializing child" << std::endl;
#endif
                if(!m_childrenToAdd[i]->init())
                    return false;
                m_childrenToAdd[i]->m_initialised = true;
#ifdef DEBUG_OUTPUTS
                std::cout << "initialized" << std::endl;
#endif
            }


        }

        m_childrenToAdd.clear();
#ifdef DEBUG_OUTPUTS
        std::cout << "ChildrenBeforeLoops End" << std::endl;
#endif

        return true;
    }

    bool Actor::isChild(Actor* a)
    {
        for(std::size_t i = 0;i<m_children.size();i++)
        {
            if(m_children[i]->getID() == a->getID())
            {
                return true;
            }
        }

        return false;
    }

    void Actor::setParent(Actor* a)
    {
        m_parent = a;
    }

    void Actor::setPosition(const Vector2& p)
    {
        setPosition(p.getX(),p.getY());
    }

    void Actor::setPosition(const float& x, const float& y)
    {
        m_position.setX(x);
        m_position.setY(y);
    }

    void Actor::addPosition(const Vector2& p)
    {
        addPosition(p.getX(),p.getY());
    }

    void Actor::addPosition(const float& x, const float& y)
    {
        m_position.setX(m_position.getX()+x);
        m_position.setY(m_position.getY()+y);
    }

    void Actor::setSize(const Vector2& p)
    {
        setSize(p.getX(),p.getY());
    }

    void Actor::setSize(const float& x, const float& y)
    {
        m_size.setX(x);
        m_size.setY(y);
    }

    void Actor::addSize(const Vector2& p)
    {
        addSize(p.getX(),p.getY());
    }

    void Actor::addSize(const float& x, const float& y)
    {
        m_size.setX(m_size.getX()+x);
        m_size.setY(m_size.getY()+y);
    }

    const Vector2& Actor::getPosition() const
    {
        return m_position;
    }

    void Actor::removeChild(Actor* a,bool del,bool instant)
    {
        if(!instant)
        {
            m_idsToRemove.push_back(a->getID());
            m_ifDeleteToRemove.push_back(del);
        }
        else
        {
            for(std::size_t i = 0;i<m_children.size();i++)
            {
                if(m_children[i]->getID() == a->getID())
                {
                    if(del)
                    {
                        m_children[i]->m_all_quit();
                        delete m_children[i];
                    }
                    m_children[i] = m_children.back();
                    m_children.pop_back();

                    m_needsChildReorder = true;

                    return;
                }
            }
        }

    }

    void Actor::removeChildrenAfterLoops()
    {
#ifdef DEBUG_OUTPUTS
    	std::cout << "Remove Children After Loops: " << m_name << std::endl;
#endif
        if(m_idsToRemove.empty())
        {
#ifdef DEBUG_OUTPUTS
        	std::cout << "Returned From Empty" << std::endl;
#endif
        	return;
        }


        int id;
        bool del;
        for(size_t u = 0;u < m_idsToRemove.size();u++)
        {
            id = m_idsToRemove[u];
            del = m_ifDeleteToRemove[u];
#ifdef DEBUG_OUTPUTS
            std::cout << "Reovable[" << u << "]: " << m_name << std::endl;
#endif

            for(std::size_t i = 0;i<m_children.size();i++)
            {
                if(m_children[i]->getID() == id)
                {
                    if(del)
                    {
#ifdef DEBUG_OUTPUTS
                    	std::cout << "Found and Delete" << std::endl;
#endif
                        m_children[i]->m_all_quit();
                        delete m_children[i];
                    }
                    m_children[i] = m_children.back();
                    m_children.pop_back();

                    m_needsChildReorder = true;
                    break;
                }
            }
        }

        m_idsToRemove.clear();
        m_ifDeleteToRemove.clear();
    }

    void Actor::m_all_quit()
    {
        quit();
        m_quit();
    }

    bool Actor::m_all_render()
    {
        if(!m_visible)
            return true;

        if(!m_renderer)
        {
            m_renderer = m_mainClass->getRenderer();
        }

        if(m_isAffectedByCamera)
        {
            return m_mainClass->getCamera()->renderActor(this);
        }
        else
        {
            return m_render_render();
        }


        return true;
    }

    bool Actor::m_all_update()
    {
#ifdef DEBUG_OUTPUTS
    	std::cout << "m_all_update: " << m_name << std::endl;
#endif
        if(!addChildrenBeforeLoops())
            return false;
#ifdef DEBUG_OUTPUTS
        std::cout << "After Children adding" << std::endl;
#endif

        if(m_needsChildReorder)
        {
#ifdef DEBUG_OUTPUTS
        	std::cout << "Reorder Children" << std::endl;
#endif
            reorderChildren();
            m_needsChildReorder = false;
        }
#ifdef DEBUG_OUTPUTS
        std::cout << "Right Before updating" << std::endl;
#endif

        if(!(update() && m_update()))
            return false;
#ifdef DEBUG_OUTPUTS
        std::cout << "m_all_update end: " << m_name << std::endl;
#endif

        return true;
    }

    void Actor::setMainClass(MainClass* mc)
    {
        m_mainClass = mc;
    }

    MainClass* Actor::getMainClass()
    {
        return m_mainClass;
    }

    bool Actor::init()
    {
        return true;
    }

    void Actor::setAffectedByCamera(const bool b)
    {
        if(m_affectedByScaling != b)
        {
            m_renderer = nullptr;
        }
        m_isAffectedByCamera = b;
    }

    void Actor::setVisible(const bool b)
    {
        m_visible = b;
    }

    bool Actor::isOnScreen()
    {
        if(m_position.getX() + m_size.getX() < 0 || m_position.getY() + m_size.getY() < 0 ||
           m_position.getX() > NATIVE_W || m_position.getY() > NATIVE_H)
        {
            return false;
        }

        return true;
    }

    bool Actor::m_render_render()
    {
        if(!m_visible)
        {
#ifdef DEBUG_OUTPUTS
        std::cout << "Children Before: " << m_children.size() << std::endl;
#endif

        removeChildrenAfterLoops();
#ifdef DEBUG_OUTPUTS
        std::cout << "Children After: " << m_children.size() << std::endl;
#endif
        return true;
        }


        if(!isOnScreen())
        {
#ifdef DEBUG_OUTPUTS
        std::cout << "Children Before: " << m_children.size() << std::endl;
#endif

        removeChildrenAfterLoops();
#ifdef DEBUG_OUTPUTS
        std::cout << "Children After: " << m_children.size() << std::endl;
#endif
            return true;
        }

        m_scaledPos.setX(m_position.getX() * m_mainClass->getScaleW());
        m_scaledPos.setY(m_position.getY() * m_mainClass->getScaleH());
        m_scaledSize.setX(m_size.getX() * m_mainClass->getScaleW());
        m_scaledSize.setY(m_size.getY() * m_mainClass->getScaleH());

        m_dstRect.x = m_position.getX();
        m_dstRect.y = m_position.getY();
        m_dstRect.w = m_size.getX();
        m_dstRect.h = m_size.getY();

        if(!m_renderChildrenAfter)
        {
            if(!(m_render() && render()))
            {
#ifdef DEBUG_OUTPUTS
        std::cout << "Children Before: " << m_children.size() << std::endl;
#endif

        removeChildrenAfterLoops();
#ifdef DEBUG_OUTPUTS
        std::cout << "Children After: " << m_children.size() << std::endl;
#endif
                return false;
            }
        }
        else
        {
            if(!(render() && m_render()))
            {
#ifdef DEBUG_OUTPUTS
        std::cout << "Children Before: " << m_children.size() << std::endl;
#endif

        removeChildrenAfterLoops();
#ifdef DEBUG_OUTPUTS
        std::cout << "Children After: " << m_children.size() << std::endl;
#endif
                return false;
            }
        }
#ifdef DEBUG_OUTPUTS
        std::cout << "Children Before: " << m_children.size() << std::endl;
#endif

        removeChildrenAfterLoops();
#ifdef DEBUG_OUTPUTS
        std::cout << "Children After: " << m_children.size() << std::endl;
#endif

        return true;
    }

    void Actor::doThisWithAllChildren(void(*func)(Actor*))
    {
        for(std::size_t i = 0;i<m_children.size();i++)
        {
            (*func)(m_children[i]);
            m_children[i]->doThisWithAllChildren(func);
        }
    }

    void Actor::setPriority(int order)
    {
        m_priority = order;
        reorderChildren();
        m_parent->m_needsChildReorder = true;
    }

    void Actor::reorderChildren()
    {
        if(m_children.empty())
            return;

        int min = m_children[0]->getPriority();
        int minI = 0;
        Actor* temp = nullptr;

        for(size_t u = 0;u<m_children.size();u++)
        {
            minI = (int)u;
            min = m_children[u]->getPriority();
            for(size_t i = u;i<m_children.size();i++)
            {
                if(m_children[i]->getPriority() < min)
                {
                    min = m_children[i]->getPriority();
                    minI = (int)i;
                }
            }

            temp = m_children[u];
            m_children[u] = m_children[minI];
            m_children[minI] = temp;
        }

    }

    int Actor::addTween(Tween* t)
    {
        m_tweens.push_back(t);
        t->setParent(this);
        t->init();

        return t->getID();
    }

    void Actor::clearTweens()
    {
        for(size_t i = 0;i<m_tweens.size();i++)
        {
            delete m_tweens[i];
        }

        m_tweens.clear();
    }

    void Actor::pauseTween(int id)
    {
    	for(size_t i = 0;i<m_tweens.size();i++)
    	{
    		if(m_tweens[i]->getID() == id)
    		{
    			m_tweens[i]->setPaused(true);
    			return;
    		}
    	}
    }
    void Actor::resumeTween(int id)
    {
    	for(size_t i = 0;i<m_tweens.size();i++)
		{
			if(m_tweens[i]->getID() == id)
			{
				m_tweens[i]->setPaused(false);
				return;
			}
		}
    }
    void Actor::stopTween(int id)
    {
    	for(size_t i = 0;i<m_tweens.size();i++)
		{
			if(m_tweens[i]->getID() == id)
			{
				delete m_tweens[i];
				m_tweens[i] = m_tweens.back();
				m_tweens.pop_back();
				return;
			}
		}
    }

    Tween* Actor::getTween(int id)
    {
    	for(size_t i = 0;i<m_tweens.size();i++)
    	{
    		if(m_tweens[i]->getID() == id)
    			return m_tweens[i];
    	}

    	return nullptr;
    }


}
