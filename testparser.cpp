#define CATCH_CONFIG_MAIN
#include <iostream>
#include <utility>
#include <memory>
#include <string>
#include "catch.hpp"
#include "StringBuffer.h"
#include "parser.h"
#include "util.h"
#include "ASTVisitor.h"

using namespace halang;

TEST_CASE( "Lexer", "[Lexer]" ) {

    Parser parser;

    const char* codes[3] = {
        "def main(a, b)\n",
        "   let c = a + b * 1\n",
        "   foo();\n"
        "end"
    };

    for (int i = 0; i < 3; i++) {
        parser.AddBuffer(
            std::make_shared<std::string>(codes[i])
        );
    }

    parser.ParseProgram();

    if (parser.IsOK()) {
        auto root = parser.GetRoot();

        ASTVisitor astVisitor;
        astVisitor.Visit(root);
    } else {
        std::cout << "not ok" << std::endl;
        for (auto i = parser.getMessages().begin();
        i != parser.getMessages().end(); i++) {
            std::cout << i->msg << std::endl;
        }
    }

    

}
