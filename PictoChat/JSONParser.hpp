#pragma once

#include <Siv3D.hpp>

namespace oykdn
{
	class JSONParser
	{
	private:
		class JSONParserDetail
		{
		private:
			HashTable<String, String> m_table;

			// 開始-終端が指定された文字だったら、それらを削除した文字を返す
			// 違ったら、そのままの文字列を返す
			String mid(const String& str, const char32_t t) const
			{
				return mid(str, t, t);
			}

			// 開始-終端が指定された文字だったら、それらを削除した文字を返す
			// 違ったら、そのままの文字列を返す
			String mid(const String& str, const char32_t start, const char32_t end) const
			{
				if (str.length() >= 2 && str.front() == start && str.back() == end)
				{
					return str.substr(1, str.length() - 2);
				}

				return str;
			}

		public:
			JSONParserDetail() {}

			void clear()
			{
				m_table.clear();
			}

			bool parse(const String& json)
			{
				clear();

				String trimmed = json.trimmed();
				if (!(trimmed.starts_with(U"{") && trimmed.ends_with(U"}")))
				{
					return false;
				}

				// 注意: このプログラムでは、単純なKey-Value Pair形式のJSONのみ解析可

				// {, }の削除
				trimmed = mid(trimmed, '{', '}');

				// key-value pairに分解
				for (const auto& keyvalue : trimmed.split(','))
				{
					const auto splited = keyvalue.trimmed().split(':');
					if (splited.size() != 2) continue;

					// Print << U"key: " << splited[0] << U"value: " << splited[1];

					const String key = mid(splited[0].trimmed(), '"');
					const String value = mid(splited[1].trimmed(), '"');
					if (key.size() > 0 && value.size() > 0)
					{
						m_table.emplace(key, value);
					}
				}

				return true;
			}

			Optional<String> getString(const String& key) const
			{
				if (m_table.contains(key))
				{
					return m_table.at(key);
				}
				else
				{
					return none;
				}
			}

			Optional<int32> getOptInt32(const String& key) const
			{
				if (m_table.contains(key))
				{
					return ParseOpt<int32>(m_table.at(key));
				}
				else
				{
					return none;
				}
			}

			Optional<double> getOptDouble(const String& key) const
			{
				if (m_table.contains(key))
				{
					return ParseOpt<double>(m_table.at(key));
				}
				else
				{
					return none;
				}
			}

		};

		std::shared_ptr<JSONParserDetail> m_Impl;

	public:
		JSONParser() : m_Impl(std::make_shared<JSONParserDetail>()) {}

		[[nodiscard]] bool parse(const String& json)
		{
			return m_Impl->parse(json);
		}

		[[nodiscard]] Optional<String> getString(const String& key) const
		{
			return m_Impl->getString(key);
		}

		[[nodiscard]] Optional<int32> getOptInt32(const String& key) const
		{
			return m_Impl->getOptInt32(key);
		}

		[[nodiscard]] Optional<double> getOptDouble(const String& key) const
		{
			return m_Impl->getOptDouble(key);
		}

	};
}
