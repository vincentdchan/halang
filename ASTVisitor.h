#pragma once
#include <iostream>
#include <sstream>
#include "halang.h"
#include "ast.h"
#include "visitor.h"

namespace halang {

    class ASTVisitor : public Visitor {
    public:

		virtual void Visit(Node*) override;
#define VISIT_METHOD(NAME) virtual void Visit(NAME##Node*) override;
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD

        ostream& Out() {
            for (int i = 0; i < depth; i++) {
                std::cout << "  ";
            }
            return std::cout;
        }

        inline std::string PrintOP(OperatorType op) {
            std::stringstream ss;
            switch(op) {
                case OperatorType::ADD:
                    ss << "+";
                    break;
                case OperatorType::SUB:
                    ss << "-";
                    break;
                case OperatorType::MUL:
                    ss << "*";
                    break;
                case OperatorType::DIV:
                    ss << "/";
                    break;
                case OperatorType::EQ:
                    ss << "==";
                    break;
                case OperatorType::GT:
                    ss << ">";
                    break;
                case OperatorType::GTEQ:
                    ss << ">=";
                    break;
                case OperatorType::LT:
                    ss << "<";
                    break;
                case OperatorType::LTEQ:
                    ss << "<=";
                    break;
                case OperatorType::LG_AND:
                    ss << "&";
                    break;
                case OperatorType::LG_OR:
                    ss << "|";
                    break;
                case OperatorType::MOD:
                    ss << "%";
                    break;
                case OperatorType::POW:
                    ss << "**";
                    break;
                default:
                    ss << "unknown";
            }
            return ss.str();
        }

    };

}
