# Peanats

Minimalistic header-only C++ NATS client

### Introduction

Peanats is a minimalistic, fast, extendable C++ header-only NATS client. It's a **work in progress** written purely for fun and practice, so don't expect too much. That said, it's currently able to compile on both Windows and Linux, but you can add more platforms by overriding the virtual tcp functions of the Peanats base class.

The parser in Peanats is a fast state machine that works without copy or allocations, however the string building and logging in Peanats *does* use allocations and is far from optimal, but will get attention.

I got bored of this project long ago, and now over one year later I was "un-bored" and started to rework it pretty aggressively, so it may very well have bugs or other surprise features. AFAIK it's probably working as intended but it still needs more tests and code before I would consider it anything more than beta. Priority will be to get it's functionally complete, then a better api and lastly to optimization.

### Some limitations

+ Peanats forbids client NATS subscription id's (sid's) to be anything but a pure integer. This is because we want a simple, fast, internal lookup with low memory consumption. But I can't think of a reason why you would want to use such broad sid's anyway unless you want clear names to aid in debugging *or* if you want to store data in the sid's, but lets use the payload for that... Anyways, sid's are mostly hidden anyway.

### Building

You will need:

+ Ragel state machine compiler to compile the parser into a c++ header.
+ Googletest for compiling and running the unit tests.

### Building under Linux

+ apt-get install ragel
+ apt-get install googletest
+ git clone https://github.com/FladderKatten/Peanats.git
+ cd peanats
+ make all

### Directory layout

+ [src] - *The entire source of peanats is located in the 'src' directory, and it's also here you should point to when including peanats in other projects. *
+ [test] - *Contains the unit tests. Google test is required to be installed and compiled in order for these to compile.*
+ [example] - *Example code using peanats.*

### Use

*under construction, see examples*



### Example code

*example/nats_log.cpp*

- ```cpp
  // example/nats_pub.cpp
  //
  // peanats commandline example program that publishes
  // a userdefined payload on a userdefined local nats subject
  //
  // http://github.com/fladderkatten/peanats
  //
  
  #include <string>
  #include <iostream>
  #include <peanats/client.h>
  
  using namespace peanats;
  
  int main(int argc, char** argv)
  {
    // abort if not enough command line arguments
    if (argc < 3) {
      std::cout << "usage: " << argv[0] << " pub <subject> <payload>" << std::endl;
      return 0;
    }
  
    // Create and initialize a client and attach a custom console 'logger'
    Client client;
    Logger logger([](const std::string& s) { std::cout << s; });
    client.attach(&logger);
  
    // Setup the on connect callback to publish one message and then shut down the client
    client.on_connect_cb = [&](Peanats* c) {
      c->publish(argv[1], argv[2]);
      c->shutdown();
    }; //!< on_connect_cb
  
    // hand over execution to peanats
    return client.run("127.0.0.1", "4222");
  }
  ```



### Some TODO´s

+ Virtual timer for handling timed events.
+ Arduino client class.
+ Payload class for convenient payload encoding and decoding.
+ Better string building and logging.
+ INFO / CONNECT support.

  

### Resources

http://www.colm.net/open-source/ragel/                  
https://docs.nats.io  
https://docs.docker.com/docker-for-windows/install/      