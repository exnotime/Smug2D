#include "if_animation.h"
#include "SpriteAnimation.h"
#include <angelscript.h>
using namespace AngelScript;

SpriteAnimation::AnimationHandle LoadAnimation(const std::string& filename) {
	return SpriteAnimation::Load(filename.c_str());
}

int32_t GetAnimationIndex(SpriteAnimation::AnimationHandle handle, const std::string& name) {
	SpriteAnimation::SpriteAnimationResource* res = SpriteAnimation::GetAnimation(handle);
	if (!res) {
		return -1;
	}
	uint32_t i = 0;
	for (auto& anim : res->animations) {
		if (anim.name == name) {
			return i;
		}
		++i;
	}
	return -1;
}

void if_animation::LoadAnimationInterface(asIScriptEngine* engine) {
	engine->RegisterTypedef("AnimationHandle", "uint");
	engine->RegisterGlobalFunction("AnimationHandle LoadAnimation(const string filename)", asFUNCTION(LoadAnimation), asCALL_CDECL);
	engine->RegisterGlobalFunction("int GetAnimationIndex(AnimationHandle, const string name)", asFUNCTION(GetAnimationIndex), asCALL_CDECL);
}