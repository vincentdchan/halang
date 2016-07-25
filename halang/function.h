#pragma once
#include <vector>
#include "object.h"
#include "svm_codes.h"

namespace runtime
{
	namespace StackVM
	{

		class Function
		{
		public:
			inline const std::vector<Object>& getConstTable() const { return _const_table;  }
			inline const std::vector<VM_CODE>& getVmCodes() const { return _codes; }
			inline unsigned int getVariableSize() const { return _var_size; }
			inline void setVariableSize(unsigned int _size) { _var_size = _size; }
		private:
			unsigned int _var_size;
			std::vector<VM_CODE> _codes;
			std::vector<Object> _const_table;
		};

	}
}
