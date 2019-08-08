#ifndef BEN_BOX_HPP
#define BEN_BOX_HPP

#include <memory>
#include <utility>

namespace ben {

    namespace detail {
        // Thanks to p0653 for providing this idiom. 
        // C++20: Replace this implementation with `std::to_address`.
        template <typename T>
        auto to_address(T const& ptr) noexcept {
            return to_address(ptr.operator->());
        }

        template <typename T>
        auto to_address(T* ptr) noexcept -> T* {
            return ptr;
        }
    }

    template <typename T, typename Allocator = std::allocator<T>>
    class box {
        private: 
        using m_traits = std::allocator_traits<Allocator>;

        public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = typename m_traits::size_type;
        using difference_type = typename m_traits::difference_type;
        using reference = T&;
        using const_reference = T const&;
        using pointer = typename m_traits::pointer;
        using const_pointer = typename m_traits::const_pointer;
        using iterator = pointer;
        using const_iterator = const_pointer;

        private:
        pointer m_ptr = nullptr;
        allocator_type m_alloc;

        template <typename... Args>
        void make_heap_value(Args&&... args) {
            auto ptr = m_traits::allocate(m_alloc, 1);
            m_traits::construct(m_alloc, detail::to_address(ptr), std::forward<Args>(args)...);
            m_ptr = ptr;
        } 

        template <typename... Args>
        void replace_heap_value(Args&&... args) {

            if (m_ptr != nullptr) {
                auto address = detail::to_address(m_ptr);

                m_traits::destroy(m_alloc, address);
                m_traits::construct(m_alloc, address, std::forward<Args>(args)...);
            } else {
                make_heap_value(std::forward<Args>(args)...);
            }
        }

        public:
        box() {}
        explicit box(allocator_type const& alloc) : m_alloc(alloc) {}

        explicit box(T const& element, allocator_type const& alloc = Allocator()) : m_alloc(alloc) {
            make_heap_value(element);
        }

        explicit box(T&& element, allocator_type const& alloc = Allocator()) : m_alloc(alloc) {
            make_heap_value(std::move(element));
        }

        auto get_allocator() const -> allocator_type {
            return m_alloc;
        }

        auto get() -> reference {
            return *m_ptr;
        }

        auto get() const -> const_reference {
            return *m_ptr;
        }

        auto has_value() const -> bool {
            return m_ptr != nullptr;
        }

        auto size() const -> size_type {
            return m_ptr == nullptr ? 0 : 1;
        }

        template <typename... Args>
        void emplace(Args&&... args) {
            replace_heap_value(std::forward<Args>(args)...);           
        }
        
        void push(T const& val) {
            replace_heap_value(val);
        }

        void push(T&& val) {
            replace_heap_value(std::move(val));
        }

        auto begin() -> iterator {
            return detail::to_address(m_ptr);
        }

        auto begin() const -> const_iterator {
            return detail::to_address(m_ptr);
        }

        auto end() -> iterator {
            auto address = detail::to_address(m_ptr);
            if (address == nullptr) {
                return nullptr;
            }

            return address + 1;
        }

        auto end() const -> const_iterator {
            auto address = detail::to_address(m_ptr);
            if (address == nullptr) {
                return nullptr;
            }

            return address + 1;
        }

        auto cbegin() const -> const_iterator {
            return begin();
        }

        auto cend() const -> const_iterator {
            return end();
        }
    };
}

#endif // BEN_BOX_HPP
