#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <algorithm>
#include <execution>
#include <functional>
#include "exp_mean.h"
#include "awslabs/enhanced/lambda_client.h"
#include "awslabs/enhanced/Aws.h"
#include <cxxopts.hpp>
#include <vector>
#include <fmt/format.h>

using std::map;
using std::vector;
using std::generate;
using std::accumulate;
using fmt::format;
using std::tuple;
using std::string;
using std::ostream;
using std::cout;
using std::cerr;
using std::transform;
using std::function;
using namespace std::execution;
using namespace AwsLabs::Enhanced;


AwsApi api;
AwsLogging awsLogging(Aws::Utils::Logging::LogLevel::Trace, "enhanced_lambda_");
#ifdef THREAD_POOL
auto clientConfig = []() {
    Aws::Client::ClientConfiguration config;
    config.executor = Aws::MakeShared<Aws::Utils::Threading::PooledThreadExecutor>("clt", 100);
    return config;
}();

EnhancedLambdaClient client(clientConfig);
#else
EnhancedLambdaClient client;
#endif

auto cloud_exp_mean = BIND_AWS_LAMBDA(client, exp_mean, "exp_mean");

struct opts {
    double lambda;
    unsigned samples;
    unsigned experiments;
    bool cloud;
    string policy;
};

// Get command line options
auto get_opts(int argc, char *argv[])
{
    cxxopts::Options options("central limit theorem", "Leverage the cloud to test the central limit theorem for the (far from normal) exponential distribution");
    options.add_options()
        ("l,lambda", "Lambda parameter for exponential distribution (default = 1)", 
         cxxopts::value<double>()->default_value("1"))
        ("s,samples", "Number of samples in each experiment (default = 50000000)", 
          cxxopts::value<unsigned>()->default_value("50000000"))
        ("e,experiments", "Number of experiments to perform (default = 500)",
         cxxopts::value<unsigned>()->default_value("500"))
        ("c,cloud", "Run in the cloud",
         cxxopts::value<bool>()->default_value("false"))
        ("p,policy", "seq, par (default), cloud",
         cxxopts::value<string>()->default_value("par"));
    auto result = options.parse(argc, argv);
    return opts(result["lambda"].as<double>(),
                result["samples"].as<unsigned>(),
                result["experiments"].as<unsigned>(),
                result["cloud"].as<bool>(),
                result["policy"].as<string>());
}

// Print results as a histogram
struct scaled_hist {
    scaled_hist(vector<double> const &vals) {
        auto [min_val, max_val] = std::minmax_element(vals.begin(), vals.end());
        bucket_size = (*max_val - *min_val)/10; // Want about 10 buckets
        for(auto const &val: vals ) {
                ++hist[ val/bucket_size];
        }
    }

    inline friend ostream &operator<<(ostream &os, scaled_hist const &sh) {
        size_t constexpr stars = 100; // We want approximately 100 stars
        auto const total = accumulate(sh.hist.begin(), sh.hist.end() , 0.0, [](auto acc, auto kv) { return acc + kv.second; });
        unsigned const val_per_star = total / stars + 1; // Don't divide by zero

        for (auto const& [x, y] : sh.hist) {
            std::cout << format("{:6.3f}-{:6.3f} {:*>{}}\n", x*sh.bucket_size, (x+1)*sh.bucket_size, "", y / val_per_star);
        }
        return os;
    }
    double bucket_size{}; 
    map<int, unsigned> hist;
};

// The actual code
void run_test(auto policy, opts const &o, auto f)
{
    vector<exp_parameters> specification( o.experiments, { o.lambda, o.samples});
    vector<double> means(o.experiments);

    transform(policy, specification.begin(), specification.end(), means.begin(), f);

    cout << scaled_hist(means);

}



int main(int argc, char *argv[])
{
    opts o = get_opts(argc, argv);
    if(o.policy == "seq")
        if(o.cloud)
            run_test(seq, o, cloud_exp_mean);
        else
            run_test(seq, o, exp_mean);
    else if (o.policy == "par")
        run_test(par, o, exp_mean);
    else if (o.policy == "par_unseq")
        run_test(par_unseq, o, exp_mean);
    else if (o.policy == "cloud_launch" || o.policy == "cloud")
        run_test(cloud_launch::cloud, o, cloud_exp_mean);
    else
        cerr << format("Invalid execution policy: {}", o.policy);
    return 0;
}