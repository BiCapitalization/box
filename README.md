# box
A box type for modern C++.

Sometimes, all you need is an object on the heap, be it due to inheritance requirements or space management issues. Currently, many people resort to `std::unique_ptr`, but this actually poses a problem: Unique pointers are not copyable and are, as the name suggests, modeled after regular pointers, which is oftentimes not what you want or need. Instead, this sort of problem calls for a box type: A container that contains at most a single element, with all the conveniences and interfaces of regular containers built in.

## Usage
*This project is C++17 only.* The library is single header only. Just drop `include/box.hpp` into your project and you're good to go!

## Examples
_TODO_

## TODO

 - Expand the interface
 - Write more tests
 - Documentation
 - Implement EBO for stateless allocators
 - `noexcept` correctness and exception guarantees
 - Think of a better namespace name (suggestions welcome!)
