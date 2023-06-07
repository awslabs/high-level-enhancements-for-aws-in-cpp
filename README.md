## Introduction 

This is a preview release of the High-level enhancements for AWS in C++, which consists of high-level library abstractions
to simplify common AWS use cases in C++.  

This preview release includes 
High-level abstractions for Amazon S3 and AWS Lambda, including
support for using STL algorithms with full AWS Lambda concurrency
(currently only `transform`). As an experimental preview release, expect that interfaces may change over time. Note that this
is provided on an AS-IS basis. See Section 7 of [LICENSE](LICENSE)
for more information.

## Requirements
#### For users
- `zip`
- [CMake 3.15+](https://cmake.org/)
- [AWS SDK for C++ 1.9+](https://github.com/aws/aws-sdk-cpp)
- [AWS CLI](https://docs.aws.amazon.com/cli/latest/userguide/getting-started-install.html) for uploading the AWS Lambda functions.
- A [C++20](https://isocpp.org/) compatible compiler. (C++17 may
work)
- [AWS Lambda Runtime for C++](https://github.com/awslabs/aws-lambda-cpp)
- [TL Expected](https://github.com/TartanLlama/expected) for 
compilers that do not yet support `std::expected`
#### Additional dependencies for developers
- [GoogleTest 1.11+](https://github.com/google/googletest)

To make changes to the codebase and contribute changes please read our [development guide](DEVELOPMENT.md)
## Installation
The installation follows Modern CMake idioms. A quick start script to build and install the projects is below for reference.

The following script assumes the project code was downloaded and the shell is at the project source code directory.
```shell
mkdir build
cd build
cmake ..
cmake --build . 
cmake --install . 
```
Alternatively to install, a native installation package can be created using `cpack` instead to distributing to multiple machines.
```shell 
mkdir build
cd build
cmake ..
cmake --build . 
cmake --install . 
``` 

## Usage
See the `doc/examples` directory
for usage examples. More complete tutorial documentation and videos coming shortly. Basic features are

### RAII classes
RAII classes `AwsApi` for initializing the API, `Logging` for
initializing logging.

### S3 stream abstractions
The `is3stream` and `os3stream` libraries can be used to read and
write data in `s3` in a similar style to the use of
`fstream` to read and write files. 

### Lambda abstractions
The `lambda_client.h` header facilitates
calling lambdas from C++ similarly to other callables. See  `lambda_add_example.cpp` for a simple example. The `central_limit_theorem.cpp` example shows how to use the
`transform` function template to take advantage of AWS Lambda's built-in concurrency to efficiently transform an iterator rannge. 

The `aws_lambda.h` header facilitates
the creation of lambda functions. See `lambda_add_fn.cpp` for an
example. To package the lambda function for the cloud, you can
use `aws_lambda_package_target` from the AWS Lambda Runtime for
C++ as shown in the `CMakeLists.txt` in the `examples` directory.

To deploy the lambda function from the CLI, you can use the
command 
```shell
cli_deploy_cpp_lambda lambda_name iam_role zipped_lambda_package
```
You can also use your preferred Infrastructure as Code technique,and there is an (under development) script for [AWS SAM](https://aws.amazon.com/serverless/sam/) deployment included as well. 

