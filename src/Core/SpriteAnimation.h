#pragma once
#include "if_math.h"
#include "TextureManager.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <if_render.h>

namespace SpriteAnimation {

	struct  Transform{
		Vec2 position;
		Vec2 scale;
		Vec2 localOrigin;
		float rotation;
		float padding;
	};

	struct Node {
		Transform transform;
		uint32_t texture;
		uint32_t frame;
		uint32_t parent;
		std::vector<uint32_t> children;
		std::string name;
	};

	enum class KeyType {
		Transform,
		Frame,
		Texture,
		Tint
	};

	enum class BlendMode {
		Set,
		Lerp
	};

	struct AnimationKey {
		KeyType type;
		uint32_t nodeIndex;
		float time;
		BlendMode mode;
		//depending on the key type we either use the transform or index
		Transform transform;
		uint32_t index;
	};

	struct Animation {
		std::string name;
		float duration;
		std::vector<uint32_t> keys;
	};

	struct SpriteAnimationResource{
		std::string name;
		std::vector<sf::Texture*> textures;
		std::vector<Rect> frames;
		std::vector<uint32_t> roots;
		std::vector<Node> nodes;
		std::vector<AnimationKey> keys;
		std::vector<Animation> animations;
		//data-structure to speed up run time sampling
		std::unordered_map<uint32_t, std::vector<AnimationKey>> nodeKeys;
	};

	typedef uint32_t AnimationHandle;

	//holds the current state of an instance while the SpriteAnimation holds the data from disk in the initial state
	struct SpriteAnimationInstance {
		AnimationHandle handle;
		uint32_t animationIndex = 0;
		float time = 0; //last sampled time
		std::vector<Node> nodes;
		bool looping = true; //Should be part of the animation file weather or not the animation supports looping
		bool paused = false;
		bool backwards = false;
	};

	
	AnimationHandle Load(const char* filename);
	SpriteAnimationResource* GetAnimation(AnimationHandle handle);
	uint32_t InstansiateAnimation(AnimationHandle handle, uint32_t index = 0);
	void Update(float dt);
	void SampleAnimation(SpriteAnimationInstance& instance);
	SpriteAnimationInstance& GetInstance(uint32_t handle);
	//temp
	void RenderAnimatedSprite(sf::RenderWindow* window, SpriteAnimationInstance& instance);


}