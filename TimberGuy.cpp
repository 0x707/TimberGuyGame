#include "pch.h"
#include "Drawings.h"
#include <sstream>
#include <iomanip>
using namespace sf;

void animateCloud(Cloud* cloud, Time const & t);
void setCentre(Text&);
void updateBranches(int);
template<typename T>
void updateValue(Text&, T);
Text initText(Font&, char const*, unsigned, Color);

int main()
{
	VideoMode vm{ 1920,1080 };
	RenderWindow window{ vm, "TimberRCWD", Style::Fullscreen };

	SpriteObj background{ TEXTURE::BACKGROUND };
	background.setPosition(0, 0);

	SpriteObj tree{ TEXTURE::TREE };
	tree.setPosition(810, 0);

	Bee bee{ TEXTURE::BEE };
	bee.setPosition(-70, 800);

	Cloud clouds[NUM_CLOUDS] {
		Cloud{TEXTURE::CLOUD,-300,0,10},
		Cloud{TEXTURE::CLOUD,-300,250,30},
		Cloud{TEXTURE::CLOUD,-300,500,40},
		Cloud{TEXTURE::CLOUD,-300,289,2},
		Cloud{TEXTURE::CLOUD,-300,302,32},
		Cloud{TEXTURE::CLOUD,-300,389,12},
		Cloud{TEXTURE::CLOUD,-300,411,42}
	};

	Clock clock;

	Rectangle timeBar{ 400,80,Color::Red };
	timeBar.setPosition((1920 / 2 - timeBar.getStartWidth() / 2), 980);

	Time gameTotalTime;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBar.getStartWidth() / timeRemaining;
	
	bool paused = true;

	int score = 0;

	Font font;
	font.loadFromFile("fonts/KOMIKAB_.ttf");

	Text scoreText{initText(font,"Score = 0", 100, Color::White)};
	scoreText.setPosition(20, 20);

	Text messageText{ initText(font,"Press enter to start!",75,Color::White) };
	setCentre(messageText);

	Text fpsText{ initText(font,"0",100,Color::White) };
	fpsText.setPosition(1220,20);

	Rectangle timeBarFrame{ 400,80,Color::Transparent };
	timeBarFrame.setOutlineColor(Color::Yellow);
	timeBarFrame.setThickness(7.0f);
	timeBarFrame.setPosition((1920 / 2 - timeBar.getStartWidth() / 2), 980);

	Color transparentBlack{ 0,0,0,130 };
	Rectangle scoreBackground{ 570,120,transparentBlack };
	scoreBackground.setPosition(12, 22);

	Rectangle fpsBackground{ 700, 120, transparentBlack };
	fpsBackground.setPosition(1212, 22);

	SpriteObj branches[NUM_BRANCHES];
	for (int i = 0; i < NUM_BRANCHES; ++i) {
		branches[i] = { TEXTURE::BRANCH };
		branches[i].setPosition(-2000, -2000);
		branches[i].setOrigin(220, 40);
	}

	Player player{ TEXTURE::PLAYER };
	player.setPosition(580, 720);

	SpriteObj gravestone{ TEXTURE::RIP };
	gravestone.setPosition(-600, 860);

	SpriteObj axe{ TEXTURE::AXE };
	axe.setPosition(-700, 830);

	Log log{ TEXTURE::LOG };
	log.setPosition(810,720);

	bool acceptInput = false;

	SoundObj chop{ "sounds/chop.wav" };
	SoundObj death{ "sounds/death.wav" };
	SoundObj oot{ "sounds/out_of_time.wav" };

	Time one_sec;
	while (window.isOpen()) {

		// FPS DISPLAY CODE ----------------->
		Time dt = clock.restart();
		one_sec += dt;
		if (one_sec.asMilliseconds() > 200) {
			auto fps = 1.f / dt.asSeconds();
			updateValue(fpsText, fps);
			one_sec = Time::Zero;
		}
		// FPS DISPLAY CODE <-----------------

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				acceptInput = true;
				axe.setPosition(2000, axe.getPosition().y);
				timeBarFrame.setOutlineColor(Color::Yellow);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
			window.close();
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;
			score = 0;
			timeRemaining = 6.0f;

			for (int i = 0; i < NUM_BRANCHES; ++i)
				branch_position[i] = side::NONE;

			gravestone.setPosition(675, 2000);
			player.setPosition(580, 720);
			acceptInput = true;
		}

		if (acceptInput) {
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				player.side_ = side::RIGHT;
				++score;

				if (timeRemaining < 6.0f) {
					if (timeRemaining + (2 / score + 0.15f) > 6.0f)
						timeRemaining = 6.0f;
					else
						timeRemaining += 2 / score + 0.15f;
				}

				axe.setPosition(AXE_POSITION_RIGHT, axe.getPosition().y);
				player.setPosition(1200, 720);

				updateBranches(score);

				log.setPosition(810, 720);
				log.speedX_ = -5000;
				log.is_active_ = true;

				acceptInput = false;
				chop.play();
				updateValue(scoreText, score);
				timeBarFrame.setOutlineColor(Color::White);
			}

			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				player.side_ = side::LEFT;
				++score;

				if (timeRemaining < 6.0f) {
					if (timeRemaining + (2 / score + 0.15f) > 6.0f)
						timeRemaining = 6.0f;
					else
						timeRemaining += 2 / score + 0.15f;					
				}

				axe.setPosition(AXE_POSITION_LEFT, axe.getPosition().y);
				player.setPosition(580, 720);

				updateBranches(score);

				log.setPosition(810, 720);
				log.speedX_ = 5000;
				log.is_active_ = true;

				acceptInput = false;
				chop.play();
				updateValue(scoreText, score);
				timeBarFrame.setOutlineColor(Color::White);
			}
		}

		// UPDATE THE SCENE ===========
		if (!paused) {
			timeRemaining -= dt.asSeconds();
			timeBar.setSize(timeBarWidthPerSecond * timeRemaining, timeBar.getHeight());

			if (timeRemaining <= -0.0f) {
				paused = true;
				acceptInput = false;
				messageText.setString("Out of time!");

				setCentre(messageText);
				oot.play();
			}

			if (!bee.is_active_) {
				srand(int(time(0)));
				bee.speed_ = float((rand() % 200) + 200);

				srand(int(time(0) * 10));
				float height = float((rand() % 500) + 500);
				bee.setPosition(2000, height);
				bee.is_active_ = true;
			}
			else {
				bee.setPosition(
					bee.getPosition().x - (bee.speed_ * dt.asSeconds()),
					bee.getPosition().y);
				if (bee.getPosition().x < -100)
					bee.is_active_ = false;
			}

			for (int i = 0; i < NUM_BRANCHES; ++i) {
				float height = i * 150.0f;
				if (branch_position[i] == side::LEFT) {
					branches[i].setPosition(610, height);
					branches[i].setRotation(180);
				}
				else if (branch_position[i] == side::RIGHT) {
					branches[i].setPosition(1330, height);
					branches[i].setRotation(0);
				}
				else
					branches[i].setPosition(3000, height);
			}

			if (log.is_active_) {
				log.setPosition(
					log.getPosition().x + log.speedX_ * dt.asSeconds(),
					log.getPosition().y + log.speedY_ * dt.asSeconds());
				if (log.getPosition().x < -100 || log.getPosition().x > 2000) {
					log.is_active_ = false;
					log.setPosition(810, 720);
				}
			}

			if (branch_position[5] == player.side_) {
				paused = true;
				acceptInput = false;

				gravestone.setPosition(525, 760);
				player.setPosition(2000, 660);
				axe.setPosition(2000, 830);

				messageText.setString("ULEGLAK SQUISHED");
				setCentre(messageText);
				death.play();
			}
		}
		
		animateCloud(clouds, dt);

		// DRAW THE SCENE =============
		window.clear();
		window.draw(background.getSprite());

		for (int i = 0; i < NUM_CLOUDS; ++i)
			window.draw(clouds[i].getSprite());

		window.draw(scoreBackground.getRect());
		window.draw(fpsBackground.getRect());

		for (int i = 0; i < NUM_BRANCHES; ++i)
			window.draw(branches[i].getSprite());

		window.draw(log.getSprite());
		window.draw(tree.getSprite());
		window.draw(player.getSprite());
		window.draw(axe.getSprite());
		window.draw(gravestone.getSprite());
		window.draw(bee.getSprite());
		window.draw(scoreText);
		window.draw(fpsText);
		window.draw(timeBar.getRect());
		window.draw(timeBarFrame.getRect());

		if (paused)
			window.draw(messageText);

		window.display();
	}

	return 0;
}

