#pragma once

template <typename T>
class Singleton
{
public:
    // Constructor.
    // Sets up the instance reference
    Singleton()
    {
        instance_ = static_cast<T*>(this);
    }

    // Prevent the singleton from being moved or copied
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton) = delete;
    Singleton& operator=(Singleton&&) = delete;

    // Gets a pointer to the singleton instance
    static T* instance()
    {
        return Singleton<T>::instance_;
    }

private:
    static T* instance_;
};

// Required to prevent unresolved external symbol errors.
template<typename T>
T* Singleton<T>::instance_;
