#include "SpriteAnimation.h"


#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <stdio.h>
#include <math.h>

#include <TextureManager.h>
#include <ComponentManager.h>
#include <Components.h>
#include <EntityManager.h>

namespace SpriteAnimation {
	
	static std::unordered_map<AnimationHandle, SpriteAnimationResource> m_Animations;
	static uint32_t m_Numerator = 0;
	static std::unordered_map<AnimationHandle, SpriteAnimationInstance> m_AnimationInstances;
	static uint32_t m_InstanceNumerator = 0;

	Transform ParseTransform(const rapidjson::Value& val) {
		Transform t;
		if (val.IsObject()) {
			if (val.HasMember("position")) {
				const auto& pos = val["position"];
				t.position.x = pos[0].GetFloat();
				t.position.y = pos[1].GetFloat();
			}
			if (val.HasMember("rotation")) {
				t.rotation = val["rotation"].GetFloat();
			}
			if (val.HasMember("origin")) {
				const auto& origin = val["origin"];
				t.localOrigin.x = origin[0].GetFloat();
				t.localOrigin.y = origin[1].GetFloat();
			}
			if (val.HasMember("scale")) {
				const auto& scale = val["scale"];
				t.Scale.x = scale[0].GetFloat();
				t.Scale.y = scale[1].GetFloat();
			}
		}
		return t;
	}

	AnimationHandle Load(const char* filename) {
		AnimationHandle h = 0;
		FILE* fin = fopen(filename, "rb");
		if (fin) {
			fseek(fin, 0, SEEK_END);
			int size = ftell(fin);
			fseek(fin, 0, SEEK_SET);

			char* jsonString = (char*)malloc(size);
			if (jsonString) {
				
				rapidjson::Document document;
				rapidjson::FileReadStream stream(fin, jsonString, size);
				document.ParseStream(stream);
				SpriteAnimationResource sa;
				//load textures
				if (document.HasMember("textures")) {
					const rapidjson::Value& textures = document["textures"];
					for (auto& t : textures.GetArray()) {
						sa.textures.push_back(TextureManager::GetInstance().LoadTexture(t.GetString()));
					}
				}
				//load frames
				if (document.HasMember("frames")) {
					const rapidjson::Value& frames = document["frames"];
					for (auto& r : frames.GetArray()) {
						if (r.IsArray() && r.Capacity() == 4) {
							Rect rect;
							rect.x = r[0].GetFloat();
							rect.y = r[1].GetFloat();
							rect.w = r[2].GetFloat();
							rect.h = r[3].GetFloat();
							sa.frames.push_back(rect);
						}
					}
				}
				//load nodes
				if (document.HasMember("nodes")) {
					const rapidjson::Value& nodes = document["nodes"];
					for (auto& n : nodes.GetArray()) {
						if (n.IsObject()) {
							Node node;
							node.texture = n["texture"].GetInt();
							node.frame = n["frame"].GetInt();
							node.parent = n["parent"].GetInt();
							node.name = n["name"].GetString();
							if (n.HasMember("children")) {
								const auto& children = n["children"].GetArray();
								for (auto& ch : children) {
									node.children.push_back(ch.GetInt());
								}
							}
							node.transform = ParseTransform(n["transform"]);
							sa.nodes.push_back(node);
						}
					}
				}
				//load roots
				if (document.HasMember("roots")) {
					const rapidjson::Value& roots = document["roots"];
					for (auto& r : roots.GetArray()) {
						sa.roots.push_back(r.GetInt());
					}
				}
				//load keys
				if (document.HasMember("keys")) {
					const rapidjson::Value& keys = document["keys"];
					for (auto& k : keys.GetArray()) {
						if (k.IsObject()) {
							AnimationKey key;
							std::string type = k["type"].GetString();
							if (type == "frame") {
								key.type = KeyType::Frame;
							}
							else if (type == "transform") {
								key.type = KeyType::Transform;
							}
							else if (type == "texture") {
								key.type = KeyType::Texture;
							}

							key.nodeIndex = k["node"].GetInt();
							key.time = k["time"].GetFloat();
							std::string mode = k["mode"].GetString();
							if (mode == "set") {
								key.mode = BlendMode::Set;
							}
							else if (mode == "lerp") {
								key.mode = BlendMode::Lerp;
							}

							if (key.type == KeyType::Frame || key.type == KeyType::Texture) {
								key.index = k["value"].GetInt();
							}
							else if (key.type == KeyType::Transform) {
								key.transform = ParseTransform(k["transform"]);
							}
							sa.keys.push_back(key);
						}
					}
				}
				//load animations
				if (document.HasMember("animations")) {
					const rapidjson::Value& animations = document["animations"];
					for (auto& a : animations.GetArray()) {
						Animation animation;
						animation.name = a["name"].GetString();
						animation.duration = a["duration"].GetFloat();

						const auto& keys = a["keys"];
						for (auto& k : keys.GetArray()) {
							uint32_t keyIndex = k.GetInt();
							animation.keys.push_back(keyIndex);
						}
						sa.animations.push_back(animation);
					}
				}
				sa.name = filename;
				h = m_Numerator++;
				//create run-time structure
				for (auto& key : sa.keys) {
					sa.nodeKeys[key.nodeIndex].push_back(key);
				}
				//sort keys based on time
				for (auto& keys : sa.nodeKeys) {
					std::sort(keys.second.begin(), keys.second.end(), [](const AnimationKey& k1, const AnimationKey& k2) -> bool {
						return k1.time < k2.time;
					});
				}
				m_Animations[h] = sa;
			}
			fclose(fin);
		}
		return h;
	}

