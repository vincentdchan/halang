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

TEST_CASE( "StringBuffer", "[StringBuffer]" ) {

    StringBuffer sb;

    sb.AddBuffer(
        std::make_shared<std::string>("abc")
    );
    sb.AddBuffer(
        std::make_shared<std::string>("def")
    );

    U16String merged(u"abcdef");

    for (int i = 0; i < merged.size(); i++) {
        REQUIRE( sb.NextChar() == merged[i] );
    }

    REQUIRE( sb.GetChar() == 0x00 );
}

TEST_CASE( "StringBuffer GobackChar()", "[StringBuffer]" ) {

    StringBuffer sb;

    sb.AddBuffer(
        std::make_shared<std::string>("abc")
    );
    sb.AddBuffer(
        std::make_shared<std::string>("def")
    );

    for (int i = 0; i < 4; i++) {
        sb.NextChar();
    }

    REQUIRE( sb.GetChar() == u'e' );

    sb.GoBackChar();
    REQUIRE( sb.GetChar() == u'd' );

    sb.GoBackChar();
    REQUIRE( sb.GetChar() == u'c' );

    U16String merged(u"cdef");

    for (int i = 0; i < merged.size(); i++) {
        REQUIRE( sb.NextChar() == merged[i] );
    }

}

TEST_CASE( "Lexer", "[Lexer]" ) {

    Lexer lexer;

    const char* codes[3] = {
        "func(a, b){\n",
        "   return a + b * 1;\n",
        "}"
    };

    for (int i = 0; i < 3; i++) {
        lexer.AddBuffer(
            std::make_shared<std::string>(codes[i])
        );
    }

    std::vector<Token::TYPE> token_list;
    token_list.push_back(Token::TYPE::FUNC);
    token_list.push_back(Token::TYPE::OPEN_PAREN);
    token_list.push_back(Token::TYPE::IDENTIFIER);
    token_list.push_back(Token::TYPE::COMMA);
    token_list.push_back(Token::TYPE::IDENTIFIER);
    token_list.push_back(Token::TYPE::CLOSE_PAREN);
    token_list.push_back(Token::TYPE::OPEN_BRACKET);
    token_list.push_back(Token::TYPE::RETURN);
    token_list.push_back(Token::TYPE::IDENTIFIER);
    token_list.push_back(Token::TYPE::ADD);
    token_list.push_back(Token::TYPE::IDENTIFIER);
    token_list.push_back(Token::TYPE::MUL);
    token_list.push_back(Token::TYPE::NUMBER);
    token_list.push_back(Token::TYPE::SEMICOLON);
    token_list.push_back(Token::TYPE::CLOSE_BRACKET);

    for (int i = 0; i < Token::TYPE::TOKEN_NUMBER; i++) {
        std::cout << i << ": " << TokenName[i] << std::endl;
    }

    lexer.NextToken();
    for (auto i = token_list.begin();
    i != token_list.end(); i++) {
        REQUIRE( lexer.NextToken()->type == *i );
    }

    REQUIRE( lexer.NextToken()->type == Token::TYPE::ENDFILE );

}
