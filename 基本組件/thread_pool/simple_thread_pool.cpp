#include<thread>
#include<queue>
#include<iostream>
#include<vector>
#include<memory>
#include<functional>
#include<mutex>
#include<condition_variable>
using namespace std;

std::mutex mtx;
std::condition_variable cv;

class Queue{
public:
    void put(int val){
        unique_ptr<std::mutex>lck(mtx);
        while(!que.empty()){
            //que不為空，生產者應該通知消費者去消費，消費完了，再繼續生產
            //生產者線程應該進入阻塞，並且釋放所，讓消費者消費一下
            cv.wait(lck); //這個參數只接受unique_lock 
        }
        que.push(val);
        cv.notify_all();
        cout << "消費者 消費" << val << " 號物品" << endl;
    }

    int get(){
        unique_ptr<std::mutex>lck(mtx);
        while(que.empty()){
            
            cv.wait(lck);
        }
        int val = que.front();
        que.pop();
        cv.notify_all();
        cout<< "消費者消費" << val << " 號物品" << endl;
        return val;
    }
private:
    queue<int> que;
};

void producer(Queue *que){
    for(int  i = 1; i <= 10; i++){
        que->put(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
