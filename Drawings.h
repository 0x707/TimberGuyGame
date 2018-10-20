#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

// ========================= UTILITIES =========================
namespace {
	Texture texture_factory(char const* path)
	{
		Texture texture;
		texture.loadFromFile(path);
		return texture;
	}

	Texture const TEXTURES[]{
		texture_factory("graphics/axe.png"),
		texture_factory("graphics/background.png"),
		texture_factory("graphics/bee.png"),
		texture_factory("graphics/branch.png"),
		texture_factory("graphics/cloud.png"),
		texture_factory("graphics/log.png"),
		texture_factory("graphics/player.png"),
		texture_factory("graphics/rip.png"),
		texture_factory("graphics/tree.png"),
	};

	enum class TEXTURE {
		AXE, BACKGROUND, BEE, BRANCH,
		CLOUD, LOG, PLAYER, RIP, TREE
	};
	enum class side { LEFT, RIGHT, NONE };

	constexpr int NUM_CLOUDS = 7;
	constexpr int NUM_BRANCHES = 6;
	float const AXE_POSITION_LEFT = 700.0f;
	float const AXE_POSITION_RIGHT = 1075.0f;

	side branch_position[NUM_BRANCHES];
}

// ========================= STRUCTURES =========================

struct SoundObj
{
public:
	SoundObj(char const* path)
	{
		buffer_.loadFromFile(path);
		sound_.setBuffer(buffer_);
	}

	void play() { sound_.play(); }
private:
	SoundBuffer buffer_;
	Sound sound_;
};

struct SpriteObj
{
public:
	SpriteObj() {}
	SpriteObj(TEXTURE texture)
	{
		sprite_.setTexture(TEXTURES[static_cast<int>(texture)]);
	}

	void setPosition(float x, float y)
	{
		sprite_.setPosition(x, y);
	}

	sf::Vector2f const& getPosition() const { return sprite_.getPosition(); }

	void setOrigin(float x, float y) { sprite_.setOrigin(x, y); }
	void setRotation(float angle) { sprite_.setRotation(angle); }
	void rotate(float angle) { sprite_.rotate(angle); }

	Sprite const& getSprite() const { return sprite_; }
private:
	Sprite sprite_;
};

struct Bee : public SpriteObj
{
public:
	Bee(TEXTURE texture)
		: SpriteObj{ texture }
	{
	}

	bool is_active_ = false;
	float speed_ = 0.0f;
};

struct Cloud : public SpriteObj
{
	Cloud(TEXTURE texture, float x, float y, int modifier = 0)
		: SpriteObj{ texture }, seed_modifier_{ modifier }
	{
		setPosition(x, y);
	}

	float speed_ = 0.0f;
	bool active_ = false;
	int seed_modifier_;
};

struct Log : public SpriteObj
{
	Log(TEXTURE texture)
		: SpriteObj{ texture }
	{
	}

	bool is_active_ = false;
	int speedX_ = 1000;
	int speedY_ = -1500;
};

struct Player : public SpriteObj
{
	Player(TEXTURE texture)
		: SpriteObj{ texture }
	{
	}

	side side_ = side::LEFT;
};

struct Rectangle
{
	Rectangle(float w, float h, Color c)
		: rect_{ Vector2f{w,h} }, height_{ h }, startWidth_{ w }
	{
		rect_.setFillColor(c);
	}

	void setFillColor(Color c) { rect_.setFillColor(c); }
	void setOutlineColor(Color c) { rect_.setOutlineColor(c); }
	void setPosition(float x, float y) { rect_.setPosition(x, y); }
	void setSize(float x, float y) { rect_.setSize(Vector2f{ x,y }); }
	void setThickness(float x) { rect_.setOutlineThickness(x); }

	float getStartWidth() const { return startWidth_; }
	float getHeight() const { return height_; }
	RectangleShape getRect() const { return rect_; }
private:
	RectangleShape rect_;
	float height_;
	float startWidth_;
};
