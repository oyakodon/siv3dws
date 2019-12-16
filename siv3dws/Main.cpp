#include <Siv3D.hpp> // OpenSiv3D v0.4.2
#include "SivWSClient.hpp"

void Main()
{
	const Font font(60);

	oykdn::WebSocketClient client;
	
	client.onMessage([&](const String& msg)
		{
			Print << msg;
		}
	);

	client.onOpen([]() { Print(U"👍 connected!"); });
	client.onClose([]() { Print(U"👼 disconnected"); });
	client.onError([]() { Print(U"🙅‍♀ ERROR"); });

	client.open(U"ws://localhost:8080/");

	while (System::Update())
	{
		font(U"Hello, Siv3D!🐣").drawAt(Scene::Center());
		Circle(Cursor::Pos(), 20).draw(ColorF(1, 0, 0, 0.5));

		if (SimpleGUI::Button(U"Send", Vec2(600, 20)))
		{
			client.send(U"いろはにほへと");
		}
	}
}
