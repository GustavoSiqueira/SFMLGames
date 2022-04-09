#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include <Windows.h>

#include <sstream>
#include <ctime>

sf::Vector2f viewSize(1024, 768);
sf::VideoMode vm(viewSize.x, viewSize.y);
sf::RenderWindow window(vm, "Squares", sf::Style::Titlebar);

sf::RectangleShape player;
sf::RectangleShape enemy;
sf::RectangleShape pluck;

sf::Text start_message;
sf::Text debug_message;
sf::Text player_score_text;
sf::Text enemy_score_text;
sf::Text x_text;

auto const PADDLE_SPEED = 700.0f;
auto x_speed = 0.0f, y_speed = 0.0f;

sf::Font font;

const auto paddle_color = sf::Color(180, 180, 180, 255);
const auto paddle_size = sf::Vector2f(15.0f, 45.0f);
const auto pluck_size = sf::Vector2f(5.0f, 5.0f);
const auto clear_color = sf::Color(20, 20, 20, 255);

sf::SoundBuffer sound_buffer;
sf::Sound pluck_sound;

enum GameState
{
	STOPPED,
	RUNNING,
	PAUSED,
	LAUNCHING
};

auto gameState = GameState::STOPPED;

auto debug = false;

unsigned int player_score = 0;
unsigned int enemy_score = 0;

float launch_timer = 0.0f;
const float launch_wait = 3.0f;

float clamp_magnitude(float value, float min, float max)
{
	if (min < 0.0f || max < 0.0f) return value; //Incorrect usage. Ignore.
	if (abs(value) > max) return (max / abs(value)) * value;
	if (abs(value) < min) return (min / abs(value)) * value;;
	return value;
}

std::string createDebugMessage(const float t, const float dt)
{
	std::stringstream ss;

	ss << "Frametime = " << dt * 1000.0f << "ms" << std::endl;
	ss << "Total Time = " << t << "s" << std::endl;
	ss << "x_speed = " << x_speed << std::endl;
	ss << "y_speed = " << y_speed << std::endl;
	ss << "State = " << gameState << std::endl;
	ss << "Player Score = " << player_score << std::endl;
	ss << "COM Score = " << enemy_score << std::endl;
	ss << "Launch Timer = " << launch_timer << std::endl;

	return ss.str();
}

void init()
{
	player = sf::RectangleShape(paddle_size);
	player.setFillColor(paddle_color);
	player.setPosition(sf::Vector2f(player.getSize().x / 2.0f, viewSize.y / 2));
	player.setOrigin(player.getSize().x / 2, player.getSize().y / 2);

	enemy = sf::RectangleShape(paddle_size);
	enemy.setFillColor(paddle_color);
	enemy.setPosition(sf::Vector2f(viewSize.x - enemy.getSize().x / 2.0f, viewSize.y / 2));
	enemy.setOrigin(enemy.getSize().x / 2, enemy.getSize().y / 2);

	pluck = sf::RectangleShape(pluck_size);
	pluck.setFillColor(paddle_color);
	pluck.setPosition(sf::Vector2f(viewSize.x / 2.0f, viewSize.y / 2.0f));
	pluck.setOrigin(pluck.getSize().x / 2, pluck.getSize().y / 2);

	gameState = GameState::STOPPED;

	font = sf::Font();
	font.loadFromFile("Assets/fonts/press-start/prstart.ttf");

	start_message = sf::Text("Press ENTER to start!", font, 25);
	start_message.setFillColor(paddle_color);
	start_message.setPosition(viewSize.x / 2 - start_message.getLocalBounds().width / 2, viewSize.y - start_message.getLocalBounds().height - 100);

	player_score_text = sf::Text(std::to_string(player_score), font, 30);
	player_score_text.setFillColor(paddle_color);
	player_score_text.setOrigin(0.0f, 0.0f);
	player_score_text.setPosition(sf::Vector2f((viewSize.x / 2.0f) - 50.0f, 10.0f));

	enemy_score_text = sf::Text(std::to_string(enemy_score), font, 30);
	enemy_score_text.setFillColor(paddle_color);
	enemy_score_text.setOrigin(0.0f, 0.0f);
	enemy_score_text.setPosition(sf::Vector2f((viewSize.x / 2.0f) + enemy_score_text.getLocalBounds().width + 50.0f, 10.0f));

	x_text = sf::Text("x", font, 30);
	x_text.setPosition(viewSize.x / 2.0f, 10);
	x_text.setFillColor(paddle_color);

	debug_message = sf::Text("", font, 10);

	player_score = 0;
	enemy_score = 0;

	sound_buffer = sf::SoundBuffer();
	if (!sound_buffer.loadFromFile("Assets/sound/pluck.wav"))
	{
		exit(EXIT_FAILURE);
	}

	pluck_sound = sf::Sound(sound_buffer);
}