	uint32_t InstansiateAnimation(AnimationHandle handle, uint32_t index) {
		
		auto& animation = m_Animations.find(handle);
		if (animation != m_Animations.end()) {
			uint32_t instanceHandle = m_InstanceNumerator++;
			SpriteAnimationInstance& instance = m_AnimationInstances[instanceHandle];
			instance.animationIndex = index;
			assert(animation->second.animations.size() > index);
			instance.handle = instanceHandle;
			instance.time = 0;
			instance.nodes.insert(instance.nodes.begin(), animation->second.nodes.begin(), animation->second.nodes.end());
			return instanceHandle;
		}
		return -1;
	}

	void Update(float dt) {
		//update all animation components
		//for now just render all sprite components as immediate mode sprites
		EntityManager& em = EntityManager::GetInstance();
		ComponentManager& cm = ComponentManager::GetInstance();
		auto& entities = em.GetEntityList();
		for (auto& e : entities) {
			int flags = ComponentType::ANIMATION | ComponentType::SPRITE;
			if ((e.ComponentBitfield & flags) == flags) {
				AnimationComponent* ac = (AnimationComponent*)cm.GetComponent(e, ComponentType::ANIMATION);
				SpriteComponent* sc = (SpriteComponent*)cm.GetComponent(e, ComponentType::SPRITE);

				SpriteAnimationInstance& instance = m_AnimationInstances[ac->instance];
				instance.time += dt;
				SampleAnimation(instance.time, instance);
				//update sprite component based on animation component
				//TODO: handle multiple nodes, for now only take the sprite from the first node
				const SpriteAnimationResource& resource = m_Animations[instance.handle];
				const Node& node = instance.nodes[0];
				sc->texture = resource.textures[node.texture];
				sf::Vector2u texSize = sc->texture->getSize();
				sc->textureSize = Vec2(texSize.x, texSize.y);
				sc->textureRect = resource.frames[node.frame];
			}
		}
	}

	void SampleAnimation(float time, SpriteAnimationInstance& instance) {
		auto& animation = m_Animations.find(instance.handle);
		if (animation != m_Animations.end()) {
			const Animation& anim = animation->second.animations[instance.animationIndex];
			instance.time = fmodf(time, anim.duration);
			for (uint32_t nodeIndex = 0; nodeIndex < instance.nodes.size(); ++nodeIndex) {
				auto& keys =  animation->second.nodeKeys[nodeIndex];
				for (uint32_t keyIndex = 0; keyIndex < keys.size(); ++keyIndex) {
					if (keys[keyIndex].time > instance.time) {
						//lerp between next and last key based on blendmode
						auto& node = instance.nodes[nodeIndex];
						auto& lastKey = keys[keyIndex - 1];
						//TODO: atm only support frame animation
						if (lastKey.type == KeyType::Frame) {
							node.frame = lastKey.index;
						}
						break;
					}
				}
			}
		}
	}


	void RenderAnimatedSprite(sf::RenderWindow* window, SpriteAnimationInstance& instance) {
		auto& animation = m_Animations.find(instance.handle);
		if (animation != m_Animations.end()) {
			for (auto& node : instance.nodes) {
				sf::Sprite spr;
				spr.setTexture(*animation->second.textures[node.texture]);
				spr.setPosition(50, 50);
				Rect r = animation->second.frames[node.frame];
				spr.setTextureRect(sf::IntRect(r.x, r.y, r.w, r.h));
				spr.setScale(3, 3);
				window->draw(spr);
			}
		}
	}
}