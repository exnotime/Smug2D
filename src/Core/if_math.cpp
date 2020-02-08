#include "if_math.h"
#include <angelscript.h>
#include <new>
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

void if_math::LoadMathInterface(asIScriptEngine* engine) {
	int r = engine->RegisterObjectType("Vec2", sizeof(Vec2), asOBJ_POD | asOBJ_VALUE | asGetTypeTraits<Vec2>());
	r = engine->RegisterObjectProperty("Vec2", "float x", asOFFSET(Vec2, x));
	r = engine->RegisterObjectProperty("Vec2", "float y", asOFFSET(Vec2, y));
	r = engine->RegisterObjectBehaviour("Vec2", asBEHAVE_CONSTRUCT, "void f(float v)", asFUNCTION(ConstructVec2), asCALL_CDECL_OBJLAST);
	r = engine->RegisterObjectBehaviour("Vec2", asBEHAVE_CONSTRUCT, "void f_xy(float x, float y)", asFUNCTION(ConstructVec2XY), asCALL_CDECL_OBJLAST);
	r = engine->RegisterObjectMethod("Vec2", "Vec2 opSub(const Vec2) const", asMETHODPR(Vec2, operator-, (const Vec2) const, Vec2), asCALL_THISCALL);
	r = engine->RegisterObjectMethod("Vec2", "Vec2 opAdd(const Vec2) const", asMETHODPR(Vec2, operator+, (const Vec2) const, Vec2), asCALL_THISCALL);
	r = engine->RegisterObjectMethod("Vec2", "void opSubAssign(const Vec2)", asMETHODPR(Vec2, operator-=, (const Vec2), void), asCALL_THISCALL);
	r = engine->RegisterObjectMethod("Vec2", "void opAddAssign(const Vec2)", asMETHODPR(Vec2, operator+=, (const Vec2), void), asCALL_THISCALL);

	r = engine->RegisterObjectType("Rect", sizeof(Rect), asOBJ_POD | asOBJ_VALUE | asGetTypeTraits<Rect>());
	r = engine->RegisterObjectProperty("Rect", "float x", asOFFSET(Rect, x));
	r = engine->RegisterObjectProperty("Rect", "float y", asOFFSET(Rect, y));
	r = engine->RegisterObjectProperty("Rect", "float w", asOFFSET(Rect, w));
	r = engine->RegisterObjectProperty("Rect", "float h", asOFFSET(Rect, h));
	r = engine->RegisterObjectBehaviour("Rect", asBEHAVE_CONSTRUCT, "void f(float x, float y, float w, float h)", asFUNCTION(ConstructRect), asCALL_CDECL_OBJLAST);
}