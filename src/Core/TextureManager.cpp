#include "TextureManager.h"
#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

TextureManager::TextureManager()
{
	m_MissingTexture.texture.loadFromFile("assets/textures/missing_texture.png");
}

TextureManager::~TextureManager()
{
}

TextureManager& TextureManager::GetInstance()
{
	static TextureManager* instance = nullptr;
	if (!instance) {
		instance = new TextureManager();
	}
	return *instance;
}

sf::Texture* TextureManager::LoadTexture(const std::string& filename)
{
	auto& texIt = m_TextureMap.find(filename);
	if (texIt == m_TextureMap.end()) {
		Texture& tex = m_TextureMap[filename];
		if (tex.texture.loadFromFile(filename)){
			tex.filename = filename;
			struct stat fileStat;
			stat(filename.c_str(), &fileStat);
			tex.modifiedtime = fileStat.st_mtime;
			return &tex.texture;
		} else {
			return &m_MissingTexture.texture;
		}
	} else {
		return &texIt->second.texture;
	}
}

void TextureManager::UnloadTexture()
{
	//m_TextureMap.erase(texture.filename);
}

void TextureManager::Reload()
{
	for (auto& tex : m_TextureMap) {
		struct stat fileStat;
		stat(tex.second.filename.c_str(), &fileStat);
		if (tex.second.modifiedtime < fileStat.st_mtime) {
			tex.second.texture.loadFromFile(tex.second.filename);
		}
	}
}
