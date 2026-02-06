#include <random>
#include <limits> // for std::numeric_limits
using namespace std;

class Random64 {
public:
    /**
     * @brief Get a random number across the full 64-bit range.
     * Used for creating keys.
     */
    static unsigned long long next() {
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
    static unsigned long long range(unsigned long long min, unsigned long long max) {
        static std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<unsigned long long> dist(min, max);
        
        return dist(rng);
    }

    /**
     * @brief Get a random double between 0.0 and 1.0 (Crucial for mutation rates).
     */
    static double probability() {
        static std::mt19937_64 rng(std::random_device{}());
        static std::uniform_real_distribution<double> dist(0.0, 1.0);
        
        return dist(rng);
    }

    /**
     * @brief Generate a number from a Normal (Gaussian) distribution.
     */
    static double deviation(double mean, double stddev) {
        static std::mt19937_64 rng(std::random_device{}());
        std::normal_distribution<double> dist(mean, stddev);
        
        return dist(rng);
    }
};