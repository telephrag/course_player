
#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <map>

#include <vlcpp/vlc.hpp>

#include "playlist.hpp"

std::mutex mxt;
std::condition_variable cond_var;

std::shared_ptr<VLC::Instance> instance; 
std::shared_ptr<VLC::MediaPlayer> player; 

Playlist current_playlist;
int current_played = 0;

enum Input
{
    next, 
    prev,
    stop,
    playlist
};

std::map<std::string, Input> input_map;


