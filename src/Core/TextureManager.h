#pragma once
#include <string>
#include <map>
#include <SFML/Graphics.hpp>

struct Texture {
	sf::Texture texture;
	time_t modifiedtime;
	std::string filename;
};

class TextureManager {
public:
	TextureManager();
	~TextureManager();
	static TextureManager& GetInstance();
	sf::Texture* LoadTexture(const std::string& filename);
	void UnloadTexture(Texture& texture);
	void Reload();
private:
	std::map<std::string, Texture> m_TextureMap;
	Texture m_MissingTexture;
};