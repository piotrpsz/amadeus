#pragma once
#include "../sqlite/row.h"
#include <string>
#include <string_view>
#include <cstdint>
#include <optional>
#include <format>
#include <iostream>


class Playlist {
    using i64 = uint64_t;
    inline static std::string const CreatePlayistCmd = R"(
        CREATE TABLE playlist (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL
        )
    )";

    i64 id_{};
    std::string name_;

public:
    explicit Playlist(std::string_view str) : name_{str} {}
    explicit Playlist(Row&&);

    bool save() noexcept { return (id_ > 0) ? update() : insert(); }
    bool insert() noexcept;
    bool update() const noexcept;

    i64 id() const noexcept {
        return id_;
    }
    std::string const& name() const noexcept {
        return name_;
    }
    std::string to_str() const noexcept {
        return std::format("[id: {}, name: {}]", id_, name_);
    }
    void show() const noexcept {
        std::cout << std::format("playlist: {}\n", to_str()) << std::flush;
    }

    static bool create_table() noexcept;
    static std::optional<Playlist> with_id(i64 id) noexcept;
    static std::vector<Playlist> all() noexcept;
    static bool remove(i64 id) noexcept;
    static int count_for(std::string_view str) noexcept;


};
