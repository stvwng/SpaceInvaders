#include "Random.h"

using namespace std;

std::mt19937& Random::engine()
{
    // A function-local static is initialised once, on first use, and that
    // initialisation is thread-safe from C++11 onwards. Declaring the engine at
    // namespace scope instead would expose it to the static initialisation
    // order problem -- which is exactly the class of bug that made the
    // scattered WorldState definitions in this project worth consolidating.
    static std::mt19937 e{std::random_device{}()};
    return e;
}

float Random::inRange(float low, float high)
{
    return std::uniform_real_distribution<float>(low, high)(engine());
}

int Random::inRange(int low, int high)
{
    return std::uniform_int_distribution<int>(low, high)(engine());
}
