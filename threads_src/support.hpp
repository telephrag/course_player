
#pragma once

#include <memory>
#include "playlist.hpp"

Playlist support(std::string location)
{
    auto playlist = Playlist(location);
    
    return playlist;
}
