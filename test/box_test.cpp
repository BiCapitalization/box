#include "box.hpp"
#include <catch2/catch.hpp>

#include <string_view>
#include <string>

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

    SECTION("Element constructors - copy") {
        auto value = std::string(literal);
        auto box = ben::box(value);

        value_check(box);
        REQUIRE(box.get() == value);
    }

    SECTION("Element constructors - move") {
        auto value = std::string(literal);
        auto box = ben::box(std::move(value));

        value_check(box);
        REQUIRE(box.get() == literal);
    }
}

TEST_CASE("Modifiers") {
    SECTION("push") {
        int value = 5;
        int value_2 = -3;

        auto box = ben::box<int>();
        box.push(value);

        value_check(box);
        REQUIRE(box.get() == value);

        box.push(value_2);

        value_check(box);
        REQUIRE(box.get() == value_2);
    }

    SECTION("emplace") {
        auto literal = std::string_view("What a nice day it is!");
        auto literal_2 = std::string_view("Oh, indeed. Tomorrow, however, rain is expected.");
        auto box = ben::box<std::string>();

        box.emplace(literal);

        value_check(box);
        REQUIRE(box.get() == std::string(literal));

        box.emplace(literal_2);
        
        value_check(box);
        REQUIRE(box.get() == std::string(literal_2));
    }
}
