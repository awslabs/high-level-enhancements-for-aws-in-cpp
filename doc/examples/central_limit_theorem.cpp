#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
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
using namespace AwsLabs::Enhanced;


AwsApi api;
AwsLogging awsLogging(Aws::Utils::Logging::LogLevel::Trace, "enhanced_lambda_");
EnhancedLambdaClient client;

auto cloud_exp_mean = BIND_AWS_LAMBDA(client, exp_mean, "exp_mean");

struct opts {
    double lambda;
    int samples;
    int experiments;
    bool cloud;
};

auto get_opts(int argc, char *argv[])
{
    cxxopts::Options options("central limit theorem", "Leverage the cloud to test the central limit theorem for the (far from normal) exponential distribution");
    options.add_options()
        ("l,lambda", "Lambda parameter for exponential distribution", 
         cxxopts::value<double>()->default_value("1"))
        ("s,samples", "Number of samples in each experiment", 
          cxxopts::value<int>()->default_value("1"))
        ("e,experiments", "Number of experiments to perform",
         cxxopts::value<int>()->default_value("1000"))
        ("c,cloud", "Run in the cloud",
         cxxopts::value<bool>()->default_value("false"));
    auto result = options.parse(argc, argv);
    return opts(result["lambda"].as<double>(),
                result["samples"].as<int>(),
                result["experiments"].as<int>(),
                result["cloud"]. as<bool>());
}

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


int main(int argc, char *argv[])
{
    opts o = get_opts(argc, argv);
 
    vector<double> means(o.experiments);
    if(o.cloud)
        generate(means.begin(), means. end(), [&] {return cloud_exp_mean(o.lambda, o.samples); });
    else
        generate(means.begin(), means.end(), [&] {return exp_mean(o.lambda, o.samples); });

    cout << scaled_hist(means);
    return 0;
}