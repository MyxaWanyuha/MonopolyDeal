#include <iostream>
#include <array>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main()
{
    std::ifstream f("example.json");
    json data = json::parse(f);
    std::cout << "Hello World!\n";
}
