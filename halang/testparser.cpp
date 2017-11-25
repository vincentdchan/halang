#define CATCH_CONFIG_MAIN
#include <iostream>
#include <utility>
#include <memory>
#include <string>
#include "catch.hpp"
#include "StringBuffer.h"
#include "lex.h"
#include "util.h"

using namespace halang;

TEST_CASE( "Lexer", "[Lexer]" ) {

    Lexer lexer;

    const char* codes[3] = {
        "fun a, b -> do\n",
        "   foo();\n"
        "   a + b * 1\n",
        "end"
    };

    for (int i = 0; i < 3; i++) {
        lexer.AddBuffer(
            std::make_shared<std::string>(codes[i])
        );
    }

    std::vector<Token::TYPE> token_list;
    token_list.push_back(Token::TYPE::FUN);
    token_list.push_back(Token::TYPE::IDENTIFIER);
    token_list.push_back(Token::TYPE::COMMA);
    token_list.push_back(Token::TYPE::IDENTIFIER);
    token_list.push_back(Token::TYPE::ARROW);
    token_list.push_back(Token::TYPE::DO);
    token_list.push_back(Token::TYPE::IDENTIFIER);
    token_list.push_back(Token::TYPE::OPEN_PAREN);
    token_list.push_back(Token::TYPE::CLOSE_PAREN);
    token_list.push_back(Token::TYPE::SEMICOLON);
    token_list.push_back(Token::TYPE::IDENTIFIER);
    token_list.push_back(Token::TYPE::ADD);
    token_list.push_back(Token::TYPE::IDENTIFIER);
    token_list.push_back(Token::TYPE::MUL);
    token_list.push_back(Token::TYPE::NUMBER);
    token_list.push_back(Token::TYPE::END);

    lexer.NextToken();
    for (auto i = token_list.begin();
    i != token_list.end(); i++) {
        REQUIRE( lexer.NextToken()->type == *i );
    }

    REQUIRE( lexer.NextToken()->type == Token::TYPE::ENDFILE );

}
