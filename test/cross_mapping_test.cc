#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "../src/dataset.h"
#include "../src/lut.h"
#include "../src/nearest_neighbors_cpu.h"
#include "../src/simplex_cpu.h"
#ifdef ENABLE_GPU_KERNEL
#include "../src/nearest_neighbors_gpu.h"
#include "../src/simplex_gpu.h"
#endif

// Validation data was generated with pyEDM 1.0.1 with the following
// parameters:
// pyEDM.Simplex(dataFrame=pyEDM.sampleData["sardine_anchovy_sst"],
//               E=3, Tp=1, columns="anchovy", target="np_sst", lib="1 76",
//               pred="1 76", verbose=True)

template <class T, class U> void cross_mapping_test_common(uint32_t E)
{
    const auto tau = 1;
    const auto Tp = 1;

    Dataset ds1, ds2;
    ds1.load("sardine_anchovy_sst.csv");
    ds2.load("anchovy_sst_validation_E" + std::to_string(E) + ".csv");

    auto knn = std::unique_ptr<NearestNeighbors>(new T(tau, Tp, true));
    auto simplex = std::unique_ptr<Simplex>(new U(tau, Tp, true));

    LUT lut;
    Timeseries library = Timeseries(ds1.timeseries[1].data(),
                                    ds1.timeseries[1].size() - (E - 1));
    Timeseries target = Timeseries(ds1.timeseries[4].data(),
                                   ds1.timeseries[4].size() - (E - 1));

    Timeseries prediction;
    Timeseries shifted_target;
    Timeseries valid_prediction = ds2.timeseries[0];

    knn->compute_lut(lut, library, library, E);
    lut.normalize();

    std::vector<float> buffer;

    simplex->predict(prediction, buffer, lut, target, E);
    simplex->shift_target(shifted_target, target, E);

    REQUIRE(prediction.size() == valid_prediction.size());

    for (auto i = 0u; i < prediction.size(); i++) {
        REQUIRE(prediction[i] == Approx(valid_prediction[i]).margin(1e-5f));
    }
}

TEST_CASE("Compute cross mapping (CPU, E=2)", "[ccm][cpu]")
{
    cross_mapping_test_common<NearestNeighborsCPU, SimplexCPU>(2);
}

TEST_CASE("Compute cross mapping (CPU, E=3)", "[ccm][cpu]")
{
    cross_mapping_test_common<NearestNeighborsCPU, SimplexCPU>(3);
}

TEST_CASE("Compute cross mapping (CPU, E=4)", "[ccm][cpu]")
{
    cross_mapping_test_common<NearestNeighborsCPU, SimplexCPU>(4);
}

TEST_CASE("Compute cross mapping (CPU, E=5)", "[ccm][cpu]")
{
    cross_mapping_test_common<NearestNeighborsCPU, SimplexCPU>(5);
}

TEST_CASE("Compute cross mapping (GPU, E=2)", "[ccm][gpu]")
{
    cross_mapping_test_common<NearestNeighborsCPU, SimplexGPU>(2);
}

TEST_CASE("Compute cross mapping (GPU, E=3)", "[ccm][gpu]")
{
    cross_mapping_test_common<NearestNeighborsCPU, SimplexGPU>(3);
}

TEST_CASE("Compute cross mapping (GPU, E=4)", "[ccm][gpu]")
{
    cross_mapping_test_common<NearestNeighborsCPU, SimplexGPU>(4);
}

TEST_CASE("Compute cross mapping (GPU, E=5)", "[ccm][gpu]")
{
    cross_mapping_test_common<NearestNeighborsCPU, SimplexGPU>(5);
}
