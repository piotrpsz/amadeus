#pragma once
#include "../sqlite/row.h"
#include <string>
#include <cstdint>
#include <optional>


class Playlist {
    using i64 = uint64_t;
    inline static std::string const CreatePlayistCmd = R"(
        CREATE TABLE playlist (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL
        )
    )";

    i64 id_;
    std::string name_;

public:
    explicit Playlist(Row&&);
    bool save() noexcept { return (id_ > 0) ? update() : insert(); }
    bool insert() noexcept;
    bool update() const noexcept;


    static bool create_table() noexcept;
    static std::optional<Playlist> with_id(i64 id) noexcept;
    static std::vector<Playlist> all() noexcept;
    static bool remove(i64 id) noexcept;
};
