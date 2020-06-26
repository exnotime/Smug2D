#include "if_render.h"
#include <angelscript.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include "sprite.h"
#include "if_math.h"
#include "TextureManager.h"
#include "EntityManager.h"
#include "Components.h"
#include "ComponentManager.h"
#include "Camera.h"
#include <algorithm>

using namespace AngelScript;

#define DEFAULT_LAYER 3
uint32_t g_SpriteId = 0;

struct Text {
	std::string str;
	sf::Font* font;
	sf::Color color;
	Vec2 position;
	int charSize;
	int layer;
};

struct CircleShape {
	Vec2 position;
	sf::Color color;
	float radius;
	int layer;
};

sf::RenderWindow* g_Window = nullptr;
std::vector<Sprite*> g_Sprites;
std::vector<Sprite> g_ImmediateSprites;
std::vector<Camera> g_Cameras;
std::vector<Text> g_ImmediateTexts;
std::vector<CircleShape> g_ImmediateCircles;

void SortSprites() {
	std::sort(g_Sprites.begin(), g_Sprites.end(), [](const Sprite* s1, const Sprite* s2) -> bool { return s1->layer < s2->layer; });
}

void ClearWindow(int r, int g, int b) {
	if (g_Window) {
		g_Window->clear(sf::Color(r, g, b, 255));
	}
}

sf::Texture* LoadTexture(const std::string& filename) {
	return TextureManager::GetInstance().LoadTexture(filename);
}

void UnloadTexture(sf::Texture* tex) {
	//TODO: Update to use the texture manager
	if (tex) {
		delete tex;
	}
}
//quick function for drawing a sprite
void DrawSprite(sf::Texture* tex, const Vec2 pos, const Vec2 scale, const int layer) {
	Sprite spr;
	spr.texture = tex;
	spr.position = pos;
	spr.scale = scale;
	spr.layer = layer;
	spr.tint = sf::Color::White;
	const sf::Vector2u size = spr.texture->getSize();
	spr.textureRect = { 0.0f, 0.0f, (float)size.x, (float)size.y };
	spr.rotation = 0;
	spr.localOrigin = Vec2(0, 0);
	g_ImmediateSprites.push_back(spr);
}

//Creates a permanent sprite
Sprite* CreateSprite(sf::Texture* texture, const Vec2 pos) {
	Sprite* spr = new Sprite();
	spr->texture = texture;
	sf::Vector2u size = spr->texture->getSize();
	spr->textureSize.x = size.x;
	spr->textureSize.y = size.y;
	spr->position = pos;
	spr->layer = DEFAULT_LAYER;
	spr->tint = sf::Color::White;
	spr->textureRect = { 0.0f, 0.0f, (float)size.x, (float)size.y };
	spr->scale = { 1.0f, 1.0f };
	spr->id = g_SpriteId++;
	g_Sprites.push_back(spr);
	SortSprites();
	return spr;
}

void DestroySprite(Sprite* spr) {
	for (int i = 0; i < g_Sprites.size(); ++i) {
		if (g_Sprites[i]->id == spr->id) {
			delete spr;
			g_Sprites.erase(g_Sprites.begin() + i);
			return;
		}
	}
}

sf::Font* LoadFont(const std::string& filename) {
	sf::Font* font = new sf::Font();
	bool s = font->loadFromFile(filename);
	return font;
}

void UnloadFont(sf::Font* font) {
	delete font;
}

void DrawText(sf::Font* font, const std::string& string, const Vec2 position, const sf::Color color, const int charSize, const int layer) {
	Text text;
	text.font = font;
	text.position = position;
	text.str = string;
	text.charSize = charSize;
	text.layer = layer;
	text.color = color;
	g_ImmediateTexts.push_back(text);
}

void DrawCircle(Vec2 position, sf::Color color, float radius, int layer) {
	CircleShape cs;
	cs.position = position;
	cs.color = color;
	cs.radius = radius;
	cs.layer = layer;
	g_ImmediateCircles.push_back(cs);
}

void SetVsync(bool v) {
	g_Window->setVerticalSyncEnabled(v);
}

void ConstructColor(int r, int g, int b, void* memory) {
	new(memory) sf::Color(r,g,b);
}

void ConstructColorAlpha(int r, int g, int b, int a, void* memory) {
	new(memory) sf::Color(r, g, b, a);
}

void AddCamera(Camera& c) {
	g_Cameras.push_back(c);
}

Camera* ConstructCamera() {
	return new Camera();
}

