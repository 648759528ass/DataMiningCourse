//
// Created by jhas on 2022/3/25.
//
#include <algorithm>
#include <sstream>
#include <fstream>
#include <thread>
#include <vector>
#include <map>
#include <utility>
#include "ThreadPool/ThreadPool.h"
int total = 0;
class DataBlock{
public:
    int* ptr;
    int num;
    DataBlock(std::vector<int>& V){
        ptr = (int *)malloc(V.size()* sizeof(int));
        num = V.size();
        for(int i = 0;i<num;i++){
            ptr[i] = V[i];
        }
    }
//    int check(int* vec,int num){
//        for(int i = 0;i<num;i++){
//            if(!exist(vec[num])) return false;
//        }
//        return true;
//    }
    bool exist(int num){
        int l = 0;int r = num-1;
        while(l<r){
            int mid = (l+r)>>1;
            if(ptr[mid] == num) return true;
            else if(ptr[mid] > num) r = mid-1;
            else if(ptr[mid] < num) l = mid+1;
        }
        return false;
    }
};

class Node{
public:
    std::vector<DataBlock*> BlockList;
    std::map<int,Node*> mp;
    int cnt;
    double support;
    int* now;
    int num;
    Node(int n){
        now = new int[1];
        now[0] = n;
        num = 1;
        cnt = 0;
    }
    Node(int n,int last[],int lastNum){
        now = new int[lastNum+1];
        memcpy(now,last,lastNum);
        now[lastNum] = n;
        num = lastNum+1;
    }
    ~Node(){
        delete[] now;
    }
};
std::vector<Node*> final;
std::mutex lock;
std::map<int,Node*> mp;
inline void load(const char* path){
    std::ifstream file(path);
    std::string line;
    std::vector<int> next;
    while (getline(file,line)){
        next.clear();
        std::istringstream iss(line);
        int a;
        while(iss>>a){
            next.push_back(a);
        }
        std::sort(next.begin(),next.end());
        DataBlock* db = new DataBlock(next);
        for(auto i:next){
            if(mp[i] == nullptr){
                mp[i] = new Node(i);
            }
            mp[i]->cnt++;
            mp[i]->BlockList.push_back(db);
        }
        total++;
    }
}
ThreadPool pool(16);
void handle(Node* root){

}
int main(){
    double support = 0.01;
    for(auto it = mp.begin();it != mp.end();it++){
        if(1.0*it->second->cnt / total >= support){
            pool.commit([&it,&support](){
                bool ins = false;
                std::vector<Node*> ret;
                for(auto j = std::next(it);j!=mp.end();j++){
                    if(1.0*j->second->cnt / total < support){
                        continue;
                    }
                    int cnt = 0;
                    std::vector<DataBlock*> bList;
                    bList.reserve(j->second->BlockList.size());
                    for(auto& v:j->second->BlockList){
                        if(v->exist(j->first)){
                            cnt++;
                            bList.push_back(v);
                        }
                    }
                    if(1.0*cnt/total < support) continue;
                    if(!ins) ins = true;
                    Node* next = new Node(j->first,it->second->now,it->second->num);
                    next->cnt = cnt;
                    next->support = 1.0*cnt/total;
                    it->second->mp[it->first] = next;
                    ret.push_back(next);
                }
                if(ins) pool.commit(handle,it->second);
                if(ret.empty()){
                    std::lock_guard<std::mutex> gd(lock);
                    while(ret.empty()){
                        final.push_back(ret.back());
                        ret.pop_back();
                    }
                }
            });
        }
    }
}