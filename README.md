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

First of all, the api is not final and may have been changed when reading this.

Peanats is entirely contained in the namespace '**peanats**', so in case of ambiguous names you can always specify to the compiler what you mean.

The main class of Peanats is the **Client** which is derived from **Peanats**. The only difference between these are that Peanats doesn't have any tcp code and its up to the user to define these when creating the **Client**'s. This is what makes peanats somewhat portable as you can create your own Client classes with custom platform specific tcp code. There are two prewritten '**Client**' classes located in "peanats/client.h". Including this file will declare a **Client** with the tcp specific code for either linux or windows depending on what host os is used when compiling. If you want to write your own tcp code you instead include "peanats/peanats.h".

The client's constructor specifies how large (or small) tcp receive buffer you want to use and this will also be the maximum total packet size that peanats can handle. So if you are on a tight memory budget, keeping your subjects and payloads small, you could probably get away with a buffer of only like 50 bytes, perfect for Arduino and embedded.

Now when you've instantiated a **Client** class you *could* attach a '**Logger**' class to it. This is a convenience feature that allows you redirect the internal logging to a custom callback where you can do anything you want with it. For example write it to the console, a file, syslog etc. This feature is disabled by default but will start outputting once a logger is attached. You can also entirely disable all the logging code by defining **PEANATS_NO_LOGGING** when compiling. *Example of a syslogger attached with a lamda function callback:*

```
client.attach(Logger([](const char* str, size_t len) { syslog(LOG_INFO, s); }))
```

At this point its time to override the **on_connect_cb** which peanats will call after connecting to a server. This callback will also trigger whenever peanats reconnects. Since peanats also support *capturing lambdas*, local function variables and arguments are not a problem. Example:

```cpp
client.on_connect_cb = [&](Peanats* cli) { std::cout << argv[0] << " has connected!\n"; };
```

In the connect callback we put our initial subscriptions and whatever other code we want. Subscriptions are created with '**client::subscribe**' which allows you to specify a **MessageCallback** function which will trigger when something is published on that subject. The return value of the subscribe is a id which you later use to unsubscribe. The callback argument is a **Message** reference which contains the subject, id, reply-to & payload. Example

```cpp
// answer anyone who posted on subject "hello" with "world"
auto hello = [](Message& msg) {
    m.client.reply(msg, "world");
}
```



From here the api should be pretty self explementary.



### Full Example

*example/nats_log.cpp*

- ```cpp
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