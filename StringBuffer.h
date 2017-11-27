#pragma once
#include "halang.h"
#include "token.h"
#include "util.h"
#include <deque>
#include <vector>
#include <string>
#include <memory>

namespace halang {

    class StringBuffer {
    public:

        StringBuffer();
        void AddBuffer(const std::shared_ptr<U16String>& buf);
        void AddBuffer(const std::shared_ptr<std::string>& buf);

		static bool IsDigit(char16_t t);
		static bool IsAlphabet(char16_t t);
		static bool IsWhite(char16_t t);
		static bool IsLineBreak(char16_t t);

        inline Location GetLocation() {
            return loc;
        }

        inline char16_t GetChar() {
            if (back_ptr >= 0) {
                return back_buffer[back_buffer.size() - back_ptr - 1].first;
            }
            if (buffer_list.size() == 0) {
                return 0;
            }
            auto buf = *buffer_list.begin();
            return (*buf)[char_ptr];
        }

        inline void GoBackChar() {
            back_ptr++;
            auto _pair = back_buffer[back_ptr];
            loc = _pair.second;
        }

        inline void Finish() {
            is_finished = true;
        }

        inline bool IsFinished() const {
            return is_finished;
        }

        char16_t NextChar();

    protected:
        Location loc;

    private:
        int char_ptr;
        int back_ptr;
        bool is_finished;

        const char16_t ch_endofline = u'\n';

        std::deque<std::shared_ptr<U16String>> buffer_list;
        std::vector<
            std::pair<char16_t, Location>
        > back_buffer;
        void check_back_buffer();
    };

}
