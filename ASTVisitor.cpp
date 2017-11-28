#include "ASTVisitor.h"
#include "util.h"

namespace halang {

	void ASTVisitor::Visit(Node* _node) {
        if (_node == nullptr) {
            Out() << "null" << std::endl;
        } else {
            _node->Visit(this);
        }
    }
    
    void ASTVisitor::Visit(ProgramNode *node) {
        Out() << "Program" << std::endl;

        depth++;
        for (auto i = node->statements.begin();
            i != node->statements.end(); i++) {
                Visit(*i);
            }

        depth--;
    }

    void ASTVisitor::Visit(NumberNode *node) {
        Out() << "Number: " << node->number << std::endl;
    }

    void ASTVisitor::Visit(NullStatementNode *node) {
        Out() << "NullStatement" << std::endl;
    }

    void ASTVisitor::Visit(IdentifierNode *node) {
        Out() << "Identifier: " <<
            utils::utf16_to_utf8(node->name) << std::endl;
    }

    void ASTVisitor::Visit(StringNode *node) {
        Out() << "String: " <<
            utils::utf16_to_utf8(node->content) << std::endl;
    }

    void ASTVisitor::Visit(LetStatementNode *node) {
        Out() << "LetStatement:" << std::endl;
        depth++;

        for (auto i = node->assignments.begin();
        i != node->assignments.end(); i++) {
            Visit(*i);
        }

        depth--;
    }

    void ASTVisitor::Visit(ExpressionStatementNode *node) {
        Out() << "ExpressionStatement:" << std::endl;
        depth++;

        Out() << "expression:" << std::endl;
        depth++;
        Visit(node->expression);
        depth--;

        depth--;
    }

    void ASTVisitor::Visit(IfStatementNode *node) {
        Out() << "IfStatement:" << std::endl;
        depth++;
        Out() << "condition:" << std::endl;
        depth++;
        Visit(node->condition);
        depth--;
        depth--;

        depth++;
        Out() << "children:" << std::endl;

        depth++;
        for (auto i = node->children.begin();
        i != node->children.end(); i++) {
            Visit(*i);
        }
        depth--;

        Out() << "else children:" << std::endl;

        depth++;
        for (auto i = node->else_children.begin();
        i != node->else_children.end(); i++) {
            Visit(*i);
        }
        depth--;

        depth--;
    }

    void ASTVisitor::Visit(WhileStatementNode *node) {
        Out() << "WhileStatement:" << std::endl;
        depth++;

        Out() << "condition:" << std::endl;
        depth++;
        Visit(node->condition);
        depth--;

        Out() << "children:" << std::endl;
        depth++;
        for (auto i = node->children.begin();
        i != node->children.end(); i++) {
            Visit(*i);
        }
        depth--;

        depth--;
    }

    void ASTVisitor::Visit(BreakStatementNode *node) {
        Out() << "BreakStatement" << std::endl;
    }

    void ASTVisitor::Visit(ContinueStatementNode *node) {
        Out() << "ContinueStatement" << std::endl;
    }

    void ASTVisitor::Visit(ReturnStatementNode *node) {
        Out() << "ReturnStatement:" << std::endl;

        if (node->expression) {
            depth++;
            Out() << "expression:" << std::endl;
            depth++;
            Visit(node->expression);
            depth--;

            depth--;
        }
    }

    void ASTVisitor::Visit(DefStatementNode *node) {
        Out() << "DefStatement:" << std::endl;
        depth++;

        Out() << "name:" << std::endl;
        depth++;
        Visit(node->name);
        depth--;

        Out() << "params:" << std::endl;
        depth++;
        for (auto i = node->params.begin();
        i != node->params.end(); i++) {
            Visit(*i);
        }
        depth--;

        Out() << "body:" << std::endl;
        depth++;
        for (auto i = node->body.begin();
        i != node->body.end(); i++) {
            Visit(*i);
        }
        depth--;

        depth--;
    }

    void ASTVisitor::Visit(MemberExpressionNode *node) {
        Out() << "MemberExpression:" << std::endl;
        depth++;

        Out() << "left:" << std::endl;
        depth++;
        Visit(node->left);
        depth--;

        Out() << "right:" << std::endl;
        depth++;
        Visit(node->right);
        depth--;

        depth--;
    }

    void ASTVisitor::Visit(CallExpressionNode *node) {
        Out() << "CallExpression:" << std::endl;
        depth++;

        Out() << "callee:" << std::endl;
        depth++;
        Visit(node->callee);
        depth--;

        Out() << "params:" << std::endl;
        depth++;
        for (auto i = node->params.begin();
        i != node->params.end(); i++) {
            Visit(*i);
        }
        depth--;

        depth--;
    }

    void ASTVisitor::Visit(AssignExpressionNode *node) {
        Out() << "AssignExpression:" << std::endl;
        depth++;

        Out() << "identifier:" << std::endl;
        depth++;
        Visit(node->identifier);
        depth--;

        if (node->expression) {
            Out() << "expression:" << std::endl;
            depth++;
            Visit(node->expression);
            depth--;
        }

        depth--;
    }

    void ASTVisitor::Visit(ListExpressionNode *node) {
        Out() << "ListExpression:" << std::endl;
        depth++;

        for (auto i = node->children.begin();
        i != node->children.end(); i++) {
            Visit(*i);
        }

        depth--;
    }

    void ASTVisitor::Visit(UnaryExpressionNode *node) {
        Out() << "UnaryExpression:" << std::endl;
        depth++;

        Out() << "operator: " 
            << PrintOP(node->op) << std::endl;

        depth++;
        Visit(node->child);
        depth--;

        depth--;
    }

    void ASTVisitor::Visit(BinaryExpressionNode *node) {
        Out() << "BinaryExpression:" << std::endl;
        depth++;

        Out() << "operator: " << PrintOP(node->op) 
            << std::endl;

        Out() << "left:" << std::endl;
        depth++;
        Visit(node->left);
        depth--;

        Out() << "right:" << std::endl;
        depth++;
        Visit(node->right);
        depth--;

        depth--;
    }

    void ASTVisitor::Visit(DoExpressionNode *node) {
        Out() << "DoExpression:" << std::endl;
        depth++;

        for (auto i = node->children.begin();
        i != node->children.end(); i++) {
            Visit(*i);
        }

        depth--;
    }

    void ASTVisitor::Visit(FunExpressionNode *node) {
        Out() << "FunExpression:" << std::endl;
        depth++;

        Out() << "params:" << std::endl;
        depth++;
        for (auto i = node->params.begin();
        i != node->params.end(); i++) {
            Visit(*i);
        }
        depth--;

        Out() << "expression:" << std::endl;
        depth++;
        Visit(node->expression);
        depth--;

        depth--;
    }

}
