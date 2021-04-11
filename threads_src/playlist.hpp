
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class Playlist // TODO: Maybe I should use json 
{
private:
    
    std::vector<std::string> url_vector;
    std::string location = ""; // NOTE: File with URL must be placed into executable directory (build in this case)

public:
    
    Playlist(std::string location = "")
    {
        this->location = location;
    }
    
    
    void parse_file_with_urls()
    {
        if (location.length() == 0)
            throw "No file location was specified. Aborting.";
        
        std::ifstream file;
        file.open(location, std::ios::in);
        
        if (!file)
        {
            std::cout << "Error opening file. Abborting. \n";
            return;
        }
        
        std::string temp = "";
        while (!file.eof())
        {
            std::getline(file, temp);
            url_vector.push_back(temp);
        }
    }
    
    
    void print()
    {
        for (auto song: this->url_vector)
        {
            std::cout << song << "\n";
        }
    }
    
    
    std::vector<std::string> get_result()
    {
        return this->url_vector;
    }
};
