# Pollster
C++ Polling Class for BSD/MacOS &amp; Linux (in-progress)

## Usage:
It is recommended that you use cmake and link against the "pollster" library rule created by this project.
Code:
```c++
#include <Pollster.hpp>
 //Create subclass of Pollster::Handler
 class ExampleHandler : Pollster::Handler {
 public:
     //Action when new data is available on fd
     void operator()(int fd) { /*...*/ } 
     
     //Action to perform upon adding fd to Pollster
     void connect(int fd) { /*...*/ }
     
     //Action to perform when pollster fails or when fd disconnects
     //Should close fd and cleanup and related data
     //Note, if disconnect tries to send data to the fd, it should
     //handle SIGPIPEs or send with MSG_DONTWAIT in case the fd
     //is already closed before data is sent
     void disconnect(int fd, std::string reason) { /*...*/ }
     
 };
 //Create instance of pollster providing a handler instance and the max clients to poll
 ExampleHandler p;
 Pollster poll(10, p);
 //Add clients
 if(!poll.addClient(fd)){
     //Handle error or too many clients
 }  
```
cmake rule:
```cmake
#build pollster before any dependant executables/libraries
add_subdirectory(Pollster)

#generate executable here
#i.e.
add_executable(test test.cpp) #example

#link against pollster
target_link_libraries(test pollster)
```
