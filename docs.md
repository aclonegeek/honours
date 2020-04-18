# Documentation

## Setup, Building, & Running

See (README)[README.md].

## Architecture

This is an asynchronous client-server application.

### Server
When launched, the `Server` binds to a port and waits for connections from `Client`s. It also initializes the `University` object, which it will later share (as a reference) with all of its sessions. When it receives a connection, it creates a `TemporarySession`. This `TemporarySession` is used to determine what type the user is - a `Clerk` or a `Student`. After the user enters their desired type, a `ClerkSession` or `StudentSession` is created, which will then handle all future interactions between the client and the server, and will be directly responsible for manipulating the `University`.

### Client

When launched, the `Client` connects to the server. It receives input from stdin and prints out what it receives from the server on stdout.

### University

The `University` holds all of the `Course`s and `Student`s. It keeps track of its state, of which it has 4 of: Pre-registration, registration, term, and end of term. When initialized, it launches 3 timers that will change to the next corresponding state when appropriate. The `University` also defines the length of a day, and how long each period (state) will last for.

## Tests

All tests can be located inside the `test/doctest` directory.

### Acceptance Test Suite

For all scenarios inside the `scenarios` and `scenarios/native` directories (not the `parallel` directory), an inherited `ScenarioContext` is defined and used. A `ScenarioContext` is a class that is responsible for initializing and handling all resources that a scenario needs - generally, this means launching the server, firing up the clients, connecting them, and then potentially creating students and/or courses. The `ScenarioContext` itself only handles launching the server - the rest depends on what the feature requires.

### Native and Parallel

The `native` and `parallel` directories hold implementations of the two specific examples noted below. Native acts as native Cucumber would (without true parallel support, it spawns its own threads). Parallel acts as parallel Cucumber would (spawning each scenaio on a thread of its own).

1. 3 students attempting to register for a single course with only 1 slot left.
2. A course has a capsize of 3, with 2 students registered in it. Then, one of the students deregisters from the course while simultaneously 2 other students try to register in the course.
