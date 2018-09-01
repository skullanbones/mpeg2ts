#pragma once

#include <memory>
#include <vector>

typedef std::vector<uint8_t> ByteVector;
typedef std::shared_ptr<ByteVector> ByteVectorPtr;


/// Window types
#ifdef WIN32
#define ssize_t size_t
typedef char TCHAR;
#endif
