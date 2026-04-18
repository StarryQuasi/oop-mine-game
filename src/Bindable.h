#pragma once

#include <algorithm>
#include <cassert>
#include <functional>

// A property object that can be bound (synced) to other instances
// bidirectionally, stores a local copy, also provides value changed event
template <typename T>
class Bindable
{
public:
	// Creates an empty bindable
	Bindable() = default;
	// Creates a bindable that's bound to the other bindable instance
	Bindable(Bindable<T>& other);
	// Moves the other bindable instance to this
	Bindable(Bindable<T>&& other);
	// Deletes the bindable, also unbinds from all bindings
	~Bindable();

	// Gets the value
	const T& get() const;
	const T& set(T v);

	Bindable<T>& operator=(T v);
	const T* operator->() const;

	// Copies other to this, calls value changed handler if needed
	void bindTo(Bindable<T>& other);
	void unbindFrom(Bindable<T>& other);
	void unbindAll();

	void valueChanged(const T& old, const T& $new) const;
	void
	onValueChanged(std::function<void(const T& old, const T& $new)> handler);

	Bindable<T>& operator=(Bindable<T>& other) = delete;
	Bindable<T>& operator=(Bindable<T>&& other);

private:
	std::vector<Bindable<T>*> bindings{};
	T value{};
	std::function<void(const T& old, const T& $new)> valueChangedHandler{};

	void updateBindings(Bindable<T>* from, Bindable<T>* to);
};

template <typename T>
Bindable<T>::Bindable(Bindable<T>& other)
{
	bindTo(other);
}

template <typename T>
Bindable<T>::Bindable(Bindable<T>&& other) :
	bindings(std::move(other.bindings)),
	value(std::move(other.value))
{
	updateBindings(&other, this);
}

template <typename T>
Bindable<T>::~Bindable()
{
	unbindAll();
}

template <typename T>
Bindable<T>& Bindable<T>::operator=(Bindable<T>&& other)
{
	if (this == &other)
		return *this;
	value = std::move(other.value);
	bindings = std::move(other.bindings);
	updateBindings(&other, this);
	return *this;
}

template <typename T>
const T* Bindable<T>::operator->() const
{
	return &value;
}

template <typename T>
const T& Bindable<T>::get() const
{
	return value;
}

template <typename T>
const T& Bindable<T>::set(T v)
{
	if (value != v)
	{
		valueChanged(value, v);
		for (const auto& binding : bindings)
		{
			binding->value = v;
			binding->valueChanged(value, v);
		}
		value = std::move(v);
	}
	return value;
}

template <typename T>
Bindable<T>& Bindable<T>::operator=(T v)
{
	set(v);
	return *this;
}

template <typename T>
void Bindable<T>::bindTo(Bindable<T>& other)
{
	assert(&other != this);
	assert(std::ranges::find(bindings, &other) == bindings.end());
	bindings.push_back(&other);
	other.bindings.push_back(this);
	if (value != other.value)
	{
		valueChanged(value, other.value);
		value = other.value;
	}
}

template <typename T>
void Bindable<T>::unbindFrom(Bindable<T>& other)
{
	assert(&other != this);
	const auto it = std::ranges::find(bindings, &other);
	assert(it != bindings.end());
	bindings.erase(it);
	other.bindings.erase(std::ranges::find(other.bindings, this));
}

template <typename T>
void Bindable<T>::unbindAll()
{
	for (const auto& binding : bindings)
	{
		const auto it = std::ranges::find(binding->bindings, this);
		assert(it != binding->bindings.end());
		binding->bindings.erase(it);
	}
	bindings.clear();
}

template <typename T>
void Bindable<T>::valueChanged(const T& old, const T& $new) const
{
	if (valueChangedHandler)
		valueChangedHandler(old, $new);
}

template <typename T>
void Bindable<T>::onValueChanged(
	std::function<void(const T& old, const T& $new)> handler)
{
	valueChangedHandler = std::move(handler);
}

template <typename T>
void Bindable<T>::updateBindings(Bindable<T>* from, Bindable<T>* to)
{
	assert(from != to);
	for (const auto& binding : bindings)
	{
		auto it = std::ranges::find(binding->bindings, from);
		assert(it != binding->bindings.end());
		*it = to;
	}
}