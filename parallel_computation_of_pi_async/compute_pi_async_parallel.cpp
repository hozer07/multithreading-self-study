#include <iostream>
#include <math.h>
#include <chrono>
#include <iomanip>
#include <future>

using namespace std;
using namespace chrono;

#define NUM_OF_THREADS   8
#define N                1000000 // Numerical integration interval
using msec_in_double = chrono::duration<double, std::ratio<1, 1000>>;

void compute_pi_serial();
void compute_pi_parallel();
double numerical_integration_pi(unsigned start_idx, unsigned end_idx);

int main(){
    compute_pi_serial();
    compute_pi_parallel();
    return 0;
}

void compute_pi_serial(){
    auto start_time = steady_clock::now();
    auto pi = numerical_integration_pi(0, N);
    auto duration_in_msec = duration_cast<msec_in_double>(steady_clock::now() - start_time).count();
    cout << "Pi = " << setprecision(8) << pi << ". Computed serially in " << duration_in_msec << " milliseconds.\n";
}

void compute_pi_parallel(){
    auto start_time = steady_clock::now();
    double pi{};
    future<double> thread_returns[NUM_OF_THREADS];

    for(int thread_idx{}; thread_idx < NUM_OF_THREADS; thread_idx++){
        auto interval_start = thread_idx * N / NUM_OF_THREADS;
        auto interval_end = (thread_idx + 1) * N / NUM_OF_THREADS;
        thread_returns[thread_idx] = async(numerical_integration_pi, interval_start, interval_end);
    }

    for(int thread_idx{}; thread_idx < NUM_OF_THREADS; thread_idx++){
        pi += thread_returns[thread_idx].get();
    }

    auto duration_in_msec = duration_cast<msec_in_double>(steady_clock::now() - start_time).count();
    cout << "Pi = " << setprecision(8) << pi << ". Computed in parallel in " << duration_in_msec << " milliseconds.\n";
}

double numerical_integration_pi(unsigned start_idx, unsigned end_idx){
    double pi_temp{};
    for(size_t i{start_idx}; i < end_idx; i++){
        pi_temp += sqrt(1 - pow(i * 1. / N, 2)) / N;
    }
    return 4 * pi_temp;
}
