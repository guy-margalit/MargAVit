#pragma once

#include <functional>

template<class ObjectType, class DeleterType>
class AutoCloseObject
{
public:
	AutoCloseObject(const ObjectType& object, DeleterType& deleter) :
		m_object(object),
		m_deleter(deleter),
		m_should_free(true)
	{}

	AutoCloseObject(ObjectType&& object, DeleterType& deleter) :
		m_object(object),
		m_deleter(deleter),
		m_should_free(true)
	{}

	AutoCloseObject(AutoCloseObject&& other)
	{
		m_object = other.m_object;
		m_deleter = other.m_deleter;
		m_should_free = other.m_should_free;

		other.m_object = nullptr;
		other.m_deleter = nullptr;
		other.m_should_free = false;
	}

	AutoCloseObject(const AutoCloseObject&) = delete;
	AutoCloseObject& operator=(const AutoCloseObject&) = delete;

	virtual ~AutoCloseObject()
	{
		try
		{
			close();
		}
		catch (...) {}
	}

	void close()
	{
		if (m_should_free && (nullptr != m_object))
		{
			m_deleter(m_object);
			m_should_free = false;
		}
	}

	ObjectType get() const { return m_object; }

	ObjectType detach()
	{
		m_should_free = false;
		return m_object;
	}

protected:
	ObjectType m_object;
	const std::function<DeleterType> m_deleter;
	bool m_should_free;
};
