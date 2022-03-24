#ifndef DATAREADER
#define DATAREADER
#include<utility>
#include<vector>

template<typename K,typename V>
class DataReader
{
private:
    
public:
    virtual vector<pair<K,vector<V>>> getData();
    virtual ~DataReader();
};

#endif