void draw(float dt, float t)
{
	window.draw(player);
	window.draw(enemy);
	window.draw(pluck);

	player_score_text.setString(std::to_string(player_score));
	enemy_score_text.setString(std::to_string(enemy_score));

	window.draw(enemy_score_text);
	window.draw(player_score_text);
	window.draw(x_text);

	if (gameState == GameState::STOPPED)
	{
		if (sin(6 * t) > 0) window.draw(start_message); //makes the text blink
	}

	if (debug)
	{
		debug_message.setString(createDebugMessage(t, dt));
		debug_message.setFillColor(paddle_color);
		debug_message.setPosition(0, viewSize.y - debug_message.getLocalBounds().height - 10);
		window.draw(debug_message);
	}
}

void launch_pluck(float wait)
{
	gameState = GameState::LAUNCHING;
	launch_timer = wait;
	srand(time(0));
	x_speed = clamp_magnitude(sin(rand() % 1000) * 12.0f, 7.0f, 12.0f);
	y_speed = clamp_magnitude(cos(rand() % 1000) * 7.0f, 2.0f, 6.0f);
}

void launch_pluck()
{
	launch_pluck(launch_wait);
}

void update(float dt, float t)
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		if (event.key.code == sf::Keyboard::Escape || event.type == sf::Event::Closed)
		{
			window.close();
		}

		if (event.key.code == sf::Keyboard::Home && event.type == sf::Event::EventType::KeyPressed)
		{
			debug = !debug;
		}
	}

	if (gameState == GameState::LAUNCHING)
	{
		if (launch_timer > 0)
		{
			launch_timer -= dt;
		}
		else
		{
			gameState = GameState::RUNNING;
			pluck_sound.play();
		}
	}

	if (gameState == GameState::RUNNING)
	{
		auto pluck_pos = pluck.getPosition();
		auto player_pos = player.getPosition();
		auto enemy_pos = enemy.getPosition();
		auto paddle_size = player.getSize();

		pluck_pos.x += x_speed;
		pluck_pos.y += y_speed;

		if (pluck_pos.y <= 0 || pluck_pos.y >= viewSize.y) y_speed *= -1;
		if (pluck_pos.x - pluck_size.x <= player_pos.x + paddle_size.x / 2.0f)
		{
			if ((pluck_pos.y >= player_pos.y - paddle_size.y / 2.0f) && (pluck_pos.y <= player_pos.y + paddle_size.y / 2.0f))
			{
				x_speed *= -1;
				pluck_sound.play();
			}
			else
			{
				enemy_score++;
				pluck_pos = sf::Vector2f(viewSize.x / 2.0f, viewSize.y / 2.0f);
				launch_pluck();
			}
		}

		if (pluck_pos.x + pluck_size.x >= enemy_pos.x - paddle_size.x / 2.0f)
		{
			if ((pluck_pos.y >= enemy_pos.y - paddle_size.y / 2.0f) && (pluck_pos.y <= enemy_pos.y + paddle_size.y / 2.0f))
			{
				x_speed *= -1;
				pluck_sound.play();
			}
			else
			{
				player_score++;
				pluck_pos = sf::Vector2f(viewSize.x / 2.0f, viewSize.y / 2.0f);
				launch_pluck();
			}
		}

		pluck.setPosition(pluck_pos);
	}

	if (gameState == GameState::STOPPED)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		{
			launch_pluck(0.5f);
		}
	}

	const auto player_position = player.getPosition();
	const auto enemy_position = enemy.getPosition();
	const auto halfHeight = player.getSize().y / 2.0f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		if (player_position.y > halfHeight)
			player.move(0.0f, -PADDLE_SPEED * dt);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		if (player_position.y < vm.height - halfHeight)
			player.move(0.0f, PADDLE_SPEED * dt);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		if (enemy_position.y > halfHeight)
			enemy.move(0.0f, -PADDLE_SPEED * dt);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		if (enemy_position.y < vm.height - halfHeight)
			enemy.move(0.0f, PADDLE_SPEED * dt);
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	init();

	sf::Clock frame_clock, clock;

	window.setVerticalSyncEnabled(true);

	while (window.isOpen())
	{
		sf::Time dt = frame_clock.restart();
		sf::Time t = clock.getElapsedTime();
		window.clear(clear_color);
		update(dt.asSeconds(), t.asSeconds());
		draw(dt.asSeconds(), t.asSeconds());
		window.display();
	}

	return 0;
}