/*
 * ContactListener.cpp
 *
 *  Created on: 02.04.2017
 *      Author: johnny
 */
#include <ContactListener.h>
#include <PhysicsSprite.h>
#include <iostream>

namespace SDL
{
	ContactListener::ContactListener()
	{

	}
	ContactListener::~ContactListener()
	{

	}

	void ContactListener::BeginContact(b2Contact* contact)
	{
		b2Fixture* f1 = contact->GetFixtureA();
		b2Fixture* f2 = contact->GetFixtureB();

		PhysicsSprite* spr1 = nullptr;
		PhysicsSprite* spr2 = nullptr;

		spr1 = static_cast<PhysicsSprite*>(f1->GetBody()->GetUserData());
		spr2 = static_cast<PhysicsSprite*>(f2->GetBody()->GetUserData());

		if(spr1)
		{
			spr1->BeginContact(contact,f1,f2);
		}
		if(spr2)
		{
			spr2->BeginContact(contact,f2,f1);
		}
	}
	void ContactListener::EndContact(b2Contact* contact)
	{
		b2Fixture* f1 = contact->GetFixtureA();
		b2Fixture* f2 = contact->GetFixtureB();

		PhysicsSprite* spr1 = nullptr;
		PhysicsSprite* spr2 = nullptr;

		spr1 = static_cast<PhysicsSprite*>(f1->GetBody()->GetUserData());
		spr2 = static_cast<PhysicsSprite*>(f2->GetBody()->GetUserData());

		if(spr1)
		{
			spr1->EndContact(contact,f1,f2);
		}
		if(spr2)
		{
			spr2->EndContact(contact,f2,f1);
		}
	}

	void ContactListener::PreSolve(b2Contact* contact,const b2Manifold* oldManifold)
	{
		b2Fixture* f1 = contact->GetFixtureA();
		b2Fixture* f2 = contact->GetFixtureB();

		PhysicsSprite* spr1 = nullptr;
		PhysicsSprite* spr2 = nullptr;

		spr1 = static_cast<PhysicsSprite*>(f1->GetBody()->GetUserData());
		spr2 = static_cast<PhysicsSprite*>(f2->GetBody()->GetUserData());

		if(spr1)
		{
			spr1->PreSolve(contact,f1,f2,oldManifold);
		}
		if(spr2)
		{
			spr2->PreSolve(contact,f2,f1,oldManifold);
		}
	}
	void ContactListener::PostSolve(b2Contact* contact,const b2ContactImpulse* impulse)
	{
		b2Fixture* f1 = contact->GetFixtureA();
		b2Fixture* f2 = contact->GetFixtureB();

		PhysicsSprite* spr1 = nullptr;
		PhysicsSprite* spr2 = nullptr;

		spr1 = static_cast<PhysicsSprite*>(f1->GetBody()->GetUserData());
		spr2 = static_cast<PhysicsSprite*>(f2->GetBody()->GetUserData());

		if(spr1)
		{
			spr1->PostSolve(contact,f1,f2,impulse);
		}
		if(spr2)
		{
			spr2->PostSolve(contact,f2,f1,impulse);
		}
	}
}


