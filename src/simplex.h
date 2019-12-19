#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

#include "dataset.h"
#include "lut.h"

class Simplex
{
public:
    Simplex(uint32_t tau, uint32_t Tp, bool verbose)
        : tau(tau), Tp(Tp), verbose(verbose)
    {
    }
    virtual ~Simplex(){};

    // Predict timeseries using Simplex projection. `prediction` is the
    // predicted rsult. The actual values are stored into `buffer`. `lut`
    // needs to be pre-computed using NearestNeighbors and normalized.
    virtual void predict(Timeseries &prediction, std::vector<float> &buffer,
                         const LUT &lut, const Timeseries &target,
                         uint32_t E) = 0;

    // Shift and trim the target timeseries so that its time index matches the
    // predicted timeseries.
    virtual void shift_target(Timeseries &shifted_target,
                              const Timeseries &target, uint32_t E)
    {
        const auto shift = (E - 1) * tau + Tp;
        const auto n_prediction = target.size() - shift;

        shifted_target = Timeseries(target.data() + shift, n_prediction);
    }

protected:
    // Lag
    const uint32_t tau;
    // How many steps to predict in future
    const uint32_t Tp;
    // Enable verbose logging
    const bool verbose;
};

#endif
