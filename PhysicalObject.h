#ifndef _PHYSICAL_OBJECT_H_
#define _PHYSICAL_OBJECT_H_

#include "BaseObject.h"

#include "Texture2D.h"
#include <vector>

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
	virtual bool        Update(const float deltaTime, const Vector2D playerPosition) override;

	const Vector2D      GetCollisionBox() const { return mCollisionBox; }

	void                RenderSprite(const Vector2D renderReferencePoint, const unsigned int currentFrameID);

protected:
	// Non-instance specific data
	static std::vector<Texture2D*>   mSpriteSheets;
	static std::vector<unsigned int> mInstanceCounts;
	static SDL_Renderer*             mRenderer;

	unsigned int					 mSingleSpriteWidth;
	unsigned int					 mSingleSpriteHeight;

	const unsigned int				 mSpritesOnWidth;
	const unsigned int				 mSpritesOnHeight;

	Texture2D*						 mThisSpriteSheet;
	const Vector2D					 mCollisionBox;
};

#endif // !Physical Object
