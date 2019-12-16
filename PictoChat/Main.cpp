#include <Siv3D.hpp> // OpenSiv3D v0.4.2
#include "SivWSClient.hpp"
#include "JSONParser.hpp"

struct Message
{
	String name;
	String text;
	Optional<Image> image;

	Message(const String& _name, const String& _text, const Optional<Image>& _image)
		: name(_name), text(_text), image(_image)
	{}
};

void Main()
{
	bool isJoined = false;
	int32 img_current = 0;
	int32 img_count = 0;

	Array<std::shared_ptr<Message>> messages;

	oykdn::WebSocketClient client;
	oykdn::JSONParser parser;

	client.onMessage([&](const String& msg)
		{
			if (!parser.parse(msg))
			{
				// Not JSON
				// Print << msg;
				return;
			}

			if (!isJoined) return;

			if (parser.getString(U"type"))
			{
				String type = parser.getString(U"type").value();
				auto name = parser.getString(U"name");
				auto text = parser.getString(U"text");
				auto base64img = parser.getString(U"image");

				if (type == U"message")
				{
					if (name)
					{
						if (text)
						{
							if (base64img)
							{
								messages.emplace_back(std::make_shared<Message>(name.value(), text.value(), Image(Base64::Decode(base64img.value()))));
								img_current = 0;
								img_count++;
							}
							else
							{
								messages.emplace_back(std::make_shared<Message>(name.value(), text.value(), none));
							}
						}
						else if (base64img)
						{
							messages.emplace_back(std::make_shared<Message>(name.value(), U"", Image(Base64::Decode(base64img.value()))));
							img_current = 0;
							img_count++;
						}
						
					}
				}
				else if (type == U"join")
				{
					if (name)
					{
						Print << name.value() << U"さんが参加しました。";
					}
				}
				else if (type == U"leave")
				{
					if (name)
					{
						Print << name.value() << U"さんが退出しました。";
					}
				}

			}

		}
	);

	client.onOpen([]() { Print(U"👍 connected!"); });
	client.onClose([]() { Print(U"👼 disconnected"); });
	client.onError([]() { Print(U"🙅‍♀ ERROR"); });

	client.open(U"ws://localhost:8080/");

	const Font font(24);

	TextEditState input;
	String name;
	bool canvasChanged = false;

	constexpr Point canvasBasePos(40, 350);
	constexpr Size canvasSize(500, 150);
	double thickness = 0.5;
	HSV penColor = Palette::Black;
	Image image(canvasSize, Palette::White);
	DynamicTexture texture(image);

	constexpr Point logBasePos(40, 40);
	constexpr Point logImgBasePos(510, 118);
	DynamicTexture texLog;

	System::SetTerminationTriggers(UserAction::None);
	Window::SetTitle(U"PictoChat");

	while (System::Update())
	{
		if (System::GetUserActions() == UserAction::CloseButtonClicked
			|| KeyEscape.down())
		{
			if (isJoined)
			{
				client.send(U"{{\"type\": \"leave\", \"name\": \"{}\"}}"_fmt(name));
			}

			System::Exit();
		}

		if (!isJoined)
		{
			font(U"名前: ").draw(Vec2(80, 50));

			SimpleGUI::TextBox(input, Vec2(160, 50));

			if (SimpleGUI::Button(U"参加", Vec2(370, 50)))
			{
				if (!input.text.trimmed().isEmpty())
				{
					name = input.text.trimmed();
					Window::SetTitle(U"PictoChat: " + name);
					client.send(U"{{\"type\": \"join\", \"name\": \"{}\"}}"_fmt(name));
					input.clear();
					isJoined = true;
				}
			}

			continue;
		}

		// ログ描画エリア
		if (SimpleGUI::Button(U"<", logImgBasePos + Vec2(0, 85), 60, (img_current < img_count - 1)))
		{
			img_current++;
		}
		if (SimpleGUI::Button(U">", logImgBasePos + canvasSize / 2 + Vec2(-60, 10), 60, (img_current >= 1)))
		{
			img_current--;
		}

		int32 y = 0, count = 0;
		for (const auto& msg : messages.reversed().take(8).reversed())
		{
			font(U"{}: {}"_fmt(msg->name, msg->text.isEmpty() ? U"(画像のみ)" : msg->text)).draw(logBasePos.movedBy(0, y));
			y += 30;
		}
		for (const auto& msg : messages.reversed())
		{
			if (msg->image)
			{
				if (count <= img_current)
				{
					texLog.fill(msg->image.value());
				}
				count++;
			}
		}
		if (texLog)
		{
			texLog.scaled(0.5).draw(logImgBasePos);
		}

		// お絵かきエリア
		if (Rect(canvasBasePos, canvasSize).leftPressed())
		{
			canvasChanged = true;

			const Point from = MouseL.down() ? Cursor::Pos() : Cursor::PreviousPos();
			const Point to = Cursor::Pos();
			Line(from, to).movedBy(-canvasBasePos).overwrite(image, static_cast<int32>(thickness * 16), penColor);
			texture.fill(image);
		}

		SimpleGUI::ColorPicker(penColor, Vec2(555, 350));
		SimpleGUI::VerticalSlider(thickness, Vec2(730, 350), 150);

		if (SimpleGUI::Button(U"全消し", Vec2(640, 520), 75))
		{
			image.fill(Palette::White);
			texture.fill(image);
		}

		texture.draw(canvasBasePos);

		// チャットエリア
		SimpleGUI::TextBox(input, Vec2(40, 520), 500);

		if (SimpleGUI::Button(U"送信", Vec2(555, 520), 75)
			|| (KeyControl.pressed() && KeyEnter.down()))
		{
			String text = input.text.trimmed();
			if (text)
			{
				if (canvasChanged)
				{
					client.send(U"{{\"type\": \"message\", \"name\": \"{}\", \"text\": \"{}\", \"image\": \"{}\"}}"_fmt(name, text, Base64::Encode(image.encodePNG())));
					messages.emplace_back(std::make_shared<Message>(name, text, image));
					canvasChanged = false;
					image.fill(Palette::White);
					texture.fill(image);

					img_count++;
					img_current = 0;
				}
				else
				{
					client.send(U"{{\"type\": \"message\", \"name\": \"{}\", \"text\": \"{}\"}}"_fmt(name, text));
					messages.emplace_back(std::make_shared<Message>(name, text, none));
				}

				input.clear();
			}
			else if (canvasChanged)
			{
				client.send(U"{{\"type\": \"message\", \"name\": \"{}\", \"image\": \"{}\"}}"_fmt(name, Base64::Encode(image.encodePNG())));
				messages.emplace_back(std::make_shared<Message>(name, U"", image));
				canvasChanged = false;
				image.fill(Palette::White);
				texture.fill(image);

				img_count++;
				img_current = 0;
			}
		}

		if (SimpleGUI::Button(U"切断", Vec2(700, 20), 75))
		{
			client.send(U"{{\"type\": \"leave\", \"name\": \"{}\"}}"_fmt(name));

			isJoined = false;
			name = U"";
			image.fill(Palette::White);
			texture.fill(image);
			messages.clear();
			input.clear();

			img_count = 0;
			img_current = 0;

			Window::SetTitle(U"PictoChat");
		}
	}

}
