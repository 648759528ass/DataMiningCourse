#include <iostream>
#include "DataReader/FileDataReader.h"
#include "DataHandler/Handler.h"
#include <time.h>
int main(int, char**) {
    clock_t loadStartTime,loadEndTime;
    loadStartTime = clock();
    FileDataReader<int> FDR("/Users/mac/DataMiningCourse/retail.dat");
    auto L =  FDR.getData();
    Handler<int> hd(L);
    loadEndTime = clock();
    clock_t handleStartTime,handleEndTime;
    handleStartTime = clock();
    auto ret = hd.handle(0.50,0.005);
    handleEndTime = clock();
    for(auto& ele:ret){
        std::cout<<ele<< " sup: " << ele.support<<" conf: "<<ele.confidence<<"\n";
    }
    std::cout<<"loadTime:"<<(double)(loadEndTime - loadStartTime) / CLOCKS_PER_SEC<<"\n"
    <<"handleTime:"<<(double)(handleEndTime - handleStartTime) / CLOCKS_PER_SEC;
}
