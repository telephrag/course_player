
#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <map>
#include <array>

#include <vlcpp/vlc.hpp>

#include "playlist.hpp"

std::mutex mtx;
std::condition_variable cond_var;

std::shared_ptr<VLC::Instance> instance; 
std::shared_ptr<VLC::MediaPlayer> player; 

Playlist current_playlist;
unsigned int current_played = 0;
bool input_sent = false;
std::string current_input = "";

const int INPUT_SIGNAL_COUNT = 3;
std::array<std::string, INPUT_SIGNAL_COUNT> input_signals = { "next", "prev", "playlist" };
enum Input { next, prev, playlist };

std::map<std::string, Input> input_map;


