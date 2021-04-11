
#pragma once

#include "common.hpp"

extern std::map<std::string, Input> input_map;

void listen_for_input()
{
    
    for (;;)
    {
        std::string input = "";
        std::cin >> input;
        
        switch (input_map[input])
        {
            case next:
            case prev:
            case stop:
            case playlist:
        }
    }
}
