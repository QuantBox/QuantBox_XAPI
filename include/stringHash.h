#ifndef _STRING_HASH_H_
#define _STRING_HASH_H_

#if defined _WIN32 || WIN32 || _WINDOWS
#else

#include <string>
#include <map>
#include <hash_map>

using namespace std;
using namespace __gnu_cxx;

namespace __gnu_cxx
{
    template<> struct hash<const string>
    {
        size_t operator()(const string& s) const
        { return hash<const char*>()( s.c_str() ); } // __stl_hash_string
    };
    template<> struct hash<string>
    {
        size_t operator()(const string& s) const
        { return hash<const char*>()( s.c_str() ); }
    };
}

#endif // defined

#endif // _STRING_HASH_H_
