#ifndef Return_H
#define Return_H
#include <stdexcept>
#include "Object.h"

class Return final {
public: 
    Object value;
    
    Return(Object value);
};

#endif