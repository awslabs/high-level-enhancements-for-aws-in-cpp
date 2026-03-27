# Development guide

## Setting up the workspace

Instructions for setting up development environment.
First, enable testing in `cmake` by passing `-DBUILD_TESTING` and use `ctest` to run the test suite.

```shell
mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
cmake --build .
ctest .
```

## Code style and static analysis

### Formatting

All C++ source files are formatted with **clang-format** using the LLVM base style (see
`.clang-format` at the project root). Before committing, format any files you have touched:

```shell
# Format a specific file
clang-format -i include/awslabs/enhanced/s3buf.h

# Format all project headers and test sources
find include test doc/examples -name "*.h" -o -name "*.cpp" | xargs clang-format -i
```

CI will reject PRs where source files are not properly formatted.

### Static analysis

The project uses **clang-tidy** for static analysis (see `.clang-tidy` at the project root).
Enable it during CMake configuration with:

```shell
cmake -DBUILD_TESTING=ON -DENABLE_CLANG_TIDY=ON ..
cmake --build .
```

`ENABLE_CLANG_TIDY` is **OFF** by default so that developer iterations stay fast. It is
recommended to enable it before opening a Pull Request.

Both tools require clang 18 or newer (matching the project's compiler baseline).

## Contributing

The framework used to support testing is [GoogleTest 1.11+](https://github.com/google/googletest).

### Bug fix patches

To submit bug fixes, please add a test showing the failing behavior that is being fixed
to prevent its reintroduction in the future.

### New features

For contributing new features we recommend creating a Pull Request with a .md doc in the feature_rfc directory.
There, we can have design discussions before investing time in the implementation of it.

Once a feature is designed, Pull Requests with the new feature implementation can be submitted.
Any new feature submission requires updates to the documentation to include the new feature, unit tests,
modifications to the CMakeLists building scripts, and usage examples. If you need help with any of these
components, please feel free to open a Pull Request with the partial contribution and explain on it what
is missing so others can contribute to complete the PR submission before merging to main.
For features that are contributed by multiple contributors, feature-branches can be requested and created.