#include "WSClientDetail.hpp"

namespace oykdn
{
	void WebSocketClient::WSClientDetail::_onMessage(websocketpp::connection_hdl hdl, message_ptr msg)
	{
		LOG_DEBUG(U"WebSocketClient: _onMessage() message = {}"_fmt(Unicode::Widen(msg->get_payload())));

		if (m_callbackMessage)
		{
			m_callbackMessage(Unicode::FromUTF8(msg->get_payload()));
		}
	}

	void WebSocketClient::WSClientDetail::_onOpen(websocketpp::connection_hdl hdl)
	{
		LOG_DEBUG(U"WebSocketClient: _onOpen()");

		m_waitingConnection = false;
		m_isOpen = true;

		if (m_callbackOpen)
		{
			m_callbackOpen();
		}
	}

	void WebSocketClient::WSClientDetail::_onClose(websocketpp::connection_hdl hdl)
	{
		LOG_DEBUG(U"WebSocketClient: _onClose()");

		if (m_callbackClose)
		{
			m_callbackClose();
		}
	}

	void WebSocketClient::WSClientDetail::_onError(websocketpp::connection_hdl hdl)
	{
		LOG_DEBUG(U"WebSocketClient: _onError()");

		if (m_callbackError)
		{
			m_callbackError();
		}
	}

	bool WebSocketClient::WSClientDetail::open(const String& uri)
	{
		m_uri = uri;

		if (m_isOpen)
		{
			return false;
		}

		if (m_waitingConnection)
		{
			return false;
		}

		m_client = std::make_shared<client>();
		// m_client->set_access_channels(websocketpp::log::alevel::all);
		// m_client->clear_access_channels(websocketpp::log::alevel::frame_payload);

		m_client->init_asio();

		m_client->set_open_handler(std::bind(&WSClientDetail::_onOpen, this, std::placeholders::_1));
		m_client->set_close_handler(std::bind(&WSClientDetail::_onClose, this, std::placeholders::_1));
		m_client->set_fail_handler(std::bind(&WSClientDetail::_onError, this, std::placeholders::_1));
		m_client->set_message_handler(
			std::bind(&WSClientDetail::_onMessage, this, std::placeholders::_1, std::placeholders::_2));

		websocketpp::lib::error_code error;
		m_connectionHdl = m_client->get_connection(uri.toUTF8(), error);
		if (error)
		{
			LOG_FAIL(U"WebSocketClient: open failed: {}"_fmt(Unicode::Widen(error.message())));
			m_error = NetworkError::Error;
			m_client.reset();
			return false;
		}

		m_client->connect(m_connectionHdl);

		m_waitingConnection = true;

		m_client->start_perpetual();
		m_io_service_thread = std::async([=] { m_client->run(); });

		return true;
	}

	void WebSocketClient::WSClientDetail::close()
	{
		if (m_client)
		{
			if (m_client->is_listening())
			{
				m_client->close(m_connectionHdl, websocketpp::close::status::normal, "close()");
			}

			m_waitingConnection = false;
			m_error = NetworkError::OK;
			m_isOpen = false;

			m_client->stop_perpetual();
			m_client->stop();
			m_io_service_thread.wait();
			m_client->reset();

			m_client.reset();

			LOG_DEBUG(U"WebSocketClient: closed");
		}
	}

	bool WebSocketClient::WSClientDetail::send(const String& payload)
	{
		if (!m_isOpen)
		{
			return false;
		}

		m_client->send(m_connectionHdl, payload.toUTF8(), websocketpp::frame::opcode::TEXT);

		return true;
	}

}
