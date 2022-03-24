#ifndef DATAREADER
#define DATAREADER
#include <utility>
#include <vector>
#include <set>
#include <list>
template<typename V>
class DataReader
{
public:
    virtual std::list<std::set<V>> getData() = 0;
    virtual ~DataReader() = default;
};

#endif