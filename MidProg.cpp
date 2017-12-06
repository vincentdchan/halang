#include "halang.h"
#include "parser.h"
#include "MidProg.h"
#include "util.h"
#include <string>
#include <sstream>

namespace halang {

    void MidProg::Visit(Node * node) {
        node->Visit(this);
    }

    void MidProg::Visit(ProgramNode * node) {
        for (auto i = node->statements.begin();
            i != node->statements.end(); i++) {
                Visit(*i);
            }
    }

    void MidProg::Visit(NumberNode * node) {
        U16String str = utils::utf8_to_utf16(std::to_string(node->number));
        names_stack.push(str);
    }

    void MidProg::Visit(NullStatementNode * node) {
        // wait to finish
    }

    void MidProg::Visit(IdentifierNode * node) {
        names_stack.push(node->name);
    }

    void MidProg::Visit(StringNode * node) {
        // wait to finish
    }

    void MidProg::Visit(LetStatementNode * node) {
        // wait to finish
        for (auto i = node->assignments.begin();
            i != node->assignments.end(); i++) {
            Visit(*i);
        }
    }

    void MidProg::Visit(ExpressionStatementNode * node) {
        // wait to finish
        Visit(node->expression);
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
        Code code;
        code.code = u"MOV";

        Visit(node->identifier);
        code.op1 = names_stack.top();
        names_stack.pop();

        Visit(node->expression);
        code.op2 = names_stack.top();
        names_stack.pop();

        names_stack.push(code.op1);
        codes.push_back(code);
    }

    void MidProg::Visit(ListExpressionNode * node) {
        // wait to finish
    }

    void MidProg::Visit(UnaryExpressionNode * node) {
        Code code;
        Visit(node->child);

        auto tmp = GetTmpVariableName();
        code.code = u"MUL";
        code.op1 = tmp;
        code.op3 = names_stack.top();
        names_stack.pop();

        switch(node->op) {
            case OperatorType::ADD:
                code.op2 = u"1";
                break;
            case OperatorType::SUB:
                code.op2 = u"-1";
                break;
        }
        
        names_stack.push(tmp);
        codes.push_back(code);
    }

    void MidProg::Visit(BinaryExpressionNode * node) {
        Code code;
        Visit(node->left);
        Visit(node->right);

        code.op3 = names_stack.top();
        names_stack.pop();

        code.op2 = names_stack.top();
        names_stack.pop();

        U16String tmp = GetTmpVariableName();
        code.op1 = tmp;

        switch(node->op) {
            case OperatorType::ADD:
                code.code = u"ADD";
                break;
            case OperatorType::SUB:
                code.code = u"SUB";
                break;
            case OperatorType::MUL:
                code.code = u"MUL";
                break;
            case OperatorType::DIV:
                code.code = u"DIV";
                break;
            case OperatorType::MOD:
                code.code = u"MOD";
                break;
            case OperatorType::POW:
                code.code = u"POW";
        }

        names_stack.push(tmp);
        codes.push_back(code);
    }

    void MidProg::Visit(DoExpressionNode * node) {
        // wait to finish
    }

    void MidProg::Visit(FunExpressionNode * node) {
        // wait to finish
    }

    bool MidProg::IsIdentifier(Node * node) const {
        return node->AsIdentifier() != nullptr;
    }

    const std::vector<Code>&
    MidProg::GetCodes() const {
        return codes;
    }

    bool MidProg::IsNumber(Node * node) const {
        return node->AsNumber() != nullptr;
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

        for (auto i = midProg.GetCodes().begin();
            i != midProg.GetCodes().end(); i++) {
                std::cout << utils::utf16_to_utf8(i->code) 
                << "\t"
                << utils::utf16_to_utf8(i->op1) 
                << "\t" 
                << utils::utf16_to_utf8(i->op2) 
                << "\t"
                << utils::utf16_to_utf8(i->op3) 
                << std::endl;
            }

    } else {
        for (auto i = parser.getMessages().begin();
        i != parser.getMessages().end(); i++) {
            std::cout << i->msg << std::endl;
        }

        return -1;
    }

    return 0;
}