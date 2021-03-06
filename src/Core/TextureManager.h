#pragma once
#include <string>
#include <map>
#include <SFML/Graphics.hpp>



class TextureManager {
public:
	TextureManager();
	~TextureManager();
	static TextureManager& GetInstance();
	sf::Texture* LoadTexture(const std::string& filename);
	sf::RenderTexture* CreateRenderTexture(const std::string& name, float width, float height);
	void UnloadTexture();
	void Reload();
private:
	struct Texture {
		sf::Texture texture;
		time_t modifiedtime;
		std::string filename;
	};

	std::map<std::string, Texture> m_TextureMap;
	std::map<std::string, sf::RenderTexture> m_RenderTextureMap;
	Texture m_MissingTexture;
};