
#pragma once

#include <future>
#include <memory>
#include "playlist.hpp"

void support(std::promise<Playlist>&& playlist_promise, std::string location)
{
    auto *playlist = new Playlist(location);
    playlist_promise.set_value(*playlist);
}
