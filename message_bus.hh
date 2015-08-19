#pragma once
#include <string>
#include <functional>
#include <map>
#include "any.hh"
#include "function_traits.hh"
#include "non_copyable.hh"

using namespace std;

class MessageBus: NonCopyable
{
public:
	//注册消息
	template<typename F>
	void attach(F&& f, const string& strTopic="")
	{
		auto func = to_function(std::forward<F>(f));
		add(strTopic, std::move(func));
	}

	//发送消息
	void send(const string& strTopic = "")
	{
		using function_type = std::function<void()>;
		string strMsgType =strTopic+ typeid(function_type).name();
		auto range = m_map.equal_range(strMsgType);
		for (Iterater it = range.first; it != range.second; ++it)
		{
			auto f = it->second.AnyCast < function_type >();
			f();
		}
	}
	template<typename... Args>
	void send(Args... args, const string& strTopic = "")
	{
		using function_type = std::function<void(Args...)>;
		string strMsgType =strTopic+ typeid(function_type).name();
		auto range = m_map.equal_range(strMsgType);
		for (Iterater it = range.first; it != range.second; ++it)
		{
			auto f = it->second.AnyCast < function_type >();
			f(std::forward<Args>(args)...);
		}
	}

	//移除某个主题, 需要主题和消息类型
	template<typename R, typename... Args>
	void remove(const string& strTopic = "")
	{
		using function_type = std::function<R(Args...)>; //typename function_traits<void(CArgs)>::stl_function_type;

		string strMsgType =strTopic +typeid(function_type).name();
		auto range = m_map.equal_range(strMsgType);
		m_map.erase(range.first, range.second);
	}

private:
	template<typename F>
	void add(const string& strTopic, F&& f)
	{
		string strMsgType = strTopic + typeid(F).name();
		m_map.emplace(std::move(strMsgType), std::forward<F>(f));
	}

private:
	std::multimap<string, Any> m_map;
	typedef std::multimap<string, Any>::iterator Iterater;
};
