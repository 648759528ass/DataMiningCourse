//
// Created by Mac on 22/03/2022.
//

#ifndef DATAMININGCOURSE_HANDLER_H
#define DATAMININGCOURSE_HANDLER_H
#include <utility>
#include <list>
#include <set>
#include <map>
#include "Trim.h"
template<typename T>
class Handler {
private:
    std::list<std::set<T>> local;
public:
    explicit Handler(std::list<std::set<T>> p){
        local = p;
    }
    std::vector<Dic<T>> handle(double confidence,double support){
        int n = local.size();
        int min_support = support*local.size();
        std::vector<Dic<T>> ret;
        std::map<T,Dic<T>*> mp;
        std::vector<Dic<T>*> V;
        for(auto it = local.begin();it!=local.end();it++){
            for(auto &v:*it){
                if(mp.count(v) == 0){
                    std::vector<typename std::list<std::set<T>>::iterator> nw;
                    mp[v] = new Dic<T>({},v,0,0,0,nw);
                }
                mp[v]->cnt++;
                mp[v]->nodeList.push_back(it);
            }
        }
        for(auto it1 = mp.begin();it1!=mp.end();it1++){
            if(it1->second->cnt < min_support) continue;
            V.push_back(it1->second);
            for(auto it2 = std::next(it1);it2!=mp.end();it2++){
                if(it2->second->cnt < min_support) continue;
                int cnt = 0;
                std::vector<typename std::list<std::set<T>>::iterator> nodeList;
                for(auto &node:it1->second->nodeList){
                    if(check(*node,{it1->first,it2->first})){
                        cnt++;
                        nodeList.push_back(node);
                    }
                }
                if(1.0*cnt/n < support) continue;
                auto Nl = new Dic<T>(
                        {it1->first},
                        it2->first,
                        cnt,
                        1.0*cnt/it1->second->cnt,
                        1.0*cnt/n,
                        nodeList
                );
                it1->second->mp[it2->first] = Nl;
//                if(Nl->confidence >= confidence && Nl->support>=support ){
//                    ret.push_back(*Nl);
//                }
                if(Nl->support>=support ){
                    ret.push_back(*Nl);
                }
            }
        }
        while(V.size()){
            std::vector<Dic<T>*> next;
            for(auto& ptr:V){
                for(auto it1 = ptr->mp.begin();it1!=ptr->mp.end();it1++){
                    if(it1->second->cnt < min_support) continue;
                    next.push_back(it1->second);
                    auto tmpV = it1->second->now;
                    for(auto it2 = std::next(it1); it2 != ptr->mp.end() ;it2++){
                        if(it2->second->cnt < min_support) continue;
                        int cnt = 0;
                        tmpV.push_back(it2->first);
                        std::vector<typename std::list<std::set<T>>::iterator> nodeList;
                        for(auto &node : it1->second->nodeList){
                            if(check(*node,tmpV)){
                                cnt++;
                                nodeList.push_back(node);
                            }
                        }
                        auto Nl = new Dic<T>(
                                it1->second->now,
                                it2->first,
                                cnt,
                                1.0*cnt/it1->second->cnt,
                                1.0*cnt/n,
                                nodeList
                        );
                        it1->second->mp[it2->first] = Nl;
                        if(Nl->confidence >= confidence&& Nl->support>=support ){
                            ret.push_back(*Nl);
                        }
                        tmpV.pop_back();
                    }
                }
            }
            V = next;
        }
        return ret;
    }
    bool check(std::set<T>& st,std::vector<T> V){
        for(auto&k : V){
            if(!st.count(k)) return false;
        }
        return true;
    }
};


#endif //DATAMININGCOURSE_HANDLER_H
