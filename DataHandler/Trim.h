//
// Created by Mac on 22/03/2022.
//

#ifndef DATAMININGCOURSE_TRIM_H
#define DATAMININGCOURSE_TRIM_H

#include <vector>
#include <ostream>
#include <map>
#include <list>
#include <set>
template<typename T>
class Dic {
public:
    std::vector<T> now;
    std::map<T,Dic<T>*> mp;
    std::vector<typename std::list<std::set<T>>::iterator> nodeList;
    int cnt;
    double confidence;
    double support;
    Dic(std::vector<T> last,T next,int cnt,double c,double s,std::vector<typename std::list<std::set<T>>::iterator> nodeList):cnt(cnt),confidence(c),support(s),nodeList(nodeList){
        now = last;
        now.push_back(next);
    }
    friend std::ostream& operator <<(std::ostream& out,Dic& d){
        if(d.now.size()<=0) return out;
        out<<"{"<<d.now[0];
        for(auto i = 1;i<d.now.size();i++){
            out<<","<<d.now[i];
        }
        out<<"}";
        return out;
    }
};


#endif //DATAMININGCOURSE_TRIM_H
