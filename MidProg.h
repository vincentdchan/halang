#pragma once
#include "halang.h"
#include "ast.h"
#include "visitor.h"
#include <unordered_set>
#include <stack>
#include <vector>

namespace halang {

    class Code {
    public:

        Code() { }
        Code(U16String o1, U16String o2, U16String o3) :
        op1(o1), op2(o2), op3(o3) { }

        U16String code;
        U16String op1;
        U16String op2;
        U16String op3;

    };

    class MidProg : public Visitor {
    public:

		virtual void Visit(Node*) override;
#define VISIT_METHOD(NAME) virtual void Visit(NAME##Node*) override;
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD

        const std::vector<Code>&
        GetCodes() const;


    protected:

        U16String GetTmpVariableName();

        bool IsNumber(Node * node) const;
        bool IsIdentifier(Node * node) const;

    private:
        std::unordered_set<U16String> var_names_set;
        int tmp_name_counter = 0;

        std::stack<U16String> names_stack;
        std::vector<Code> codes;

   };

}