sf::RenderTexture* CreateRenderTexture(const std::string name, Vec2 size) {
	return TextureManager::GetInstance().CreateRenderTexture(name, size.x, size.y);
}

void ClearRenderTexture(sf::RenderTexture* target, const sf::Color c) {
	target->clear(c);
}

void DrawSpriteToTexture(sf::RenderTexture* target, sf::Texture* tex, Vec2 position, Vec2 scale, Vec2 texpos, Vec2 texSize) {
	sf::Sprite spr = sf::Sprite();
	spr.setTexture(*tex);
	spr.setPosition(position.x, position.y);
	spr.setScale(scale.x, scale.y);
	spr.setTextureRect(sf::IntRect(texpos.x, texpos.y, texSize.x, texSize.y));
	target->draw(spr);
}

void DrawRectToTexture(sf::RenderTexture* target, Vec2 position, Vec2 size, sf::Color color) {
	sf::RectangleShape rect;
	rect.setFillColor(color);
	rect.setPosition(position.x, position.y);
	rect.setSize(sf::Vector2f(size.x, size.y));
	target->draw(rect);
}

void DrawTextToTexture(sf::RenderTexture* target, sf::Font* font, Vec2 position, std::string& str, const int charSize, const sf::Color color) {
	sf::Text text;
	text.setCharacterSize(charSize);
	text.setColor(color);
	text.setFont(*font);
	text.setPosition(position.x, position.y);
	text.setString(str);
	target->draw(text);
}

void FlushRenderTexture(sf::RenderTexture* target) {
	target->display();
}

const sf::Texture* GetTextureFromRenderTexture(sf::RenderTexture* tex) {
	return &tex->getTexture();
}

