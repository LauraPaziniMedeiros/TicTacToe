#ifndef RAND_H
    #define RAND_H
    #include <random>
    #include <limits> // for std::numeric_limits
    using namespace std;
    
    class Random64 {
        public: 
        static unsigned long long next();
        static unsigned long long range(unsigned long long min, unsigned long long max);
        static double probability();
        static double deviation(double mean, double stddev);
    };
#endif