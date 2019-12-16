#include <Siv3D.hpp> // OpenSiv3D v0.4.2
#include "SivWSClient.hpp"
#include "JSONParser.hpp"

void Main()
{
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	const Font fontSm(24);
	const Font font(60);

	Circle me(Scene::Center(), 30);
	Vec2 diff = Vec2::Zero();

	oykdn::WebSocketClient client;
	oykdn::JSONParser parser;

	client.onMessage([&](const String& msg)
		{
			if (!parser.parse(msg))
			{
				return;
			}

			if (parser.getOptDouble(U"x") && parser.getOptDouble(U"y"))
			{
				diff.x = parser.getOptDouble(U"x").value();
				diff.y = parser.getOptDouble(U"y").value();
				me.moveBy(diff.x / 10.0, diff.y / 10.0);
			}
		}
	);

	client.open(U"ws://localhost:8080/");

	while (System::Update())
	{
		fontSm(U"diff: ", diff).draw(Vec2(10, 10), Palette::Black);

		font(U"Hello, Siv3D!🐣").drawAt(Scene::Center(), Palette::Black);

		if (KeyR.down())
		{
			me.setPos(Scene::Center());
		}

		me.draw(ColorF(1, 0, 0, 0.5));
	}

}
