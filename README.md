# COMP4905 - Honours Project

# Table of Contents
- [COMP4905 - Honours Project](#comp4905---honours-project)
- [Table of Contents](#table-of-contents)
- [Requirements](#requirements)
- [Dependencies](#dependencies)
- [Documentation](#documentation)
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

# Documentation

See [docs](docs.md).

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

**Note**: If building with MSVC, it will place executables one directory deeper inside a `Debug` or `Release` directory, depending on the configuration built with (the default is `Debug`). `run_server` and `run_client` end up in `build/Debug` and `test_suite` ends up in `build/test/Debug/test_suite` (the directory will be Release if built in Release mode).

## Running
The client and server executables can be located inside the `build` directory.

The university's periods (e.g. pre-registration -> registration) are currently set for optimizing test speed (if you run the client, you won't be able to do much because it'll reach term and end of term fast). See the note about them below in [Running Tests](#running-tests).

To the run server: `run_server <port>`(e.g. `run_server 5001`).

To run the client: `run_client <host> <port>` (e.g. `run_client localhost 5001`).

The password for the clerk account is `admin`.

## Running Tests
The test suite can be located inside the `build/test` directory.

The tests depend on the timing values defined inside the `University`'s header file - if they are decreased, the tests may fail. Increasing the values is (should be :)) fine.

The tests also depend on sleeps. If the tests are failing, try adjusting the sleep value inside the `step_definitions.cpp` file inside the `void wait_for_action_to_finish()` function, or wherever else you encounter sleeps.

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

**Note**: If on Windows, append `-C Debug` or `-C Release` depending on the configuration built with to the following commands, right after `--verbose`.

For ex1, run `ctest -R ex1.* -j6 --verbose`.

For ex2, run `ctest -R ex2.* -j7 --verbose`.
