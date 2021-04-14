
#pragma once

#include "common.hpp"

extern std::map<std::string, Input> input_map;
extern std::shared_ptr<VLC::MediaPlayer> player;
extern std::array<std::string, INPUT_SIGNAL_COUNT> input_signals;
extern std::mutex mtx;

void next_track();
void prev_track();
void stop_playback();
void set_playlist();

bool is_input_code(std::string code)
{
    for (auto iter : input_signals)
    {
        if (code.compare(iter) == 0) 
            return true;
    }
    
    return false;
}

void listen_for_input()
{
    
    for (;;)
    {
        std::string input = "";
        std::cin >> input;
        std::cin.clear();
        
        if (is_input_code(input))
        {
            std::lock_guard<std::mutex> lgm (mtx);
            input_sent = true;
            current_input = input;
            cond_var.notify_one();
        }
    }
}

void handle_input()
{
    switch (input_map[current_input])
    {
        case next:
            next_track();
            break;
        case prev:
            prev_track();
            break;
        case stop:
            stop_playback();
            break;
        case playlist:
            set_playlist();
            break;
    }
}

void next_track()
{
    std::cout << "Switching to next track.\n";
}

void prev_track()
{
    std::cout << "Switching to previous track.\n";
}

void stop_playback()
{
    std::cout << "Aborting playback.\n";
    player->stop();
}

void set_playlist()
{
    std::cout << "Changing current playlist.\n";
}

