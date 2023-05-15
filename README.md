## Introduction 

This is a preview release of the High-level enhanced for AWS in C++.

The SDK is a set of libraries to interface smoothly standard C++ libraries with AWS Services. 

### This preview release includes
- is3stream and os3stream libraries to access AWS S3 using C++ streams.
- aws_lambda.h and aws_client.h to bind C++ functions from AWS Lambda as standard C++ functions.
- A CLI tool to upload and setup standard C++ functions in AWS Lambda to be called from the client Application. 
## Requirements
#### For users
- [CMake 3.15+](https://cmake.org/)
- [AWS SDK for C++ 1.9+](https://github.com/aws/aws-sdk-cpp)
- [AWS CLI](https://docs.aws.amazon.com/cli/latest/userguide/getting-started-install.html) for uploading the AWS Lambda functions.
- A [C++20](https://isocpp.org/) compatible compiler.
- [AWS Lambda Runtime for C++](https://github.com/awslabs/aws-lambda-cpp)
- [TL Expected](https://github.com/TartanLlama/expected)
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
The is3stream and os3stream libraries can be used as dropping replacement of fstream equivalents. 
Examples are available in the examples' directory.

The lambda and lambda client need a complementary tool for the deployment of the function to the AWS. 
This tool is deployed to RUNTIME_DIRECTORY by cmake. 
You can upload a function as in the following example.

```shell
deploy_cpp_lambda_fn 
    --account=123567890 
    --region=us-east-1 
    --source=my_function.cpp 
    --attach-role=my_iam_role 
    --attache-policy=my_policy 
```

In this example, the function will be build with additional code to allow it to be externally called.
If build succeeds, the binary will be uploaded to AWS Lambda in the region specified, and the IAM role and security policies passed will be attached to it. 

To use the new function from lambda, it can be bind to a function pointer and called as show in the examples' directory. 