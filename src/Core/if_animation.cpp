#include "if_animation.h"
#include "SpriteAnimation.h"
#include <angelscript.h>
using namespace AngelScript;

SpriteAnimation::AnimationHandle LoadAnimation(const std::string& filename) {
	return SpriteAnimation::Load(filename.c_str());
}

void if_animation::LoadAnimationInterface(asIScriptEngine* engine) {
	engine->RegisterTypedef("AnimationHandle", "uint");
	engine->RegisterGlobalFunction("AnimationHandle LoadAnimation(const string filename)", asFUNCTION(LoadAnimation), asCALL_CDECL);
}