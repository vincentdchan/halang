#include "token.h"
#include <memory>

namespace halang {

    Token::Token(): 
    type(ILLEGAL), maybeInt(false)
    {}

    Token::Token(Token::TYPE _t, Location _loc)
    {
        type = _t;
        location = _loc;
    }

#define EXTEND_OP(NAME, STR, PRECEDENCE) case TYPE::NAME: \
    result = #NAME; \
    break;

#define EXTEND_TOKEN(NAME) case TYPE::NAME: \
    result = #NAME; \
    break;

    const std::string&
    Token::ToString() const {
        std::string result;

        switch(type) {
			OPERATOR_LIST(EXTEND_OP)
            TOKEN_LIST(EXTEND_TOKEN)

            case TYPE::TOKEN_NUMBER:
                result = "TOKEN_NUMBER";
                break;
        }

        return result;
    }

#undef EXTEND_OP
#undef EXTEND_TOKEN

}
