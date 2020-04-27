#include "../src/SortedList.h"
#include "../src/Node.h"
#include "omp.h"
#include <vector>

#include <random>

#define RANGE_MIN 0
#define RANGE_MAX 2147483647
#define PREINSERT_SIZE 100000

bool _debug = false;

std::vector<int> genRandInt(int sz, int mn, int mx) {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(mn, mx);
    std::vector<int> v;
    v.reserve(sz);
    for (int n = 0; n < sz; ++n) {
        v.push_back(dist(rd));
    }
    return v;
}

int main(int argc, char* argv[]) {


    if (argc != 4) {
        printf("usage: ./test [nThreads] [nWorkload] [WorkloadType]\n");
        printf("WorkloadType: PURE_READ=0 PURE_INSERT=1 PURE_REMOVE=2 MIXED=3 \n");
        return 0; 
    }
    int nThreads = atoi(argv[1]);
    int nWorkload = atoi(argv[2]);
    int WorkloadType = atoi(argv[3]);

    SortedList<Node, int> sl;
    // pre insert some nodes
    printf("pre insert %d nodes\n", PREINSERT_SIZE);
    auto v = genRandInt(PREINSERT_SIZE,RANGE_MIN,RANGE_MAX);
    for (int i : v) sl.insert(i);
    printf("txsl size: %d\n", sl.size);

    // _debug = true;
    auto v2 = genRandInt(nWorkload,RANGE_MIN,RANGE_MAX);
    // test
    double start = omp_get_wtime();
    int totalAborts = 0;
    int abortCount = 0;

    for (int i=0; i<nWorkload; ++i) {
        abortCount = 0;
        switch (WorkloadType)
        {
        case 0:  
            sl.find(v2[i]);   
            break;
        case 1:
            sl.insert(v2[i]);   
            break;
        case 2:
            sl.remove(v[i]);   
            break;
        case 3:
            switch (i % 4)
            {
            case 0:
                sl.insert(v2[i]); 
                break;
            default:
                sl.find(v2[i]); 
                break;
            }
        case 4:
            switch (i % 6)
            {
            case 0:
                sl.insert(v2[i]); 
                break;
            default:
                sl.find(v2[i]); 
                break;
            }
        }       
        totalAborts += abortCount;
    }

    double elapsedTime = omp_get_wtime() - start;
    printf("elapsed time: %lf seconds\n", elapsedTime);
    printf("nOps: %d\n nAborts: %d\n", nWorkload, totalAborts);
    printf("abort rate: %lf\n", 1.0*totalAborts/nWorkload);
    printf("throughput: %lf ops/seconds\n", 1.0*nWorkload/elapsedTime);
    printf("txsl size: %d\n", sl.size);
}