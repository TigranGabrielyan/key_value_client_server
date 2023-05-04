
# Key-Value Remote Manager
Remote server application to manage "Key=Value" type of strings provided by client applications.

# Server
- Implemented in C
- Works as daemon
- Stores keys and values
- Provides the following operation to the clients:
    - Insert, Delete, List, Search, Count
- Connection via TCP/IP
- Handles multiple connections without threads

# Client
- Implemented in C
- Accepts server configuration (IP and port) as a command line argument in "IP:Port" format
- Provides the following operations:
    - list - Get and print all Key/Value pairs from the server
    - put Key=Value - Send Key/Value pair to server to store
    - get Key - Get value for specified Key
    - del Key - Delete Key/Value pair with specified Key form the server
    - count - Get the count of the Key/Value pairs stored on the server

# Further Improvements

 - TDB

# Environment setup
Following tools are required for successful build and execution of the JEL library.

 - GCC and G++.
 - CMake (version 3.18 or higher)
 - GoogleTest
 - doxygen

# External Dependencies
Following external libraries are used:
 - Apache Portable Runtime v1.7 (https://apr.apache.org/).
Used to work with hash tables and file IO. The library is precompiled and stored under `external` folder in the repository.

# How To Build
 - To build the server, client as well as tests `./build.sh` command should be executed. 
 - To generate doxygen documentation `./doc_gen.sh` should be executed. Documentation will be generated in the `./docs` folder.
 - To clean all temporary generated file `./clean.sh` should be executed.

# How To Run
TBD