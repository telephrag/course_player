
#pragma once

#include "common.hpp"
#include "video_controls.hpp"

extern std::map<std::string, Input> input_map;
extern std::shared_ptr<VLC::MediaPlayer> player;
extern std::array<std::string, INPUT_SIGNAL_COUNT> input_signals;
extern std::mutex mtx;

extern unsigned int current_played;
extern Playlist current_playlist;

void next_track();
void prev_track();
void set_playlist();
void handle_input();

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
            handle_input();
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
        case playlist:
            set_playlist();
            break;
    }
}

void next_track()
{
    std::cout << "Switching to next track.\n";
    if (current_played >= current_playlist.get_length() - 1)
    {
        current_played = 0;
    }
    else
    {
        current_played++;
    }
}

void prev_track()
{
    std::cout << "Switching to previous track.\n";
    if (current_played <= 0)
    {
        current_played = current_playlist.get_length() - 1;
    }
    else
    {
        current_played--;
    }
}

void set_playlist()
{
    std::cout << "Changing current playlist.\n";
    std::string next_playlist_location;
    std::cout << "Input the next playlist location: "; std::cin >> next_playlist_location;
    
    current_playlist = support(next_playlist_location);
    current_played = 0;
}

