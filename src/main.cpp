#include<iostream>
#include<fstream>
#include<sstream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "No file passed for execution.." << std::endl;
        std::cerr << "apex <input.apex>" << std::endl;
        return EXIT_FAILURE;
    }

    std::stringstream contents_stream;
    {
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
    }

    std::string contents = contents_stream.str();

    std::cout << contents << std::endl;
    return EXIT_SUCCESS;
}