void if_render::LoadRenderInterface(AngelScript::asIScriptEngine* engine) {
	int r = 0;
	engine->RegisterGlobalFunction("void ClearWindow(int r, int g, int b)", asFUNCTION(ClearWindow), asCALL_CDECL);
	engine->RegisterObjectType("Texture", sizeof(sf::Texture*), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterGlobalFunction("Texture@ LoadTexture(const string)", asFUNCTION(LoadTexture), asCALL_CDECL);
	engine->RegisterGlobalFunction("void UnloadTexture(Texture@ tex)", asFUNCTION(UnloadTexture), asCALL_CDECL);
	engine->RegisterGlobalFunction("void DrawSprite(const Texture@ tex, const Vec2 pos, const Vec2 scale, const int layer = 3)", asFUNCTION(DrawSprite), asCALL_CDECL);
	engine->RegisterObjectType("Color", sizeof(sf::Color), asOBJ_POD | asOBJ_VALUE | asGetTypeTraits<sf::Color>());
	engine->RegisterObjectProperty("Color", "uint8 r", asOFFSET(sf::Color, r));
	engine->RegisterObjectProperty("Color", "uint8 g", asOFFSET(sf::Color, g));
	engine->RegisterObjectProperty("Color", "uint8 b", asOFFSET(sf::Color, b));
	engine->RegisterObjectProperty("Color", "uint8 a", asOFFSET(sf::Color, a));
	engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(int r, int g, int b)", asFUNCTION(ConstructColor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(int r, int g, int b, int a)", asFUNCTION(ConstructColorAlpha), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectType("Sprite", sizeof(Sprite), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("Sprite", "Texture@ tex", asOFFSET(Sprite, texture));
	engine->RegisterObjectProperty("Sprite", "Vec2 pos", asOFFSET(Sprite, position));
	engine->RegisterObjectProperty("Sprite", "Vec2 scale", asOFFSET(Sprite, scale));
	engine->RegisterObjectProperty("Sprite", "Rect textureRect", asOFFSET(Sprite, textureRect));
	engine->RegisterObjectProperty("Sprite", "Vec2 textureSize", asOFFSET(Sprite, textureSize));
	engine->RegisterObjectProperty("Sprite", "Color tint", asOFFSET(Sprite, tint));
	engine->RegisterObjectProperty("Sprite", "uint layer", asOFFSET(Sprite, layer));
	engine->RegisterObjectProperty("Sprite", "bool flipX", asOFFSET(Sprite, flipX));
	engine->RegisterObjectProperty("Sprite", "bool flipY", asOFFSET(Sprite, flipY));
	engine->RegisterGlobalFunction("Sprite@ CreateSprite(Texture@ tex, const Vec2 pos)", asFUNCTION(CreateSprite), asCALL_CDECL);
	engine->RegisterGlobalFunction("void DestroySprite(Sprite@ spr)", asFUNCTION(DestroySprite), asCALL_CDECL);
	engine->RegisterObjectType("Font", sizeof(sf::Font*), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterGlobalFunction("Font@ LoadFont(const string filename)", asFUNCTION(LoadFont), asCALL_CDECL);
	engine->RegisterGlobalFunction("void UnloadFont(Font@ font)", asFUNCTION(UnloadFont), asCALL_CDECL);
	engine->RegisterGlobalFunction("void DrawText(Font@ font, const string str, const Vec2 pos, const Color c, const int charSize = 30, const int layer = 2)", asFUNCTION(DrawText), asCALL_CDECL);
	engine->RegisterGlobalFunction("void DrawCircle(Vec2 position, Color color, float radius, int layer = 3)", asFUNCTION(DrawCircle), asCALL_CDECL);
	engine->RegisterGlobalFunction("void SetVsync(bool v)", asFUNCTION(SetVsync), asCALL_CDECL);
	r = engine->RegisterObjectType("Camera", 0, asOBJ_REF);
	r = engine->RegisterObjectBehaviour("Camera", asBEHAVE_FACTORY, "Camera@ f()", asFUNCTION(ConstructCamera), asCALL_CDECL);
	r = engine->RegisterObjectBehaviour("Camera", asBEHAVE_ADDREF, "void f()", asMETHOD(Camera, Increment), asCALL_THISCALL);
	r = engine->RegisterObjectBehaviour("Camera", asBEHAVE_RELEASE, "void f()", asMETHOD(Camera, Release), asCALL_THISCALL);
	r = engine->RegisterObjectProperty("Camera", "Vec2 position", asOFFSET(Camera, position));
	r = engine->RegisterObjectProperty("Camera", "Vec2 size", asOFFSET(Camera, size));
	r = engine->RegisterObjectProperty("Camera", "float rotation", asOFFSET(Camera, rotation));
	r = engine->RegisterObjectProperty("Camera", "Rect viewport", asOFFSET(Camera, viewport));
	r = engine->RegisterGlobalFunction("void AddCamera(Camera@ c)", asFUNCTION(AddCamera), asCALL_CDECL);
	

	r = engine->RegisterObjectType("RenderTexture", sizeof(sf::RenderTexture*), asOBJ_REF | asOBJ_NOCOUNT);
	r = engine->RegisterGlobalFunction("RenderTexture@ CreateRenderTexture(const string name, Vec2 size)", asFUNCTION(CreateRenderTexture), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("void ClearRenderTexture(RenderTexture@ target, const Color c)", asFUNCTION(ClearRenderTexture), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("void FlushRenderTexture(RenderTexture@ target)", asFUNCTION(FlushRenderTexture), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("void DrawSpriteToTexture(RenderTexture@ target, Texture@ spriteTex, Vec2 position, Vec2 scale, Vec2 texPos, Vec2 texSize)", asFUNCTION(DrawSpriteToTexture), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("void DrawRectToTexture(RenderTexture@ target, Vec2 position, Vec2 size, const Color c)", asFUNCTION(DrawRectToTexture), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("void DrawTextToTexture(RenderTexture@ target,Font@ font, Vec2 position, string str, const int charSize, const Color c)", asFUNCTION(DrawTextToTexture), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("const Texture@ GetTextureFromRenderTexture(RenderTexture@ target)", asFUNCTION(GetTextureFromRenderTexture), asCALL_CDECL);
}

void if_render::SetWindow(sf::RenderWindow* window) {
	g_Window = window;
}

void DrawSpriteToWindow(const Sprite* s) {
	sf::Sprite spr;
	spr.setColor(s->tint);
	spr.setTexture(*s->texture);
	spr.setTextureRect(sf::IntRect(s->textureRect.x, s->textureRect.y, s->textureRect.w, s->textureRect.h));
	spr.setPosition(sf::Vector2f(s->position.x, s->position.y));
	spr.setScale(s->scale.x, s->scale.y);
	spr.setRotation(s->rotation);
	spr.setOrigin(s->localOrigin.x, s->localOrigin.y);
	sf::Vector2f scale = spr.getScale();
	sf::FloatRect localBounds = spr.getLocalBounds();
	sf::Vector2f origin = spr.getOrigin();
	if (s->flipX) {
		origin.x = localBounds.width;
		scale.x *= -1;
	}
	if (s->flipY) {
		origin.y = localBounds.height;
		scale.y *= -1;
	}
	spr.setOrigin(origin);
	spr.setScale(scale);
	g_Window->draw(spr);
}

void DrawTextToWindow(const Text* t) {
	sf::Text text;
	text.setColor(t->color);
	text.setString(t->str);
	text.setFont(*t->font);
	text.setPosition(sf::Vector2f(t->position.x, t->position.y));
	text.setCharacterSize(t->charSize);
	g_Window->draw(text);
}


void if_render::Render() {
	//for now just render all sprite components as immediate mode sprites
	EntityManager& em = EntityManager::GetInstance();
	ComponentManager& cm = ComponentManager::GetInstance();
	auto& entities = em.GetEntityList();
	for (auto& e : entities) {
		int flags = ComponentType::TRANSFORM | ComponentType::SPRITE;
		if (e.ComponentBitfield & flags) {
			TransformComponent* tc = (TransformComponent*)cm.GetComponent(e, ComponentType::TRANSFORM);
			SpriteComponent* sc = (SpriteComponent*)cm.GetComponent(e, ComponentType::SPRITE);

			Sprite spr;
			spr.position = tc->position; 
			spr.layer = sc->layer;
			spr.scale = tc->scale;
			spr.texture = sc->texture;
			spr.textureRect = sc->textureRect;
			spr.tint = sc->tint;
			spr.localOrigin = tc->localOrigin;
			spr.rotation = tc->rotation;
			spr.flipX = sc->flipX;
			spr.flipY = sc->flipY;

			if (e.ComponentBitfield & ComponentType::ANIMATION) {
				AnimationComponent* ac = (AnimationComponent*)cm.GetComponent(e, ComponentType::ANIMATION);
				const SpriteAnimation::SpriteAnimationInstance& instance = SpriteAnimation::GetInstance(ac->instance);

				spr.scale *= instance.nodes[0].transform.scale;
				spr.position += instance.nodes[0].transform.position;
				spr.localOrigin += instance.nodes[0].transform.localOrigin;
			}

			g_ImmediateSprites.push_back(spr);
		}
	}
	std::sort(g_ImmediateTexts.begin(), g_ImmediateTexts.end(), [](const Text& t1, const Text& t2) ->bool { return t1.layer < t2.layer; });
	std::sort(g_ImmediateSprites.begin(), g_ImmediateSprites.end(), [](const Sprite& s1, const Sprite& s2) ->bool { return s1.layer < s2.layer; });
	std::sort(g_ImmediateCircles.begin(), g_ImmediateCircles.end(), [](const CircleShape& s1, const CircleShape& s2) ->bool { return s1.layer < s2.layer; });

	for (auto& camera : g_Cameras) {
		sf::View view;
		view.setSize(camera.size.x, camera.size.y);
		view.setRotation(camera.rotation);
		view.setCenter(camera.position.x + camera.size.x * 0.5f, camera.position.y + camera.size.y * 0.5f);
		view.setViewport(sf::FloatRect(camera.viewport.x, camera.viewport.y, camera.viewport.w, camera.viewport.h));
		g_Window->setView(view);

		int currentSprite = 0;
		int currentImmSprite = 0;
		int currentImmText = 0;
		int currentCircle = 0;
		for (uint32_t l = 0; l < 10 + 1; ++l) {
			for (int i = currentSprite; i < g_Sprites.size(); ++i, currentSprite++) {
				if (g_Sprites[i]->layer <= l) {
					DrawSpriteToWindow(g_Sprites[i]);
				}
				else {
					break;
				}
			}
			for (int i = currentImmSprite; i < g_ImmediateSprites.size(); ++i, currentImmSprite++) {
				if (g_ImmediateSprites[i].layer <= l) {
					DrawSpriteToWindow(&g_ImmediateSprites[i]);
				}
				else {
					break;
				}
			}
			for (int i = currentImmText; i < g_ImmediateTexts.size(); ++i, currentImmText++) {
				if (g_ImmediateTexts[i].layer <= l) {
					DrawTextToWindow(&g_ImmediateTexts[i]);
				}
				else {
					break;
				}
			}
			for (int i = currentCircle; i < g_ImmediateCircles.size(); ++i, currentCircle++) {
				if (g_ImmediateCircles[i].layer <= l) {
					const CircleShape& cs = g_ImmediateCircles[i];
					sf::CircleShape circle;
					circle.setFillColor(cs.color);
					circle.setPosition(cs.position.x, cs.position.y);
					circle.setRadius(cs.radius);
					g_Window->draw(circle);
				}
				else {
					break;
				}
			}
		}
	}
	g_Cameras.clear();
	g_ImmediateTexts.clear();
	g_ImmediateSprites.clear();
	g_ImmediateCircles.clear();
	
}