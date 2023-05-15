# High-Level Enhancements for AWS in C++ Libraries example

In the directory examples, a project composed of simple usage examples of the High-Level Enhancements for AWS in C++ libraries is included.

## Building the examples

You can copy the examples to your preferred location to build them and experiment.

```bash
cp -r ${INSTALL_PREFIX}/share/doc/awslabs-enhanced-cpp/examples preferred_location
cd preferred_location
mkdir build
cd build
cmake ..
cmake --build . 
```

The `CMakeLists.txt` file in the examples directory is part of the example, it assumes the High-level
Enhancements for AWS in C++ libraries are
installed already.

Each example is a target in `CMakeLists.txt`, the build target builds them all by default.

To run the examples, credentials need to be provided, the simplest way is to set them in the Environment Variables.

```bash
export AWS_ACCESS_KEY_ID=XXXXXXXXXXX
export AWS_SECRET_ACCESS_KEY=xcxcxcxcxcxcxc
export AWS_SESSION_TOKEN=XxcXxcXxc
./example_to_test
```

## Running the examples

### Example read_object

Example code `read_object.cpp` shows how to get the content of an object from s3 in a drop-in replacement to use
ifstream read from local files.
The filename to be read is passed by argument to the executable and used to read a local file
and a s3 object of the same name.  
The bucket name and region are hardcoded in the example.
The local file, the bucket and the object have to exist to succeed the run.

```bash
./read_object mytest
```

### Example write_object

Example code `write_object.cpp` shows how to put content to an object in s3 in a drop-in replacement to use
ofstream write to local files.
The filename to be read is passed by argument to the executable and used to write to the local file
and a s3 object of the same name.  
The bucket name and region are hardcoded in the example.
The local file, the bucket have to exist to succeed the run.

```bash
./write_object mytest
```

### Lambda addition function

The Lambda addition fn example is composed by two files.
The `lambda_add_fn.cpp` which implements `add()` function, and `lamda_add_example.cpp` which is an example app
using the `add()` function.
The `add()` function can be deployed to AWS::Lambda to be called remotely, or can be linked directly to run locally.
Moving from running the function locally to remotely requires no code changes except marking the intention.
To deploy the function to AWS::Lambda the `deploy_cpp_lambda_fn` tool has to be run before running the app.

```bash
deploy_cpp_lambda_fn lambda_add_fn.cpp
add()
```

### Lambda RR example

TODO: Complete here.
