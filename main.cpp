#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <numeric>
#include <random>
#include <algorithm>
#include <optional>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <iomanip> 

using namespace std;

const double DEFAULT_NU = 3.0;
const double DEFAULT_LAMBDA = 1.0;
const double DEFAULT_SIGMA = 0.5;
const double DEFAULT_ZETA = 0.0;
const int DEFAULT_ITERATION_COUNT = 1000;

typedef vector<double> Vec;
typedef function<double(const Vec&)> Oracle;

Vec bary_batch(Oracle oracle, const vector<Vec>& xs, double nu = DEFAULT_NU) {
    if (oracle(xs[0]) == 0.0) {
        throw invalid_argument("Oracle function must evaluate as a scalar value.");
    }

    // Log: oracle evaluation for the first element
    auto bexp_fun = [&](const Vec& x) {
        double result = exp(-nu * oracle(x));
        return result;
    };

    vector<future<double>> futures(xs.size());  // For parallelized weights calculation

    // Launch threads for parallelized bexp_fun
    for (size_t i = 0; i < xs.size(); ++i) {
        futures[i] = std::async(launch::async, bexp_fun, xs[i]);
    }

    vector<double> x_weights(xs.size());
    for (size_t i = 0; i < xs.size(); ++i) {
        x_weights[i] = futures[i].get();  // Wait for each thread to finish and get the result
    }

    Vec barycenter(xs[0].size(), 0.0);
    double weight_sum = accumulate(x_weights.begin(), x_weights.end(), 0.0);

    // Check if the weight_sum is too small to avoid division by zero or instability
    if (weight_sum < 1e-10) {
        cerr << "Warning: weight_sum is too small, adjusting to avoid division by zero" << endl;
        weight_sum = 1e-10;  // Set to a small but non-zero value
    }

    for (size_t i = 0; i < xs.size(); ++i) {
        for (size_t j = 0; j < xs[i].size(); ++j) {
            barycenter[j] += xs[i][j] * x_weights[i] / weight_sum;
        }
    }

    return barycenter;
}

pair<double, Vec> bary_recursive(
    Oracle oracle, Vec x0, double nu = DEFAULT_NU,
    double sigma = DEFAULT_SIGMA, double zeta = DEFAULT_ZETA,
    double lambda_ = DEFAULT_LAMBDA, int iterations = DEFAULT_ITERATION_COUNT) {

    Vec xhat = x0;
    double m = 0.0;
    bool solution_is_found = false;

    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> dist(0.0, sigma);

    auto update = [&](double weight_total, Vec& estimate, optional<Vec> prev_step = nullopt) {
        // Ensure estimate and prev_step have the same size before proceeding
        if (prev_step && prev_step->size() != estimate.size()) {
            throw runtime_error("prev_step and estimate vectors must have the same size. "
                                 "Estimate size: " + to_string(estimate.size()) +
                                 ", prev_step size: " + to_string(prev_step->size()));
        }
    
        Vec step(estimate.size(), 0.0);
        Vec curiosity(estimate.size(), 0.0);
    
        if (prev_step) {
            for (size_t i = 0; i < estimate.size(); ++i) {
                curiosity[i] = dist(gen) - zeta * (*prev_step)[i];
            }
        } else {
            for (size_t i = 0; i < estimate.size(); ++i) {
                curiosity[i] = dist(gen);
            }
        }
    
        Vec probe(estimate.size(), 0.0);
        for (size_t i = 0; i < estimate.size(); ++i) {
            probe[i] = estimate[i] + curiosity[i];  // Add curiosity to probe
        }
    
        double weight = exp(-nu * oracle(probe));
        weight_total = lambda_ * weight_total + weight;
    
        for (size_t i = 0; i < estimate.size(); ++i) {
            step[i] = curiosity[i] * weight / weight_total;
            estimate[i] += step[i];
        }
    
        return make_tuple(weight_total, estimate, step);
    };    

    int i = 1;
    optional<Vec> step = nullopt;  // Use nullopt at the start
    while (!solution_is_found) {
        tie(m, xhat, step) = update(m, xhat, step);
        solution_is_found = i >= iterations;
        ++i;
    }

    return {m, xhat};
}

int main() {
    // Define the Oracle function: Here we use the L2 norm as an example
    auto oracle = [](const Vec& x) {
        return accumulate(x.begin(), x.end(), 0.0, [](double sum, double xi) {
            return sum + xi * xi;
        });
    };

    // Example input data (list of vectors for bary_batch)
    vector<Vec> xs = {{1.0, 1.0}, {-1.0, 1.0}, {0.0, -1.0}};
    
    // Call bary_batch function
    Vec barycenter = bary_batch(oracle, xs, 3.0);
    cout << "Batch Barycenter: ";
    for (double value : barycenter) {
        cout << value << " ";
    }
    cout << endl;

    // Example initial point for bary_recursive
    Vec x0 = {1.0, 1.0};
    
    // Call bary_recursive function
    auto start_time = std::chrono::high_resolution_clock::now();

    // Call bary_recursive function
    auto [m, xhat] = bary_recursive(oracle, x0, 3.0, 1.0, 0.0, 1.0, 1000000);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    cout << "Time taken: " << setprecision(4) << duration.count() << " seconds" << endl;


    // Output the result
    cout << "Recursive Barycenter: ";
    for (double value : xhat) {
        cout << value << " ";
    }
    cout << endl;

    return 0;
}
