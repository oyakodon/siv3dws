#pragma once

#include <Siv3D.hpp>
#include <memory>

namespace oykdn
{
	class WebSocketClient
	{
	private:
		class WSClientDetail;

		std::shared_ptr<WSClientDetail> pImpl;

	public:

		WebSocketClient();
		~WebSocketClient();

		void onOpen(const std::function<void()>& callback);
		void onClose(const std::function<void()>& callback);
		void onError(const std::function<void()>& callback);
		void onMessage(const std::function<void(const String&)>& callback);

		bool open(const String& uri);

		[[nodiscard]] bool isOpen() const;

		void close();

		bool send(const String& payload);

		[[nodiscard]] String uri() const;

		[[nodiscard]] bool hasError() const;

	};
}
