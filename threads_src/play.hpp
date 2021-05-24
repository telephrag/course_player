
#pragma once

#define PY_SSIZE_T_CLEAN
#include <python3.9/Python.h>

#include <memory>
#include <future>

#include <vlcpp/vlc.hpp>

#include "support.hpp"
#include "video_controls.hpp"


extern std::mutex mtx;
extern unsigned int current_played;
extern Playlist current_playlist;
extern std::condition_variable cond_var;
extern bool input_sent;


void wait_for_duration(
    std::promise<int>&& duration_promise,
    VLC::Media&& media
)
{
    const int n = 5;
    
    int duration = media.duration();
    while (duration <= 0)
    {
        duration = media.duration();
        std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
    }
    duration_promise.set_value(duration);
}


void py_init()
{
    
}


std::string get_play_mrl (
    const std::string mrl
    )
{
    Py_Initialize();
    
    PyObject* py_module_name = PyUnicode_FromString((char*)"get_str");     
    PyObject* py_module      = PyImport_Import( py_module_name );  

    if (!py_module)
    {
        std::cout << "Error importing module.\n";
        return "Error";
    }

    PyObject* function = PyObject_GetAttrString(py_module, (char*)"get_play_url");  
    
    const char* mrl_c = mrl.c_str();                                                
    PyObject* args = PyTuple_Pack(1, PyUnicode_FromString(mrl_c));       
    
    PyObject* result = PyObject_CallObject(function, args);                         
    
    std::cout << "Module: " << Py_REFCNT(py_module) << "\nFunction: " << Py_REFCNT(function) << "\nResult: " << Py_REFCNT(result) << "\n";
    Py_ssize_t* size = nullptr; 
    wchar_t* play_mrl_wchar = PyUnicode_AsWideCharString(result, size); // TODO free memory

    std::wstring ws( play_mrl_wchar );
    std::string play_mrl_str( ws.begin(), ws.end() );
    delete [] play_mrl_wchar;
    
    Py_XDECREF(py_module_name);
    Py_XDECREF(py_module);
    Py_XDECREF(function);
    Py_XDECREF(args);
    Py_XDECREF(result);
    
    std::cout << "Module: " << Py_REFCNT(py_module) << "\nFunction: " << Py_REFCNT(function) << "\nResult: " << Py_REFCNT(result) << "\n";
    
    std::cout << play_mrl_str << "\n";
    return play_mrl_str;
//     return song.subitems()->itemAtIndex(0);
}


void play_music()
{ 
    current_playlist = Playlist("");
    std::shared_ptr<VLC::Instance> instance;
    
    #define custom_logs 1
    #if custom_logs
        const char *const arg[] = {"--preferred-resolution=720", "--no-video", "--loop"};
        instance = std::make_shared<VLC::Instance>(VLC::Instance(3, arg));
    #else
        const char *const arg[] = {"--preferred-resolution=720", "--no-video", "-vv", "--loop"};
        instance = std::make_shared<VLC::Instance>(VLC::Instance(4, arg));
    #endif
        
    std::unique_lock<std::mutex> ulm (mtx); 
    
    while (true)
    {
        std::this_thread::sleep_for (std::chrono::seconds(1) );
        cond_var.wait(
            ulm,
            []{ return (current_playlist.get_length() > 0); }
        );
        
        while(true)
        {
            input_sent = false;
            std::cout << "Curerntly playing: " << current_played << "\n";
            
            std::string song_mrl = current_playlist.get(current_played);
            
            VLC::Media song = VLC::Media(*instance, get_play_mrl(song_mrl), VLC::Media::FromLocation);
            auto player = std::make_shared<VLC::MediaPlayer>(song);
            player->play();
//             std::this_thread::sleep_for(std::chrono::seconds(30));

            
            std::promise<int> duration_promise;
            std::future<int> duration_future = duration_promise.get_future();
            std::thread duration_waiter(wait_for_duration, std::move(duration_promise), song);
            
            while (duration_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
            {
                std::cout << "Waiting for duration...\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            
            int duration = duration_future.get(); // TODO make use of future
            duration_waiter.join();
            
                                                  // UPD: using futures results in terminate without exception, investigate
            cond_var.wait_for(
                ulm,
                std::chrono::milliseconds(duration),
                [] { return input_sent; }
            );

            if (input_sent)
            {
                input_sent = false;
                continue;
            }

            next_track();
        }
    }
}


