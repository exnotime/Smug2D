#include "if_math.h"
#include <angelscript.h>
#include <new>
#include <algorithm>
using namespace AngelScript;

void ConstructVec2( float v, void* memory) {
	new(memory) Vec2(v);
}
void ConstructVec2XY(float x, float y, void* memory) {
	new(memory) Vec2(x, y);
}

void ConstructRect(float x, float y, float w, float h, void* memory) {
	new(memory) Rect(x, y, w, h);
}

float RandomFloat() {
	return rand() / (float)INT_MAX;
}

int Random() {
	return rand();
}

int RandomRange(int min, int max) {
	int range = std::max(1, max - min);
	return min + rand() % range;
}

float MinFloat(float x, float y) {
	if (x < y) return x;
	return y;
}

float MaxFloat(float x, float y) {
	if (x > y) return x;
	return y;
}


void if_math::LoadMathInterface(asIScriptEngine* engine) {
	int r = engine->RegisterObjectType("Vec2", sizeof(Vec2), asOBJ_POD | asOBJ_VALUE | asGetTypeTraits<Vec2>());
	r = engine->RegisterObjectProperty("Vec2", "float x", asOFFSET(Vec2, x));
	r = engine->RegisterObjectProperty("Vec2", "float y", asOFFSET(Vec2, y));
	r = engine->RegisterObjectBehaviour("Vec2", asBEHAVE_CONSTRUCT, "void f(float v)", asFUNCTION(ConstructVec2), asCALL_CDECL_OBJLAST);
	r = engine->RegisterObjectBehaviour("Vec2", asBEHAVE_CONSTRUCT, "void f_xy(float x, float y)", asFUNCTION(ConstructVec2XY), asCALL_CDECL_OBJLAST);
	r = engine->RegisterObjectMethod("Vec2", "Vec2 opSub(const Vec2) const", asMETHODPR(Vec2, operator-, (const Vec2) const, Vec2), asCALL_THISCALL);
	r = engine->RegisterObjectMethod("Vec2", "Vec2 opAdd(const Vec2) const", asMETHODPR(Vec2, operator+, (const Vec2) const, Vec2), asCALL_THISCALL);
	r = engine->RegisterObjectMethod("Vec2", "Vec2 opMul(const Vec2) const", asMETHODPR(Vec2, operator*, (const Vec2) const, Vec2), asCALL_THISCALL);
	r = engine->RegisterObjectMethod("Vec2", "void opSubAssign(const Vec2)", asMETHODPR(Vec2, operator-=, (const Vec2), void), asCALL_THISCALL);
	r = engine->RegisterObjectMethod("Vec2", "void opAddAssign(const Vec2)", asMETHODPR(Vec2, operator+=, (const Vec2), void), asCALL_THISCALL);
	r = engine->RegisterObjectMethod("Vec2", "void opMulAssign(const Vec2)", asMETHODPR(Vec2, operator*=, (const Vec2), void), asCALL_THISCALL);

	r = engine->RegisterObjectType("Rect", sizeof(Rect), asOBJ_POD | asOBJ_VALUE | asGetTypeTraits<Rect>());
	r = engine->RegisterObjectProperty("Rect", "float x", asOFFSET(Rect, x));
	r = engine->RegisterObjectProperty("Rect", "float y", asOFFSET(Rect, y));
	r = engine->RegisterObjectProperty("Rect", "float w", asOFFSET(Rect, w));
	r = engine->RegisterObjectProperty("Rect", "float h", asOFFSET(Rect, h));
	r = engine->RegisterObjectBehaviour("Rect", asBEHAVE_CONSTRUCT, "void f(float x, float y, float w, float h)", asFUNCTION(ConstructRect), asCALL_CDECL_OBJLAST);

	r = engine->RegisterGlobalFunction("float RandomFloat()", asFUNCTION(RandomFloat), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("int Random()", asFUNCTION(Random), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("int Random(int min, int max)", asFUNCTION(RandomRange), asCALL_CDECL);

	r = engine->RegisterGlobalFunction("float MinFloat(float x, float y)", asFUNCTION(MinFloat), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("float MaxFloat(float x, float y)", asFUNCTION(MaxFloat), asCALL_CDECL);
}