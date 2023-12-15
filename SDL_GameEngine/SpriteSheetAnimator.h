#pragma once
#include <vector>
#include <map>

namespace Engine
{
	struct SpriteSheetAnimation
	{
		//vector of index of sprite and the time lasting in animation
		std::vector<std::pair<double, int>> frames;
		bool repeat;
	};
	struct SpriteSheetAnimator_Component
	{
		std::map<const char*, SpriteSheetAnimation> animationDatas;

		bool stop;

		const char* currentAnimName;
		SpriteSheetAnimation* currentAnimData;
		int currentAnimFrame;
		double currentFrameTime;
		double currentAnimTime; //How long is the animation going

		void Play(const char* animationName, bool fromStart);
		class SpriteSheet_Component* spr;
	};

	class SpriteSheetAnimator_System
	{
	public:
		void Init(class Registry* reg);
		void Update(class Registry* reg, double* deltaTime);
	};
}