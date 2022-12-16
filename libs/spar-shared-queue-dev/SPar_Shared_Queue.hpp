#include <memory>
#include <mutex>
#include <condition_variable> 

template <typename spar_T> 
class SParSharedQueue {
private:
    std::unique_ptr<spar_T*[]> spar_data;    
    size_t spar_queue_size;
    size_t spar_head;
    size_t spar_tail;
    size_t spar_EOS_count;
    bool spar_empty;
    bool spar_full;
    bool spar_EOS;
    size_t spar_num_producers;
    std::mutex spar_mtx;
    std::condition_variable spar_not_empty;
    std::condition_variable spar_not_full;

public:
    SParSharedQueue(size_t _spar_q_size, size_t _spar_num_producers) { 
        spar_head = spar_tail = 0;
        spar_queue_size = _spar_q_size;
        spar_data = std::make_unique<spar_T*[]>(spar_queue_size);
        spar_empty = true;
        spar_full = false;
        spar_EOS_count = 0;
        spar_EOS = false;
        spar_num_producers = _spar_num_producers;
    }
    void NotifyEOS() { 
        std::unique_lock<std::mutex> spar_lock(spar_mtx);
        ++spar_EOS_count;
        spar_lock.unlock();
        if(spar_EOS_count==spar_num_producers){
            spar_T * spar_temp = new spar_T;
            spar_temp->omp_spar_eos = true;
            this->Add(spar_temp);
            spar_EOS = true;
            spar_not_empty.notify_all();
        }
    }
    bool IsEmpty(void) { return spar_empty;}
    bool IsFull(void) { return spar_full;}
    void Add(spar_T * _s_data){

        std::unique_lock<std::mutex> spar_lock(spar_mtx);
        while(spar_full)
            spar_not_full.wait(spar_lock);

        spar_data[spar_tail] = std::move(_s_data);

        spar_tail++;
        if(spar_tail == spar_queue_size)
            spar_tail = 0;
        if(spar_tail == spar_head)
            spar_full = true;
        spar_empty = false;

        spar_lock.unlock();

        spar_not_empty.notify_one();

    }
    spar_T * Remove(void){

        std::unique_lock<std::mutex> spar_lock(spar_mtx);

        while(spar_empty && !spar_EOS)
            spar_not_empty.wait(spar_lock);

        spar_T * _s_data = std::move(spar_data[spar_head]);

        if(_s_data->omp_spar_eos) return _s_data;

        spar_head++;
        if(spar_head == spar_queue_size)
            spar_head = 0;
        if(spar_head == spar_tail)
            spar_empty = true;
        spar_full = false;
        spar_lock.unlock();
        spar_not_full.notify_one();

        return _s_data;
    }

};
