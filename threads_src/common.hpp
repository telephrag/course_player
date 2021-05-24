
#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <map>
#include <array>

#include <vlcpp/vlc.hpp>

#include "Playlist.hpp"

std::mutex mtx;
std::condition_variable cond_var;

Playlist current_playlist;
unsigned int current_played = 0;
bool input_sent = false;
