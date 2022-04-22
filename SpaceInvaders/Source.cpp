#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include <Windows.h>

const sf::Color CLEAR_COLOR(0.15f, 0.15f, 0.15f, 1.0f);
const float PLAYER_SPEED = 300.0f;

sf::Vector2f windowSize(1280, 720);
sf::VideoMode videoMode(windowSize.x, windowSize.y);
sf::RenderWindow window(videoMode, "Space Invaders", sf::Style::Titlebar);

sf::Event e;

void init()
{

}

void update(float t, float dt)
{
	while (window.pollEvent(e))
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();
	}
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nCmdShow)
{
	window.setVerticalSyncEnabled(true);

	init();

	sf::Clock frameClock, gameClock;

	while (window.isOpen())
	{
		sf::Time dt = frameClock.restart();
		sf::Time t = gameClock.getElapsedTime();

		window.clear(CLEAR_COLOR);

		update(t.asMilliseconds(), dt.asMilliseconds());

		window.display();
	}
}