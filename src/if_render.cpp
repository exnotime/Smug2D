#include "if_render.h"
#include <angelscript.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include "sprite.h"
#include "if_math.h"

using namespace AngelScript;
sf::RenderWindow* g_Window = nullptr;
std::vector<Sprite> g_Sprites;

void ClearWindow(int r, int g, int b) {
	if (g_Window) {
		g_Window->clear(sf::Color(r, g, b, 255));
	}
}

sf::Texture* LoadTexture(const std::string& filename) {
	sf::Texture* tex = new sf::Texture();
	tex->loadFromFile(filename);
	return tex;
}

void UnloadTexture(sf::Texture* tex) {
	if (tex) {
		delete tex;
	}
}
//quick function for drawing a sprite
void DrawSprite(const sf::Texture* tex, const Vec2 pos,const Vec2 scale) {
	sf::Sprite spr;
	spr.setTexture(*tex);
	spr.setPosition(sf::Vector2f(pos.x, pos.y));
	spr.setScale(scale.x, scale.y);
	g_Window->draw(spr);
}
//Creates a permanent sprite
Sprite* CreateSprite(sf::Texture* texture, const Vec2 pos) {
	Sprite spr;
	spr.texture = texture;
	sf::Vector2u size = spr.texture->getSize();
	spr.textureSize.x = size.x;
	spr.textureSize.y = size.y;
	spr.postion = pos;
	spr.textureRect = { 0.0f, 0.0f, (float)size.x, (float)size.y };
	spr.scale = { 1.0f, 1.0f };
	g_Sprites.push_back(spr);
	return &g_Sprites.back();
}

sf::Font* LoadFont(const std::string& filename) {
	sf::Font* font = new sf::Font();
	bool s = font->loadFromFile(filename);
	return font;
}

void UnloadFont(sf::Font* font) {
	delete font;
}

void DrawText(sf::Font* font, const std::string& string, const Vec2 position, const int charSize) {
	sf::Text text;
	text.setFont(*font);
	text.setPosition(position.x, position.y);
	text.setString(string);
	text.setColor(sf::Color::White);
	text.setCharacterSize(charSize);
	g_Window->draw(text);
}

void SetVsync(bool v) {
	g_Window->setVerticalSyncEnabled(v);
}


void if_render::LoadRenderInterface(AngelScript::asIScriptEngine* engine) {
	engine->RegisterGlobalFunction("void ClearWindow(int r, int g, int b)", asFUNCTION(ClearWindow), asCALL_CDECL);
	engine->RegisterObjectType("Texture", sizeof(sf::Texture*), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterGlobalFunction("Texture@ LoadTexture(const string)", asFUNCTION(LoadTexture), asCALL_CDECL);
	engine->RegisterGlobalFunction("void UnloadTexture(Texture@ tex)", asFUNCTION(UnloadTexture), asCALL_CDECL);
	engine->RegisterGlobalFunction("void DrawSprite(const Texture@ tex, const Vec2 pos, const Vec2 scale)", asFUNCTION(DrawSprite), asCALL_CDECL);
	engine->RegisterObjectType("Color", sizeof(sf::Color), asOBJ_POD | asOBJ_VALUE);
	engine->RegisterObjectProperty("Color", "uint8 r", asOFFSET(sf::Color, r));
	engine->RegisterObjectProperty("Color", "uint8 g", asOFFSET(sf::Color, g));
	engine->RegisterObjectProperty("Color", "uint8 b", asOFFSET(sf::Color, b));
	engine->RegisterObjectProperty("Color", "uint8 a", asOFFSET(sf::Color, a));
	engine->RegisterObjectType("Sprite", sizeof(Sprite), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("Sprite", "Texture@ tex", asOFFSET(Sprite, texture));
	engine->RegisterObjectProperty("Sprite", "Vec2 pos", asOFFSET(Sprite, postion));
	engine->RegisterObjectProperty("Sprite", "Vec2 scale", asOFFSET(Sprite, scale));
	engine->RegisterObjectProperty("Sprite", "Rect textureRect", asOFFSET(Sprite, textureRect));
	engine->RegisterObjectProperty("Sprite", "Vec2 textureSize", asOFFSET(Sprite, textureSize));
	engine->RegisterObjectProperty("Sprite", "Color tint", asOFFSET(Sprite, tint));
	engine->RegisterGlobalFunction("Sprite@ CreateSprite(Texture@ tex, const Vec2 pos)", asFUNCTION(CreateSprite), asCALL_CDECL);
	engine->RegisterObjectType("Font", sizeof(sf::Font*), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterGlobalFunction("Font@ LoadFont(const string filename)", asFUNCTION(LoadFont), asCALL_CDECL);
	engine->RegisterGlobalFunction("void UnloadFont(Font@ font)", asFUNCTION(UnloadFont), asCALL_CDECL);
	engine->RegisterGlobalFunction("void DrawText(Font@ font, const string str, const Vec2 pos, const int charSize = 30)", asFUNCTION(DrawText), asCALL_CDECL);
	engine->RegisterGlobalFunction("void SetVsync(bool v)", asFUNCTION(SetVsync), asCALL_CDECL);
}

void if_render::SetWindow(sf::RenderWindow* window) {
	g_Window = window;
}

void if_render::RenderSprites() {
	for (Sprite& s : g_Sprites) {
		sf::Sprite spr;
		spr.setColor(s.tint);
		spr.setTexture(*s.texture);
		spr.setTextureRect(sf::IntRect(s.textureRect.x, s.textureRect.y, s.textureRect.w, s.textureRect.h));
		spr.setPosition(sf::Vector2f(s.postion.x, s.postion.y));
		spr.setScale(s.scale.x, s.scale.y);
		g_Window->draw(spr);
	}
}