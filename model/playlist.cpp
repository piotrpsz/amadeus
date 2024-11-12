#include "playlist.h"
#include "../sqlite/sqlite.h"
#include <format>

using namespace std;
using namespace std::literals::string_literals;


Playlist::Playlist(Row&& row) {
    if (auto const f = row["id"])
        id_ = f->value().value<i64>();
    if (auto const f = row["name"])
        name_ = f->value().value<std::string>();

}

bool Playlist::insert() noexcept {
    static auto const query{"INSERT INTO playlist (name) VALUES(?)"s};
    if (auto const id = SQLite::self().insert(query, name_); id > 0) {
        id_ = id;
        return true;
    }
    return {};
}

bool Playlist::update() const noexcept {
    static auto const query{"UPDATE playlist SET name=? WHERE id=?"s};
    return SQLite::self().update(query, name_, id_);
}

bool Playlist::create_table() noexcept {
    std::string cmd = std::string(CreatePlayistCmd);
    if (SQLite::self().exec(cmd))
        return true;
    return {};
}

 auto Playlist::with_id(i64 id) noexcept
-> std::optional<Playlist> {
    static auto const query{"SELECT * FROM playlist WHERE id=?"s};
    if (auto const result = SQLite::self().select(query, id))
        if (result->size() == 1)
            return Playlist(result.value()[0]);
    return {};
}

auto Playlist::all() noexcept
-> std::vector<Playlist> {
    std::vector<Playlist> data{};

    static auto const query{"SELECT * FROM playlist"s};
    if (auto result = SQLite::self().select(query)) {
        for (auto&& row : result.value())
            data.emplace_back(std::move(row));
        return data;
    }
    return {};
}

auto Playlist::remove(i64 const id) noexcept
-> bool {
    return SQLite::self().exec("DELETE FROM playlist WHERE id=?", id);
}

int Playlist::count_for(string_view str) noexcept {
    auto const query = format("SELECT COUNT(*) as count FROM playlist WHERE name=?");
    if (auto const result = SQLite::self().select(query, str)) {
        if (result->size() == 1) {
            if (auto const f = (*result)[0]["count"])
                return f->value().value<i64>();
        }
    }
    return 0;
}
