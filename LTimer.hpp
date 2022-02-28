#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>
typedef std::function<void(void)> TimerEvent;

class LTimer
{
    std::atomic<bool> active{true};
    std::atomic<int> repeats{-1};

public:
    LTimer() = default;
    ~LTimer()
    {
        stop();
    }
    void setReapet(TimerEvent callback, int interval, int repeats)
    {
        this->repeats = repeats;
        active = true;
        std::thread t([=]()
                      {
        while(active.load() && this->repeats.load() != 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(!active.load()) return;
            callback();
            if(this->repeats.load() != -1){
                this->repeats--;
            }
                
        } });
        t.join();
    }

    void stop()
    {
        active = false;
    }
};
