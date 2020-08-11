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

  // Create and initialize a client and attach a custom 'logger' that 
  // logs to the console
  Client client;
  Logger logger([](const std::string& s) { std::cout << s; });
  client.attach(&logger);

  // setup the on connect callback to publish one message and then shut down
  client.on_connect_cb = [&](Peanats* c) {
    c->publish(argv[1], argv[2]);
    c->shutdown();
  }; //!< on_connect_cb

  // hand over execution to peanats
  return client.run("127.0.0.1", "4222");
}