#include <utils/multiplefloat.h>

#if DOUBLE_TYPE == DOUBLE_GMP

namespace std {
    std::string to_string(const MultipleFloat & mf) {
        return mf.getString();
    }
}

#endif //#if DOUBLE_TYPE == DOUBLE_GMP
