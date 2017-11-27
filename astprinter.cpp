#include <iostream>
#include <sstream>
#include <utility>
#include <memory>
#include <string>
#include "catch.hpp"
#include "StringBuffer.h"
#include "parser.h"
#include "util.h"
#include "ASTVisitor.h"

using namespace halang;

int main(int argc, char * argv[]) {
    Parser parser;

    while (!std::cin.eof()) {
        auto str = std::make_shared<std::string>();
        std::getline(std::cin, *str);
        *str += "\n";
        parser.AddBuffer(str);
    }

    parser.ParseProgram();

    if (parser.IsOK()) {
        auto root = parser.GetRoot();

        ASTVisitor astVisitor;
        astVisitor.Visit(root);
    } else {
        for (auto i = parser.getMessages().begin();
        i != parser.getMessages().end(); i++) {
            std::cout << i->msg << std::endl;
        }

        return -1;
    }

    return 0;
}
