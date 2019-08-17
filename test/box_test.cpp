#include "box.hpp"
#include <catch2/catch.hpp>

#include <string_view>
#include <string>
#include <utility>

template <typename T>
static void value_check(ben::box<T> const& box) {
    REQUIRE(box.has_value());
    REQUIRE(box.size() == 1);
}

TEST_CASE("Constructors") {
    SECTION("Default construction") {
        auto box = ben::box<int>();

        REQUIRE(box.size() == 0);
        REQUIRE(!box.has_value());
    }

    auto literal = std::string_view("Hello there, friend!");

    SECTION("Copy construction") {
        auto box = ben::box<int>();
        auto cpy = box;

        REQUIRE(!cpy.has_value());
        REQUIRE(cpy.size() == 0);

        auto box_2 = ben::box(5);
        auto cpy_2 = box_2;

        value_check(cpy_2);
        REQUIRE(cpy_2.value() == box_2.value());
    }

    SECTION("Move construction") {
        auto box = ben::box<int>();
        auto other = std::move(box);

        REQUIRE(!other.has_value());
        REQUIRE(other.size() == 0);

        auto box_2 = ben::box(5);
        auto other_2 = std::move(box_2);

        value_check(other_2);
        REQUIRE(other_2.value() == 5);
    }

    SECTION("Element constructors - copy") {
        auto value = std::string(literal);
        auto box = ben::box(value);

        value_check(box);
        REQUIRE(box.value() == value);
    }

    SECTION("Element constructors - move") {
        auto value = std::string(literal);
        auto box = ben::box(std::move(value));

        value_check(box);
        REQUIRE(box.value() == literal);
    }

    SECTION("From raw pieces") {
        int value = 5;
        auto ptr = new int(value);

        auto box = ben::from_raw(ptr);

        value_check(box);
        REQUIRE(box.value() == value);
    }
}

TEST_CASE("Assignment operators") {
    int value = 5;
    auto box = ben::box<int>();
    auto box_2 = ben::box(value);

    SECTION("Copy assignment") {
        box = box_2;

        value_check(box);
        value_check(box_2);
        REQUIRE(box.value() == box_2.value());
    }

    SECTION("Move assignment") {
        box = std::move(box_2);

        value_check(box);
        REQUIRE(box.value() == value);

        REQUIRE(!box_2.has_value());
    }
}

TEST_CASE("Accessors") {
    int value = 5;
    auto box = ben::box(value);

    value_check(box);
    REQUIRE(box.value() == value);
    REQUIRE(*box == value);
    
    auto opt_val = box.safe_value();
    REQUIRE(opt_val.has_value());
    REQUIRE(*opt_val == value);

    auto empty_box = ben::box<int>();
    REQUIRE(!empty_box.safe_value().has_value());
}

TEST_CASE("Modifiers") {
    SECTION("push") {
        int value = 5;
        int value_2 = -3;

        auto box = ben::box<int>();
        box.push(value);

        value_check(box);
        REQUIRE(box.value() == value);

        box.push(value_2);

        value_check(box);
        REQUIRE(box.value() == value_2);
    }

    SECTION("emplace") {
        auto literal = std::string_view("What a nice day it is!");
        auto literal_2 = std::string_view("Oh, indeed. Tomorrow, however, rain is expected.");
        auto box = ben::box<std::string>();

        box.emplace(literal);

        value_check(box);
        REQUIRE(box.value() == std::string(literal));

        box.emplace(literal_2);
        
        value_check(box);
        REQUIRE(box.value() == std::string(literal_2));
    }

    SECTION("erase") {
        auto box = ben::box(5);
        
        value_check(box);
        
        box.erase();

        REQUIRE(!box.has_value());
        REQUIRE(box.size() == 0);
    }
}

TEST_CASE("Iterators") {
    auto empty_box = ben::box<int>();

    REQUIRE(empty_box.begin() == empty_box.end());
    REQUIRE(empty_box.cbegin() == empty_box.cend());

    auto box = ben::box(5);

    REQUIRE(box.begin() < box.end());
    REQUIRE(box.cbegin() < box.cend());
    REQUIRE(box.begin() + 1 == box.end());
    REQUIRE(box.cbegin() + 1 == box.cend());
}
