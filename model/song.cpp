#include "song.h"
#include "../sqlite/sqlite.h"

Song::Song(Row&& row) {
    if (auto const f = row["id"])
        id_ = f->value().value<i64>();
    if (auto const f = row["pid"])
        pid_ = f->value().value<i64>();
    if (auto const f = row["path"])
        path_ = f->value().value<std::string>();
}


bool Song::insert() noexcept {
    static auto const query{"INSERT INTO song (pid, path) VALUES(?,?)"s};
    if (auto const id = SQLite::self().insert(query, pid_, path_); id > 0) {
        id_ = id;
        return true;
    }
    return {};
}

bool Song::update() const noexcept {
    static auto const query{"UPDATE song SET pid=?, path=? WHERE id=?"s};
    return SQLite::self().update(query, pid_, path_, id_);
}


bool Song::create_table() noexcept {
    for (auto const& cmd : CreateSongsCmd)
        if (!SQLite::self().exec(cmd))
            return {};
    return true;
}

auto Song::with_id(i64 id) noexcept
    -> std::optional<Song> {
    static auto const query{"SELECT * FROM song WHERE id=?"s};
    if (auto const result = SQLite::self().select(query, id))
        if (result->size() == 1)
            return Song(result.value()[0]);
    return {};
}

auto Song::all_for(i64 const pid) noexcept
    -> std::vector<Song> {
    std::vector<Song> data{};

    static auto const query{"SELECT * FROM song WHERE pid=?"s};
    if (auto result = SQLite::self().select(query, pid)) {
        for (auto&& row : result.value())
            data.emplace_back(std::move(row));
        return data;
    }
    return {};
}

auto Song::remove(i64 const id) noexcept
    -> bool {
    return SQLite::self().exec("DELETE FROM playlist WHERE id=?", id);
}
