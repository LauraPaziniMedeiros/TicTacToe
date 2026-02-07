#include "Random64.h"

/**
 * @brief Get a random number across the full 64-bit range.
 * Used for creating keys.
 */
unsigned long long Random64::next() {
    // 'static' ensures the engine is initialized only once per program run
    static std::mt19937_64 rng(std::random_device{}()); 
    
    // Distribution for the full range (0 to MAX)
    static std::uniform_int_distribution<unsigned long long> dist(
        0, std::numeric_limits<unsigned long long>::max()
    );

    return dist(rng);
}

/**
 * @brief Get a random number in a specific range [min, max].
 */
unsigned long long Random64::range(unsigned long long min, unsigned long long max) {
    static std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<unsigned long long> dist(min, max);
    
    return dist(rng);
}

/**
 * @brief Get a random double between 0.0 and 1.0 (Crucial for mutation rates).
 */
double Random64::probability() {
    static std::mt19937_64 rng(std::random_device{}());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    return dist(rng);
}

/**
 * @brief Generate a number from a Normal (Gaussian) distribution.
 */
double Random64::deviation(double mean, double stddev) {
    static std::mt19937_64 rng(std::random_device{}());
    std::normal_distribution<double> dist(mean, stddev);
    
    return dist(rng);
}