#include <iostream>

#include <processing.h>

/*
The program counts the frequency of words found in all files in a directory
and displays the final statistics in the console.
*/

int main(int argc, char **argv) {
  if (argc != 2) {
    return 0;
  }
  
  Dir(argv[1], true) 
    | Filter([](std::filesystem::path& p){ return p.extension() == ".txt"; })
    | OpenFiles()
    | Split("\n ,.;")
    | Transform(
        [](std::string& token) { 
            std::transform(token.begin(), token.end(), token.begin(), [](char c){return std::tolower(c);});
            return token;
        })
    | AggregateByKey(
        std::size_t{0}, 
        [](const std::string&, size_t& count) { ++count;},
        [](const std::string& token) { return token;}
      )
    | Transform([](const std::pair<std::string, size_t>& stat) { return std::format("{} - {}", stat.first, stat.second);})
    | Out(std::cout);
  
  return 0;
}
