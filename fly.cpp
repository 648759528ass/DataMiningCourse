//
// Created by jhas on 2022/3/25.
//
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <utility>
#include <time.h>
//#include "ThreadPool/ThreadPool.h"

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
    bool exist(int tag){
        int l = 0;int r = num-1;
        while(l<=r){
            int mid = (l+r)>>1;
            if(ptr[mid] == tag) return true;
            else if(ptr[mid] > tag) r = mid-1;
            else if(ptr[mid] < tag) l = mid+1;
        }
        return false;
    }
};

class Node{
public:
    DataBlock** BlockList;
    int BlockSize;
    Node** mp;
    int mpSize;
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
        memcpy(now,last,sizeof(int)*lastNum);
        now[lastNum] = n;
        num = lastNum+1;
        cnt = 0;
    }
//    ~Node(){
//        delete[] now;
//    }
};
std::vector<Node*> final;
std::mutex lock;
std::vector<DataBlock*> bl[20000];
void load(const char* path,std::map<int,Node*>& mp){
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
                auto nd = new Node(i);
                mp[i] = nd;
            }
            mp[i]->cnt++;
            bl[i].push_back(db);
        }
        total++;
    }
    for(auto&[k,v]:mp){
        v->BlockList = new DataBlock*[bl[k].size()];
        memcpy(v->BlockList,bl[k].data(),sizeof(DataBlock*) * bl[k].size());
        v->BlockSize = bl[k].size();
    }
}
const int thread_num = 15;
double support = 0.000;
//ThreadPool pool(thread_num);
void handle(Node* root){
    std::vector<Node*> ret;
    std::vector<std::thread> th;
    for(int it = 0;it<root->mpSize;it++){
        std::vector<Node*> nx;
        bool ins = false;
        for(auto it2 = it+1;it2<root->mpSize;it2++){
            int cnt = 0;
            int value = root->mp[it2]->now[root->mp[it2]->num-1];
            std::vector<DataBlock*> bList;
            bList.reserve(root->mp[it]->BlockSize);
            for(int i = 0;i<root->mp[it]->BlockSize;i++){
                if(root->mp[it]->BlockList[i]->exist(value)){
                    cnt++;
                    bList.push_back(root->mp[it]->BlockList[i]);
                }
            }
            if(1.0*cnt/total < support) continue;
            if(!ins) ins = true;
            Node* next = new Node(value,root->mp[it]->now,root->mp[it]->num);
            next->BlockList = new DataBlock*[bList.size()];
            memcpy(next->BlockList,bList.data(),sizeof(DataBlock*)*bList.size());
//            for(int i = 0;i<bList.size();i++){
//                next->BlockList[i] = bList[i];
//            }
            next->BlockSize = bList.size();
            next->cnt = cnt;
            next->support = 1.0*cnt/total;
            nx.push_back(next);
            ret.push_back(next);
        }
        if(nx.size() >= 2){
            root->mp[it]->mp = new Node*[nx.size()];
            memcpy(root->mp[it]->mp,nx.data(),sizeof(Node*)*nx.size());
//            for(int i = 0;i<nx.size();i++){
//                root->mp[it]->mp[i] = nx[i];
//            }
            root->mp[it]->mpSize = nx.size();
            th.emplace_back(handle,root->mp[it]);
//            pool.commit(handle,root->mp[it]);
//        handle(root->mp[it]);
        }
    }
    if(!ret.empty()){
        std::lock_guard<std::mutex> gd(lock);
        while(!ret.empty()){
            final.push_back(ret.back());
            ret.pop_back();
        }
    }
    for(auto& t:th){
        t.join();
    }
}
Node* V[20000];
int Vnum = 0;
void fun(int it){
    std::vector<Node*> ret;
    for(auto j = it+1;j<Vnum;j++){
        if(1.0*V[j]->cnt / total < support){
            continue;
        }
        int cnt = 0;
        std::vector<DataBlock*> bList;
        bList.reserve(V[it]->BlockSize);
        for(int i = 0;i<V[it]->BlockSize;i++){
            if(V[it]->BlockList[i]->exist(V[j]->now[V[j]->num-1])){
                cnt++;
                bList.push_back(V[it]->BlockList[i]);
            }
        }
        if(1.0*cnt/total < support) continue;
        Node* next = new Node(V[j]->now[V[j]->num-1],V[it]->now,V[it]->num);
        next->BlockList = new DataBlock*[bList.size()];
        memcpy(next->BlockList,bList.data(),sizeof(DataBlock*)*bList.size());
//                    for(int i = 0;i<bList.size();i++){
//                        next->BlockList[i] = bList[i];
//                    }
        next->BlockSize = bList.size();
        next->cnt = cnt;
        next->support = 1.0*cnt/total;
        ret.push_back(next);
    }
    std::sort(ret.begin(),ret.end(),[](Node* a,Node* b){
        return a->now[a->num-1] < b->now[b->num-1];
    });
    bool toHandle = false;
    if(ret.size() >= 2){
        toHandle = true;
        V[it]->mp = new Node*[ret.size()];
        memcpy(V[it]->mp,ret.data(),sizeof(Node*)*ret.size());
//                    for(int i = 0;i<ret.size();i++){
//                        V[it]->mp[i] = ret[i];
//                    }
        V[it]->mpSize = ret.size();
//                    pool.commit(handle,V[it]);
    }
    std::thread t([&ret](){
        if(!ret.empty()){
            std::lock_guard<std::mutex> gd(lock);
            while(!ret.empty()){
                final.push_back(ret.back());
                ret.pop_back();
            }
        }
    });
    if(toHandle) handle(V[it]);
    t.join();
};

int main(){
    clock_t loadStartTime,loadEndTime;
    loadStartTime = clock();
    std::map<int,Node*> mp;
    load("../retail.dat",mp);
    for(auto it = mp.begin();it!=mp.end();it++){
        V[Vnum++] = it->second;
    }
    std::vector<std::thread> th;
    loadEndTime = clock();
    clock_t handleStartTime,handleEndTime;
    handleStartTime = clock();
    for(int it = 0;it < Vnum;it++){
        if(1.0*V[it]->cnt / total >= support){
//            pool.commit(fun,it);
            th.emplace_back(fun,it);
        }

    }

//    while(pool._idlThrNum != thread_num){
//        std::this_thread::yield();
//    }
    for(auto& t:th){
        t.join();
    }
//    pool._run = false;
    handleEndTime = clock();
    for(auto&v:final){
        for(int i = 0;i<v->num;i++){
            std::cout<<v->now[i]<<" ";
        }
        std::cout<<"sup:"<<v->support<<"\n";
    }
    std::cout<<"loadTime:"<<(double)(loadEndTime - loadStartTime) / CLOCKS_PER_SEC<<"\n"
             <<"handleTime:"<<(double)(handleEndTime - handleStartTime) / CLOCKS_PER_SEC;
}