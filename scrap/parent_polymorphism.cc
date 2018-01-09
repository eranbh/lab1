#include <memory>

namespace type_traits
{

template <typename Type>
struct remove_reference_wrapper
{
    using type = Type;
};

template <typename Type>
struct remove_reference_wrapper<std::reference_wrapper<Type>>
{
    using type = Type;
};

template <typename Type>
using remove_reference_wrapper_t = typename remove_reference_wrapper<Type>::type;

} // type_traits

class a
{
public:
    template <typename Listener>
    void set_listener(Listener && listener)
    {
        m_listener = std::make_unique<derived_listener>(
            std::forward<Listener>(listener));
    }

private:
    class listener
    {
    public:
        virtual void foo() = 0;
        virtual ~listener() = default;
    };

    template <typename Listener>
    class derived_listener : public listener
    {
    public:
        derived_listener(Listener && user_listener) :
            listener(std::forward<Listener>(user_listener))
        {
        }

        void foo() override
        {
            return static_cast<type_traits::remove_reference_wrapper_t<Listener> &>
                (m_listener).foo();
        }

        ~listener() override = default;

    private:
        Listener m_listener;
    };

    std::unique_ptr<listener> m_listener{};
};


class my_a_listener
{
public:
    void foo()
    {
    }
};

void foo()
{
    my_a_listener l1;
    a a1;
    a1.set_listener(std::ref(l1));

}

