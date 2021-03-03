#ifndef _PHYSICAL_OBJECT_H_
#define _PHYSICAL_OBJECT_H_

#include "BaseObject.h"

#include "Texture2D.h"
#include <vector>

class InteractableLayer;
class ObjectLayer;

#define MOVEMENT_SPEED 1.1f

// ------------------------------------------------------------------------------------------------- //

struct RenderData
{
	unsigned int& currentFrameID;
	unsigned int  startrameID;
	unsigned int  endFrameID;

	float&        timeRemainingTillFrameChange;
	float         timePerFrame;
};

// ------------------------------------------------------------------------------------------------- //

class PhysicalObject abstract : public BaseObject
{
public:
	PhysicalObject() = delete;
	PhysicalObject(const Vector2D      spawnPosition, 
		           const bool          startSpawnedInLevel, 
		                 SDL_Renderer* renderer, 
		           const std::string   filePathToSpriteSheet,  
				   const unsigned int  spritesOnWidth, 
		           const unsigned int  spritesOnHeight,
		           const double        collisionBoxWidth,
		           const double        collisionBoxHeight,
		           const float		   timePerFrame);
	virtual ~PhysicalObject() override;

	virtual BaseObject* Clone(std::string dataForNewObject) = 0;

	virtual void        Render(const Vector2D renderReferencePoint) override { ; }
	virtual bool        Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer) override;

	virtual const Vector2D      GetCollisionBox() const { return mCollisionBox; }

	void                RenderSprite(const Vector2D renderReferencePoint, const unsigned int currentFrameID);

	virtual bool        GetHasUpdatedStaticVariables() { return false; }

protected:
	void                 UpdateStaticVariables(const float deltaTime);
	virtual RenderData   GetRenderData() = 0;

	void HandleMovement(const float deltaTime, InteractableLayer* interactableLayer, ObjectLayer* objectLayer);

	// Gravity application
	void ApplyGravity(const float deltaTime);

	// Non-instance specific data
	static std::vector<Texture2D*>   mSpriteSheets;
	static std::vector<unsigned int> mInstanceCounts;
	static SDL_Renderer*             mRenderer;

	Vector2D                         mVelocity;

	unsigned int					 mSingleSpriteWidth;
	unsigned int					 mSingleSpriteHeight;

	const unsigned int				 mSpritesOnWidth;
	const unsigned int				 mSpritesOnHeight;

	Texture2D*						 mThisSpriteSheet;
	const Vector2D					 mCollisionBox;

	bool                             mFacingLeft;
	bool                             mGrounded;

private:
	// Handlers
	bool HandleXCollision(const float deltaTime, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);
	bool HandleYCollision(const float deltaTime, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);

	// Actual checks in each axis
	bool CheckXCollision(const Vector2D headPos, const Vector2D footPos, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);
	bool CheckYCollision(const Vector2D leftPos, const Vector2D rightPos, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);

	// Actual interaction layer check
	bool CheckCollisionsWithInteractionLayer(InteractableLayer* interactionLayer, const Vector2D positionToCheck);
	MovementPrevention CheckForCollisionWithObjectLayer(ObjectLayer* objectLayer,               const Vector2D positionToCheck);
};

#endif // !Physical Object
