#include "SivWSClient.hpp"
#include "WSClientDetail.hpp"

namespace oykdn
{
	WebSocketClient::WebSocketClient() : pImpl(std::make_shared<WSClientDetail>())
	{

	}

	WebSocketClient::~WebSocketClient()
	{

	}

	void WebSocketClient::onOpen(const std::function<void()>& callback)
	{
		pImpl->onOpen(callback);
	}

	void WebSocketClient::onClose(const std::function<void()>& callback)
	{
		pImpl->onClose(callback);
	}

	void WebSocketClient::onError(const std::function<void()>& callback)
	{
		pImpl->onError(callback);
	}

	void WebSocketClient::onMessage(const std::function<void(const String&)>& callback)
	{
		pImpl->onMessage(callback);
	}

	bool WebSocketClient::open(const String& uri)
	{
		return pImpl->open(uri);
	}

	bool WebSocketClient::isOpen() const
	{
		return pImpl->isOpen();
	}

	void WebSocketClient::close()
	{
		pImpl->close();
	}

	bool WebSocketClient::send(const String& payload)
	{
		return pImpl->send(payload);
	}

	String WebSocketClient::uri() const
	{
		return pImpl->uri();
	}

	bool WebSocketClient::hasError() const
	{
		return pImpl->hasError();
	}

}
