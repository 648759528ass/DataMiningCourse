//
// Created by Mac on 22/03/2022.
//

#ifndef DATAMININGCOURSE_FILEDATAREADER_H
#define DATAMININGCOURSE_FILEDATAREADER_H
#include "DataReader.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
template<typename K>
class FileDataReader: public DataReader<K>  {
public:
    FileDataReader(const char * path){

    }
};
template<>
class FileDataReader<int>{
private:
    const char *path;
public:
    FileDataReader(const char * path):path(path){
    }
    std::list<std::set<int>> getData(){
        std::list<std::set<int>> st;
        std::ifstream file(path);
        std::string line;
        std::set<int > next;
        while (getline(file,line)){
            next.clear();
            std::istringstream iss(line);
            int a;
            while(iss>>a){
                next.insert(a);
            }
            st.push_back(next);
        }
        return st;
    }
};

#endif //DATAMININGCOURSE_FILEDATAREADER_H
