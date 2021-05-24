
#pragma once

#include "support.hpp"
#include "manage_db.hpp"

enum Input 
{ 
    next, 
    prev, 
    stop,
    playlist 
};

const int INPUT_SIGNAL_COUNT = 4;
const std::array<std::string, INPUT_SIGNAL_COUNT> input_signals{{ "next", "prev", "pause", "playlist" }};

std::map<std::string, Input> input_map = {
        { "next",     Input::next     }, 
        { "prev",     Input::prev     },
        { "pause",    Input::stop     },
        { "playlist", Input::playlist }
    };
    
std::string current_input = "";
extern bool input_sent;
    
extern std::mutex mtx;
extern std::condition_variable cond_var;

extern unsigned int current_played;
extern Playlist current_playlist;

void next_track();
void prev_track();
void pause_playback();
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
            current_input = "";
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
            //pause_playback();
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

void pause_playback()
{
    std::cout << "Pausign playback.\n";
}

void set_playlist()
{
    std::cout << "Changing current playlist.\n";
    manage_playlists();
    current_played = 0;
    //input_sent = false;
}

