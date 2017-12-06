#pragma once
#include "halang.h"
#include "ast.h"
#include "visitor.h"
#include <unordered_set>

namespace halang {

    class MidProg : public Visitor {
    public:

		virtual void Visit(Node*) override;
#define VISIT_METHOD(NAME) virtual void Visit(NAME##Node*) override;
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD

    protected:

        U16String GetTmpVariableName();

    private:
        std::unordered_set<U16String> var_names_set;
        int tmp_name_counter = 0;

   };

}
