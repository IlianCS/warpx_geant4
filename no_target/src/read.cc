#include "read.hh"

#include <openPMD/openPMD.hpp>
#include <algorithm>    // std::lower_bound
#include <numeric>      // std::partial_sum
#include <iostream>     // std::cout

namespace wxg4
{

ParticleData read_particle_data_3d(
    const std::string& filename,
    const std::string& species_name,
    int iteration)
{
    std::cout << "[read3D] Ouverture de la série OpenPMD : "
              << filename << std::endl;

    openPMD::Series series(
        filename,
        openPMD::Access::READ_ONLY
    );
    auto it = series.iterations[iteration];
    std::cout << "[read3D] Iteration " << iteration << " chargée." << std::endl;

    // Accès aux datasets
    auto px = it.particles[species_name]["momentum"]["x"];
    auto py = it.particles[species_name]["momentum"]["y"];
    auto pz = it.particles[species_name]["momentum"]["z"];
    auto w  = it.particles[species_name]["weighting"];

    std::cout << "[read3D] Chargement des chunks..." << std::endl;
    auto px_data = px.loadChunk<double>();
    auto py_data = py.loadChunk<double>();
    auto pz_data = pz.loadChunk<double>();
    auto w_data  = w.loadChunk<double>();

    series.flush();
    std::cout << "[read3D] Flush terminé." << std::endl;

    // Nombre de particules
    const std::size_t NP = px.getExtent()[0];
    std::cout << "[read3D] Nombre de particules = " << NP << std::endl;

    // Pointeurs sur les données brutes
    const double* v_px = px_data.get();
    const double* v_py = py_data.get();
    const double* v_pz = pz_data.get();
    const double* v_w  = w_data.get();

    // Remplissage de la structure de retour
    ParticleData pdata;
    pdata.px.assign(v_px, v_px + NP);
    pdata.py.assign(v_py, v_py + NP);
    pdata.pz.assign(v_pz, v_pz + NP);

    pdata.ws.resize(NP);
    std::partial_sum(v_w, v_w + NP, pdata.ws.begin());

    std::cout << "[read3D] Poids cumulés : premier = " << pdata.ws.front()
              << ", dernier = " << pdata.ws.back() << std::endl;

    return pdata;
}

ParticleData read_particle_data_2d(
    const std::string& filename,
    const std::string& species_name,
    int iteration)
{
    std::cout << "[read2D] Ouverture de la série OpenPMD : "
              << filename << std::endl;

    openPMD::Series series(
        filename,
        openPMD::Access::READ_ONLY
    );
    auto it = series.iterations[iteration];
    std::cout << "[read2D] Iteration " << iteration << " chargée." << std::endl;

    auto px = it.particles[species_name]["momentum"]["x"];
    auto pz = it.particles[species_name]["momentum"]["z"];
    auto w  = it.particles[species_name]["weighting"];

    std::cout << "[read2D] Chargement des chunks..." << std::endl;
    auto px_data = px.loadChunk<double>();
    auto pz_data = pz.loadChunk<double>();
    auto w_data  = w.loadChunk<double>();

    series.flush();
    std::cout << "[read2D] Flush terminé." << std::endl;

    const std::size_t NP = px.getExtent()[0];
    std::cout << "[read2D] Nombre de particules = " << NP << std::endl;

    const double* v_px = px_data.get();
    const double* v_pz = pz_data.get();
    const double* v_w  = w_data.get();

    ParticleData pdata;
    pdata.px.assign(v_px, v_px + NP);
    pdata.py.clear();  // pas de composante y en 2D
    pdata.pz.assign(v_pz, v_pz + NP);

    pdata.ws.resize(NP);
    std::partial_sum(v_w, v_w + NP, pdata.ws.begin());

    std::cout << "[read2D] Poids cumulés : premier = " << pdata.ws.front()
              << ", dernier = " << pdata.ws.back() << std::endl;

    return pdata;
}

std::array<double, 3> sample_momentum_3d(
    const ParticleData& pdata,
    double rand_0_1)
{
    double total = pdata.ws.back();
    double target = rand_0_1 * total;

    std::cout << "[sample3D] Tirage uniforme rand=" << rand_0_1
              << " -> cible = " << target
              << " / total = " << total << std::endl;

    auto it = std::lower_bound(pdata.ws.begin(), pdata.ws.end(), target);
    std::size_t idx = std::distance(pdata.ws.begin(), it);

    std::cout << "[sample3D] Particule choisie idx = " << idx
              << " (px=" << pdata.px[idx]
              << ", py=" << pdata.py[idx]
              << ", pz=" << pdata.pz[idx] << ")" << std::endl;

    return { pdata.px[idx], pdata.py[idx], pdata.pz[idx] };
}

} // namespace wxg4