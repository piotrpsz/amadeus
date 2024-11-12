#include "selection.h"
#include "song.h"
#include "playlist.h"
#include "../shared/event_controller.hh"
#include <iostream>
#include <format>
using namespace std;


bool Selection::save_as_playlist(string&& name) noexcept {
    if (empty()) {
        cout << "No selection was made\n" << flush;
        return {};
    }

    auto playlist_name = name;
    if (Playlist::count_for(std::move(name))) {
        cout << format("Playlist with the name already exists: {}\n", playlist_name) << flush;
        return {};
    }
    auto playlist = Playlist{playlist_name};
    if (!playlist.save()) {
        cout << format("Can't save the playlist: {}\n", playlist.name()) << flush;
        return {};
    }

    auto const data = to_vector();
    for (auto const& it : data)
        if (!Song(playlist.id(), it).save())
            return {};

    EventController::self().send(event::NewPlaylistAdded, QString::fromStdString(playlist_name));
    return true;
}