void animateCloud(Cloud* cloud, Time const & t)
{
	for (int i = 0; i < NUM_CLOUDS; ++i) {
		if (!cloud[i].active_) {
			srand(int(time(0) * cloud[i].seed_modifier_));
			cloud[i].speed_ = float(rand() % 50 + 50);

			srand(int(time(0) * cloud[i].seed_modifier_));
			float height = float(rand() % (cloud[i].seed_modifier_ * 5));
			cloud[i].setPosition(-300, height);
			cloud[i].active_ = true;
		}
		else {
			cloud[i].setPosition(
				cloud[i].getPosition().x + (cloud[i].speed_ * t.asSeconds()),
				cloud[i].getPosition().y);
			if (cloud[i].getPosition().x > 1920)
				cloud[i].active_ = false;
		}
	}
}

void setCentre(Text& t)
{
	FloatRect textRect = t.getLocalBounds();
	t.setOrigin(
		textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	t.setPosition(1920 / 2.0f, 1080 / 2.0f);
}

void updateBranches(int seed)
{
	for (int i = NUM_BRANCHES - 1; i > 0; --i)
		branch_position[i] = branch_position[i - 1];

	srand(int(time(0) + seed));
	int r = rand() % 5;

	switch (r) {
	case 0:
		branch_position[0] = side::LEFT;
		break;
	case 1:
		branch_position[0] = side::RIGHT;
		break;
	default:
		branch_position[0] = side::NONE;
		break;
	}
}

template<typename T>
void updateValue(Text& t, T value)
{
	std::stringstream buffer;
	buffer << "Score = " << std::setprecision(4) << value;
	t.setString(buffer.str());
}

Text initText(Font& font, char const* message, unsigned size, Color c)
{
	Text t{ message, font, size };
	t.setFillColor(c);
	return t;
}