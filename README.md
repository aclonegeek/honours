# Honours Project

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
        - [Running Native Examples](#running-native-examples)
        - [Running Parallel Examples](#running-parallel-examples)

# Requirements
- C++17 compiler (GCC >= 7, Clang >= 4)
- CMake 3.16

# Dependencies
- Asio 1.14.0 (included)
- Doctest 2.3.7 (included)

# Setup, Building, & Running
## Setup and Building
- In a terminal, run:
``` shell
git clone --recurse-submodules https://github.com/aclonegeek/honours.git
cd honours
cmake -E make_directory build
cmake -E chdir build cmake ..
# TODO make command.
```

## Running
After building, the client and server executables can be located inside the `build` directory.

To the run server: `run_server <port>`(e.g. `run_server 5001`)

To run the client: `run_client <host> <port>` (e.g. `run_client localhost 5001`)

## Running Tests
After building, the tests can be located inside the `build/test` directory.

### Running The Test Suite
To run the test suite, simply run the `test_suite` executable. This will run the entire test suite.

To run a specific test suite or test suites, you can do the following:
``` shell
# List all the test suites by name.
test_suite --lts
# Run the corresponding test suite or suites based on a given filter.
# For example, test_suite --ts=Login* will run the login_logout test suite.
test_suite --ts=<filters>
```

To run a specific test case or test cases, you can do the following:
``` shell
# List all the test cases by name.
test_suite --ltc
# Run the corresponding test case or cases based on a given filter.
# For example, test_case --tc="*logs in*" will run all tests for logging in.
test_suite --tc=<filters>
```

### Running Native Examples
TODO.
### Running Parallel Examples
TODO.
