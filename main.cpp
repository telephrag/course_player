
#include <iostream>
#include <thread>

#include <vlc/vlc.h>
#include <vlcpp/vlc.hpp>

#include "threads_src/support/support.hpp"

int main() {
    std::string location = "home/trofchik/Documents/cpp/coursework_actual/threads_src/support/sample_playlist";
    
    std::promise<Playlist> playlist_promise;
    std::future<Playlist> playlist_future = playlist_promise.get_future();
    
    std::thread suport_thread(support, std::move(playlist_promise), location);
    
    Playlist playlist = playlist_future.get();
    playlist.parse_file_with_urls();
    playlist.print();
    
    suport_thread.join();
    
    std::cin.get();
    
    return 0;
}
