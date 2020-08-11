// example/nats_sub.cpp
//
// peanats commandline example program that monitors a userdefined
// local nats subject and prints the activity
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
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " <subject>" << std::endl;
    return 0;
  }

  // Create and initialize a client and attach a custom 'logger' that 
  // redirect the output to the screen
  Client client;
  Logger logger([](const std::string& s) { std::cout << s; });
  client.attach(&logger);

  // setup the on connect callback to publish one message and then shut down
  client.on_connect_cb = [&](Peanats* c) {
    // setup a subscriber on the provided subject but since we already
    // have a logger attached we actually don't have to do anything, so
    // a empty callback is fine here
    c->subscribe(argv[1], [&](Message& m) {});
    c->logn("Listening...");
  }; //!< on_connect_cb

  // hand over execution to peanats
  return client.run("127.0.0.1", "4222");
}

