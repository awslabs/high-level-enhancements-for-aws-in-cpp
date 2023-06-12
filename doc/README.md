# High-Level Enhancements for AWS in C++ Libraries example

In the directory examples, a project composed of simple usage examples of the High-Level Enhancements for AWS in C++ libraries is included.

## Quick tutorial
More complete tutorial documentation and videos coming shortly. 
### Prerequisites
* Ensure that the High-Level Enhancements for AWS in C++ are installed as described
in the [Installation Instructions](https://github.com/awslabs/high-level-enhancements-for-aws-in-cpp#installation). We
recommend adding the installation location's `bin` directory to your `PATH`

* You will need an AWS Account. If you don't have one, you can 
get one through
the [AWS Free Tier](https://aws.amazon.com/free).
* Client code should be run in an environment with AWS credentials.  A simple way is to set them in the Environment Variables:

    ```bash
    export AWS_ACCESS_KEY_ID=XXXXXXXXXXX
    export AWS_SECRET_ACCESS_KEY=xcxcxcxcxcxcxc
    export AWS_SESSION_TOKEN=XxcXxcXxc
    ./example_to_test
    ``` 
    For alternatives, see [Authentication and Access Credentials](https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-authentication.html).

* To use the [Lambda Abstractions](#lambda-abstractions), you will
need to have an [IAM role](https://docs.aws.amazon.com/IAM/latest/UserGuide/id_roles.html) specifying the permissions for your Lambda
function. See [this page](https://docs.aws.amazon.com/lambda/latest/dg/lambda-intro-execution-role.html) for instructions
on creating a basic Lambda execution role sufficient for our examples. Save the [ARN](https://docs.aws.amazon.com/IAM/latest/UserGuide/reference-arns.html) that you get when you create the role for later use

    Of course, if you have a preferred way of creating roles, such
as the [CDK](https://aws.amazon.com/cdk/), you are free to use that.
### Copying the examples
 copy the examples to your preferred location to build them and experiment.

```bash
cp -r ${INSTALL_PREFIX}/share/doc/awslabs-enhanced-cpp/examples preferred_location
cd preferred_location
mkdir build
cd build
cmake ..
cmake --build . 
```

### S3 examples
The `doc/examples/s3stream` examples demonstrate how to use
[Amazon S3](https://aws.amazon.com/s3/) cloud object store 
using familiar `fstream`-style syntax and IOStreams.  **Note:** Since S3 requires globally unique bucket names, you may need to change the bucket name
in the code.

#### Example read_object

Example code `read_object.cpp` shows how to get the content of an object from s3 in a drop-in replacement to use
ifstream read from local files.
The filename to be read is passed by argument to the executable and used to read a local file
and a s3 object of the same name.  
The bucket name and region are hardcoded in the example.
The local file, the bucket and the object have to exist to succeed the run.

```bash
./read_object mytest
```

#### Example write_object

Example code `write_object.cpp` shows how to put content to an object in s3 in a drop-in replacement to use
ofstream write to local files.
The filename to be read is passed by argument to the executable and used to write to the local file
and a s3 object of the same name.  
The bucket name and region are hardcoded in the example.
The local file, the bucket have to exist to succeed the run.

```bash
./write_object mytest
```




### Lambda examples
**Note**: While the client programs can run on a wide variety
of platforms that support the AWS C++ SDK as listed 
[here](https://aws.amazon.com/sdk-for-cpp/), the Lambda function
package itself should be built on a Linux machine as it will
be run in a Linux environment in the cloud (we plan to investigate
cross-compilation later).

#### Add example
This program creates and uses a Lambda function that adds its two
integer arguments.

1. Build the `aws-lambda-package-lambda_add_fn` target. This will produce
a Lambda function package that we will upload to the cloud.

1. Deploy the Lambda function to the cloud. One way to do this is
    ```shell
    cd build
    cli_deploy_cpp_lambda add iam_role lambda_add_fn.zip
    ```
    where you should substitute the ARN of the role you created in
[Prerequisites](#prerequisites) for `iam_role`. If you forgot
to save the ARN of the role, you can find it in the AWS IAM Console. If
the `cli_deploy_cpp_lambda` command is not found, it is in the `bin` subdirectory
of the High-Level Enhancements for AWS in C++ installation directory.

1. Build and run the `lambda_add` target. It should call your cloud Lambda
to perform the addition. If you want, you can verify this in the 
monitoring tab in the AWS Lambda Console.

#### Central Limit Theorem example
This example demonstrates how [Lambda Scaling](https://docs.aws.amazon.com/lambda/latest/dg/lambda-concurrency.html) can improve performance. 
On our test machine, this program takes 15 minutes to run locally and 9
seconds to run in the cloud.
(Officially, the program histograms the means of the exponential distribution to illustrate the [Central Limit Theorem](https://en.wikipedia.org/wiki/Central_limit_theorem), but that's not really the point here...). 

1. Build the `aws-lambda-package-lambda_exp_mean_fn` target. This will produce
a Lambda function package that we will upload to the cloud.

1. deploy the Lambda function to the cloud. One way to do this is
    ```shell
    cd build
    cli_deploy_cpp_lambda exp_mean iam_role exp_mean_fn.zip
    ```
    where you should substitute the ARN of the role you created in
[Prerequisites](#prerequisites) for `iam_role`. If you forgot
to save the ARN of the role, you can find it in the AWS IAM Console.  If
the `cli_deploy_cpp_lambda` command is not found, it is in the `bin` subdirectory
of the High-Level Enhancements for AWS in C++ installation directory.

1. Build the `central_limit_theorem` target.

1. Run it locally and then in the cloud to compare the performance.
    ```shell
    time ./central_limit_theorem --policy par # std::execution::par
    time ./central_limit_theorem --policy cloud
    ```

