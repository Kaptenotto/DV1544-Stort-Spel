#ifndef ENEMY_NECROMANCER_H
#define ENEMY_NECROMANCER_H

#include <AI\Enemy.h>
#include <Player\Player.h>

namespace Logic
{
	class EnemyNecromancer : public Enemy
	{
		private:
			const int SPEED_AB1 = 75, SPEED_AB2 = 100;
		public:
			EnemyNecromancer(Graphics::ModelID modelID, btRigidBody* body, btVector3 halfExtent);
			virtual ~EnemyNecromancer();

			virtual void clear();
      
			virtual void onCollision(PhysicsObject& other, btVector3 contactPoint, float dmgMultiplier);
			virtual void onCollision(Player& other);

			virtual void updateSpecific(Player const &player, float deltaTime);
			virtual void updateDead(float deltaTime);

			virtual void useAbility(Entity const &target);
	};
}

#endif