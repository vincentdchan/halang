#include "StringBuffer.h"

namespace halang {

    StringBuffer::StringBuffer():
    char_ptr(0), back_ptr(-1), is_finished(false) {
    }

    void StringBuffer::AddBuffer(
        const std::shared_ptr<U16String>& buf)
    {
        buffer_list.push_back(buf);
    }

    void StringBuffer::AddBuffer(
        const std::shared_ptr<std::string>& buf)
    {
        AddBuffer(
            std::make_shared<U16String>(
                utils::utf8_to_utf16(*buf)
            )
        );
    }

    char16_t StringBuffer::NextChar() {
        auto current = GetChar();
        if (back_ptr >= 0) {
            back_ptr--;
        } else {
            if (buffer_list.size() == 0) {
                return 0x00;
            }

            auto buf = *buffer_list.begin();
            if (char_ptr + 1 >= buf->size()) { // end of buffer
                buffer_list.pop_front();
                char_ptr = 0;
            } else {
                char_ptr++;
            }

            back_buffer.push_back(
                std::make_pair(current, GetLocation())
            );
            check_back_buffer();
        }

        loc.pos++;
        // check utf 16
        if (current == ch_endofline) {
            loc.line++;
            loc.column = 0;
        } else {
            loc.column++;
        }
        return current;
    }

    void StringBuffer::check_back_buffer() {
        if (back_buffer.size() > 64) {
            back_buffer.resize(32);
        }
    }

	bool StringBuffer::IsDigit(char16_t ch)
	{
		return ch >= u'0' && ch <= u'9' ? true : false;
	}

	bool StringBuffer::IsAlphabet(char16_t ch)
	{
		return (ch >= u'a' && ch <= u'z') || (ch >= u'A' && ch <= u'Z');
	}

	bool StringBuffer::IsWhite(char16_t ch) {
		return (
			ch == u' ' ||
			ch == u'\n' ||
			ch == u'\r' ||
			ch == u'\t'
		);
	}

	bool StringBuffer::IsLineBreak(char16_t ch) {
		return (
			ch == u'\n' ||
			ch == u'\r'
		);
	}

}
