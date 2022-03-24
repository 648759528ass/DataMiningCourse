
#include "DataReader/FileDataReader.h"
bool check(std::set<int>& st,std::vector<int> V){
    for(auto&k : V){
        if(!st.count(k)) return false;
    }
    return true;
}
int main(int, char**) {
    FileDataReader<int> FDR("/Users/mac/DataMiningCourse/retail.dat");
    auto L =  FDR.getData();
    int ans = 0;
    for(auto&st :L){
        if(check(st,{49})) ans++;
    }
    std::cout<<ans;

}
