#pragma once

#include "../sqlite/row.h"
#include <string>
#include <vector>
#include <cstdint>

class Song {
    using i64 = uint64_t;
    inline static std::vector<std::string> const CreateSongsCmd{
        {
        R"(
            CREATE TABLE song (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            pid INTEGER NOT NULL,
            path TEXT NOT NULL
            );)"
        },
        {
        R"(
            CREATE UNIQUE INDEX pid_name ON songs(pid, path);
        )"
        }
    };
    i64 id_;
    i64 pid_;   // playlist id
    std::string path_;

public:
    explicit Song(Row&&);
    bool insert() noexcept;
    bool update() const noexcept;

    static bool create_table() noexcept;
    static std::optional<Song> with_id(i64 id) noexcept;
    static std::vector<Song> all_for(i64 pid) noexcept;
    static bool remove(i64 id) noexcept;
};
