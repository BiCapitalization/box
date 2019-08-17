#ifndef BEN_BOX_HPP
#define BEN_BOX_HPP

#include <functional>
#include <memory>
#include <optional>
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
    class box; 

    template <typename T, typename Allocator>
    void swap(box<T, Allocator>& a, box<T, Allocator>& b); 

    template <typename T, typename Allocator>
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
        using iterator = T*;
        using const_iterator = T const*;

        private:
        pointer m_ptr = nullptr;
        bool m_has_value = false;
        allocator_type m_alloc;

        explicit box(pointer ptr, allocator_type const& alloc)
            : m_ptr(ptr), m_alloc(alloc), m_has_value(true) {}

        template <typename... Args>
        void make_heap_value(Args&&... args) {
            auto ptr = m_traits::allocate(m_alloc, 1);
            m_traits::construct(m_alloc, detail::to_address(ptr), std::forward<Args>(args)...);
            m_ptr = ptr;
            m_has_value = true;
        } 

        template <typename... Args>
        void replace_heap_value(Args&&... args) {

            if (m_ptr != nullptr) {
                if (m_has_value) {
                    value() = value_type(std::forward<Args>(args)...);
                } else {
                    m_traits::construct(m_alloc, detail::to_address(m_ptr), std::forward<Args>(args)...);
                    m_has_value = true;
                }

            } else {
                make_heap_value(std::forward<Args>(args)...);
            }
        }

        void memory_cleanup() {
            if (m_ptr != nullptr) {
                m_traits::deallocate(m_alloc, detail::to_address(m_ptr), 1);
                m_ptr = nullptr;
            }
        }

        void full_cleanup() {
            erase();
            memory_cleanup();
        }

        template <typename U> 
        friend auto from_raw(U* ptr) -> box<U>; 

        friend void swap<T, Allocator>(box<T, Allocator>& a, box<T, Allocator>& b);

        public:
        box() {}
        explicit box(allocator_type const& alloc) : m_alloc(alloc) {}

        explicit box(T const& element, allocator_type const& alloc = Allocator()) : m_alloc(alloc) {
            make_heap_value(element);
        }

        explicit box(T&& element, allocator_type const& alloc = Allocator()) : m_alloc(alloc) {
            make_heap_value(std::move(element));
        }

        box(box const& other)
            : m_alloc(m_traits::select_on_container_copy_construction(other.m_alloc)) { 

            if (other.m_has_value) {
                make_heap_value(other.value());
            }
        }

        box(box&& other) {
            swap(*this, other);
        }

        ~box() {
            erase();

            if (m_ptr != nullptr) {
                m_traits::deallocate(m_alloc, detail::to_address(m_ptr), 1);
            }
        }

        auto operator=(box const& other) -> box& {
            if (m_traits::propagate_on_container_copy_assignment::value || m_alloc != other.m_alloc) {
                memory_cleanup();

                m_alloc = other.m_alloc;
                if (other.has_value()) {
                    make_heap_value(other.value());
                }
            } else {
                if (other.has_value()) {
                    replace_heap_value(other.value());
                } else {
                    erase();
                }
            }

            return *this;
        }

        auto operator=(box&& other) -> box& {
            using std::swap;

            if (m_traits::is_always_equal::value || m_alloc == other.m_alloc) {
                full_cleanup();

                swap(m_ptr, other.m_ptr);
                swap(m_has_value, other.m_has_value);
            } else if constexpr (m_traits::propagate_on_container_move_assignment::value) {
                full_cleanup();

                swap(m_ptr, other.m_ptr);
                swap(m_has_value, other.m_has_value);
                m_alloc = std::move(other.m_alloc);
            } else {
                if (other.has_value()) {
                    replace_heap_value(std::move(other.value()));
                } else {
                    erase();
                }
            }

            return *this;
        }

        auto get_allocator() const -> allocator_type {
            return m_alloc;
        }

        auto value() -> reference {
            return *m_ptr;
        }

        auto value() const -> const_reference {
            return *m_ptr;
        }

        auto operator*() -> reference {
            return value();
        }

        auto operator*() const -> const_reference {
            return value();
        }

        auto safe_value() -> std::optional<std::reference_wrapper<value_type>> {
            if (m_has_value) {
                return value();
            }

            return std::nullopt;
        }

        auto safe_value() const -> std::optional<std::reference_wrapper<value_type const>> {
            if (m_has_value) {
                return value();
            }

            return std::nullopt;
        }

        auto has_value() const -> bool {
            return m_has_value;
        }

        auto size() const -> size_type {
            return m_has_value ? 1 : 0;
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

        void erase() {
            if (!m_has_value) {
                return;
            }

            m_traits::destroy(m_alloc, detail::to_address(m_ptr));
            m_has_value = false;
        }

        auto begin() -> iterator {
            if (!m_has_value) {
                return nullptr;
            }

            return detail::to_address(m_ptr);
        }

        auto begin() const -> const_iterator {
            if (!m_has_value) {
                return nullptr;
            }

            return detail::to_address(m_ptr);
        }

        auto end() -> iterator {
            if (!m_has_value) {
                return nullptr;
            }
            
            return detail::to_address(m_ptr) + 1;
        }

        auto end() const -> const_iterator {
            if (!m_has_value) {
                return nullptr;
            }
            
            return detail::to_address(m_ptr) + 1;
        }

        auto cbegin() const -> const_iterator {
            return begin();
        }

        auto cend() const -> const_iterator {
            return end();
        }
    };

    template <typename U> 
    auto from_raw(U* ptr) -> box<U> {
       return box<U>(ptr, std::allocator<U>()); 
    }

    template <typename T, typename Allocator>
    void swap(box<T, Allocator>& a, box<T, Allocator>& b) {
        using std::swap;

        if constexpr (std::allocator_traits<Allocator>::propagate_on_container_swap::value) {
            swap(a.m_alloc, b.m_alloc);
        }

        swap(a.m_ptr, b.m_ptr);
        swap(a.m_has_value, b.m_has_value);
    }
}

#endif // BEN_BOX_HPP
