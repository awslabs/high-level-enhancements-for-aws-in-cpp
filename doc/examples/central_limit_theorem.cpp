#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include "exp_mean.h"
#include "awslabs/enhanced/lambda_client.h"
#include "awslabs/enhanced/Aws.h"
#include <cxxopts.hpp>

using namespace AwsLabs::Enhanced;
AwsApi api;
AwsLogging awsLogging(Aws::Utils::Logging::LogLevel::Trace, "enhanced_lambda_");
EnhancedLambdaClient client;

auto cloud_exp_mean = BIND_AWS_LAMBDA(client, exp_mean, "exp_mean");

int main(int argc, char *argv[])
{
    cxxopts::Options options("central limit theorem", "Leverage the cloud to test the central limit theorem for the (far from normal) exponential distribution");
    options.add_options()
        ("l,lambda", "Lambda parameter for exponential distribution", 
         cxxopts::value<double>()->default_value("1"))
        ("s,samples", "Number of samples in each experiment", 
          cxxopts::value<int>()->default_value("1"))
        ("e,experiments", "Number of experiments to perform",
         cxxopts::value<int>()->default_value("10000"))
        ("c,cloud", "Run in the cloud",
         cxxopts::value<bool>()->default_value("false"));
    auto result = options.parse(argc, argv);
    auto lambda = result["lambda"].as<double>();
    auto samples = result["samples"].as<int>();
    auto experiments = result["experiments"].as<int>();
    auto cloud = result["cloud"]. as<bool>();

    std::map<int, int> hist;

    for (int n = 0; n < experiments; ++n) {
        double result = cloud? cloud_exp_mean(lambda, samples)
                             : exp_mean(lambda, samples);
        ++hist[2 * result];
    }
    // Inspired by https://en.cppreference.com/w/cpp/numeric/random/exponential_distribution
    for (auto const& [x, y] : hist)
        std::cout << std::fixed << std::setprecision(1)
                  << x / 2.0 << '-' << (x + 1) / 2.0 << ' '
                  << std::string(y / (1 + experiments/50), '*') << '\n';
    return 0;
}