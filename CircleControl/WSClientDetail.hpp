#pragma once

#include "SivWSClient.hpp"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

namespace oykdn
{
	typedef websocketpp::client<websocketpp::config::asio_client> client;
	typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

	class WebSocketClient::WSClientDetail
	{
	private:
		std::future<void> m_io_service_thread;
		std::shared_ptr<client> m_client;

		String m_uri;
		client::connection_ptr m_connectionHdl;
		void _onOpen(websocketpp::connection_hdl hdl);
		void _onClose(websocketpp::connection_hdl hdl);
		void _onError(websocketpp::connection_hdl hdl);
		void _onMessage(websocketpp::connection_hdl hdl, message_ptr msg);

		std::function<void()> m_callbackOpen;
		std::function<void()> m_callbackClose;
		std::function<void()> m_callbackError;
		std::function<void(const String&)> m_callbackMessage;
		
		bool m_isOpen = false;
		bool m_waitingConnection = false;
		NetworkError m_error = NetworkError::OK;

	public:

		WSClientDetail() {}

		~WSClientDetail()
		{
			close();
		}

		void onOpen(const std::function<void()>& callback)
		{
			m_callbackOpen = callback;
		}

		void onClose(const std::function<void()>& callback)
		{
			m_callbackClose = callback;
		}

		void onError(const std::function<void()>& callback)
		{
			m_callbackError = callback;
		}

		void onMessage(const std::function<void(const String&)>& callback)
		{
			m_callbackMessage = callback;
		}

		bool open(const String& uri);

		bool isOpen() const
		{
			return m_isOpen;
		}

		void close();

		bool send(const String& payload);

		String uri() const
		{
			return m_uri;
		}

		bool hasError() const
		{
			return m_error != NetworkError::OK;
		}

	};
}
