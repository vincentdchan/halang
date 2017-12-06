#include "parser.h"
#include "MidProg.h"
#include "util.h"
#include <string>

namespace halang {

    void MidProg::Visit(Node * node) {
        // wait to finish
    }

    void MidProg::Visit(ProgramNode * node) {
        // wait to finish
    }

    void MidProg::Visit(NumberNode * node) {
        // wait to finish
    }

    void MidProg::Visit(NullStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(IdentifierNode * node) {
        // wait to finish
    }

    void MidProg::Visit(StringNode * node) {
        // wait to finish
    }

    void MidProg::Visit(LetStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(ExpressionStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(IfStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(WhileStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(BreakStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(ContinueStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(ReturnStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(DefStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(MemberExpressionNode * node) {
        // wait to finish
    }

    void MidProg::Visit(CallExpressionNode * node) {
        // wait to finish
    }

    void MidProg::Visit(AssignExpressionNode * node) {
        // wait to finish
    }

    void MidProg::Visit(ListExpressionNode * node) {
        // wait to finish
    }

    void MidProg::Visit(UnaryExpressionNode * node) {
        // wait to finish
    }

    void MidProg::Visit(BinaryExpressionNode * node) {
        // wait to finish
    }

    void MidProg::Visit(DoExpressionNode * node) {
        // wait to finish
    }

    void MidProg::Visit(FunExpressionNode * node) {
        // wait to finish
    }

    U16String MidProg::GetTmpVariableName() {
        U16String name;
        U16String tmp = utils::utf8_to_utf16("tmp_");

        do {
            name = tmp + utils::utf8_to_utf16(std::to_string(tmp_name_counter++));
        } while (var_names_set.find(name) != var_names_set.end());

        var_names_set.insert(name);
        return name;
    }

}

int main(int argc, char * argv[]) {
    using namespace halang;

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

        MidProg midProg;
        midProg.Visit(root);
    } else {
        for (auto i = parser.getMessages().begin();
        i != parser.getMessages().end(); i++) {
            std::cout << i->msg << std::endl;
        }

        return -1;
    }

    return 0;
}