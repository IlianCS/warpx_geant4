#ifndef READ_HH
#define READ_HH

#include <vector>
#include <string>
#include <array>
#include <cmath>        // pour std::sin, std::cos
#include <algorithm>    // pour std::lower_bound
#include <numeric>      // pour std::partial_sum

namespace wxg4
{

static constexpr double PI = 3.14159265358979323846;

struct ParticleData {
    std::vector<double> px, py, pz;
    std::vector<double> ws;  // somme cumulée des poids
};

ParticleData read_particle_data_3d(
    const std::string& filename,
    const std::string& species_name,
    int iteration);

ParticleData read_particle_data_2d(
    const std::string& filename,
    const std::string& species_name,
    int iteration);

std::array<double, 3> sample_momentum_3d(
    const ParticleData& pdata,
    double rand_0_1);

enum class DIR { X, Z };

template <DIR rot_axis>
std::array<double, 3> sample_momentum_2d(
    const ParticleData& pdata,
    double rand_0_1,
    double rand_0_1_ang)
{
    double total = pdata.ws.back();
    double target = rand_0_1 * total;
    auto it = std::lower_bound(pdata.ws.begin(), pdata.ws.end(), target);
    std::size_t idx = std::distance(pdata.ws.begin(), it);

    double ang = rand_0_1_ang * 2.0 * PI;
    if constexpr (rot_axis == DIR::X) {
        return {
            pdata.px[idx],
            pdata.pz[idx] * std::sin(ang),
            pdata.pz[idx] * std::cos(ang)
        };
    } else {
        return {
            pdata.px[idx] * std::cos(ang),
            pdata.px[idx] * std::sin(ang),
            pdata.pz[idx]
        };
    }
}

} // namespace wxg4

#endif // READ_HH