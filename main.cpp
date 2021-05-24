
#include <iostream>
#include <thread>

#include <vlc/vlc.h>
#include <vlcpp/vlc.hpp>

#include "threads_src/support.hpp"
#include "threads_src/play.hpp"
#include "threads_src/video_controls.hpp"
#include "threads_src/common.hpp"


int main() {
    std::thread play_thread(play_music); 
    std::thread control_thread(listen_for_input);
    
    // TODO: Encapsulate player and playlist into a single class?
    
    play_thread.join();
    std::cin.get();
    
    return 0;
}
