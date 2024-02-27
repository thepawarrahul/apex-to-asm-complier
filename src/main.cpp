#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include <optional>

enum class TokenType {
    sys_debug,
    open_par,
    close_par,
    init_lit,
    semi
};

struct Token {
    TokenType type;
    std::string value = "";
};


std::vector<Token> tokenize(const std::string& str) {
    std::string buff;
    std::vector<Token> tokens;
    for(int i = 0; i < str.length(); i++) {
        
        char c = str.at(i);

        if (std::isalpha(c)) {
            buff.push_back(c);
            i++;

            while (std::isalnum(str.at(i))) {
                buff.push_back(str.at(i));
                i++;
            }
            i--;
            
            if (buff == "debug") {
                tokens.push_back({
                    .type = TokenType::sys_debug
                });
                buff.clear();
                continue;

            } else {
                std::cerr << "Wrong Syntax " << std::endl;
                exit (EXIT_FAILURE); 
            }
        }

        else if (std::isspace(c)) {
            continue;
        }

        else if (c == '(') {
            tokens.push_back({
                .type = TokenType::open_par
            });
            continue;
        }

        else if (c == ')') {
            tokens.push_back({
                .type = TokenType::close_par
            });
            continue;
        }

        else if (std::isdigit(c)) {
            buff.push_back(c);
            i++;

            while(std::isdigit(str.at(i))){
                buff.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.push_back({
                .type = TokenType::init_lit,
                .value = buff
            });
            buff.clear();
        }

        else if (c == ';') {
            tokens.push_back({
                .type = TokenType::semi
            });
        } else {
            std::cerr << "Wrong Syntax.." << std::endl;
            exit(EXIT_FAILURE);
        }
        return tokens;
    }
}

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output;

    output << "global _start\n";
    output << "section .data\n";

    for (int i = 0; i< tokens.size(); i++) { 
        const Token& token = tokens.at(i);

        if (token.type == TokenType::sys_debug) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type==TokenType::open_par) {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::init_lit) {
                    output << "    num db \'" << tokens.at(i+2).value << "\', 0xa\n";
                }
            }
        }
    }

    output << "section .text\n_start:\n";
    output << "   mov rax, 1\n";
    output << "   mov rdi, 1\n";
    output << "   mov rsi, num\n";
    output << "   mov rdx, 3\n";
    output << "   syscall\n";
    output << "   mov rax, 60\n";
    output << "   mov rdi, rdi\n";
    output << "   syscall\n";

    return output.str(); 
} 

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

    std::vector<Token> tokens = tokenize(contents);

    {
        std::fstream file("./out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }
    
    system("nasm -felf64 out.asm -o out.o");
    system("ld out.o -o out");
    system("./out");

    return EXIT_SUCCESS;
}
