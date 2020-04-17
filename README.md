# COMP4905 - Honours Project

# Table of Contents
- [Honours Project](#honours-project)
- [Table of Contents](#table-of-contents)
- [Requirements](#requirements)
- [Dependencies](#dependencies)
- [Setup, Building, & Running](#setup-building--running)
    - [Setup and Building](#setup-and-building)
    - [Running](#running)
    - [Running Tests](#running-tests)
        - [Running The Test Suite](#running-the-test-suite)
        - [Running Native Tests](#running-native-tests)
        - [Running Parallel Tests](#running-parallel-tests)

# Requirements
- C++17 compiler (GCC >= 7, Clang >= 4, MSVC >= 19.25)
- CMake >= 3.16 (make sure it's on the PATH)
- git

# Dependencies
- Asio 1.16.0 (included)
- Doctest 2.3.7 (included)

# Setup, Building, & Running
## Setup and Building
- In a terminal, run:
``` shell
git clone -b final-submission --single-branch --recurse-submodules https://github.com/aclonegeek/honours.git
cd honours
cmake -E make_directory build
cmake -E chdir build cmake ..
cmake --build build
```

**Note**: Downloading the repository as a zip will not pull in the third_party submodules.

**Note**: If building with MSVC, it will place executables one directory deeper inside a Debug directory. `run_server` and `run_client` end up in `build/Debug` and `test_suite` ends up in `build/test/Debug/test_suite` (the directory will be Release if built in Release mode).

## Running
The client and server executables can be located inside the `build` directory.

To the run server: `run_server <port>`(e.g. `run_server 5001`)

To run the client: `run_client <host> <port>` (e.g. `run_client localhost 5001`)

## Running Tests
The test suite can be located inside the `build/test` directory.

### Running The Test Suite
The code for the test suite is located in `test/doctest/scenarios/`.

To run the test suite, simply run `test_suite --tce=*EX1*,*EX2*`. This will run the entire test suite except the native and parallel tests for ex1 and ex2.

To run a specific test suite or test suites, you can do the following:
``` shell
# List all the test suites by name.
test_suite --lts
# Run the corresponding test suite or suites based on a given filter.
# For example, test_suite --ts="Logging in" will run the Logging in test suite.
test_suite --ts=<filters>
```

To run a specific test case or test cases, you can do the following:
``` shell
# List all the test cases by name.
test_suite --ltc
# Run the corresponding test case or cases based on a given filter.
# For example, test_case --tc="*logs in*" will run all tests for logging in.
# Note: The *'s at the front and end of the filter are important - all test cases have the text "Scenario:" (minus the quotes) prepended to them, and we want to filter that out.
test_suite --tc=<filters>
```

### Running Native Tests
The code for the native tests is located in `test/doctest/scenarios/native/`.

Run `test_suite --tc=*EX?N*` where ? is 1 or 2 depending on the native test that you want to run.

### Running Parallel Tests
The code for the native tests is located in `test/doctest/scenarios/parallel/`.

**Note**: If on Windows, append `-C Debug` to the following commands, right after `--verbose`.

For ex1, run `ctest -R ex1.* -j6 --verbose`.
For ex2, run `ctest -R ex2.* -j7 --verbose`.
