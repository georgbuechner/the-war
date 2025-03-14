#include <iostream>
#include <memory>
#include <thread>
#include <stdlib.h>     
#include <time.h>      

#include "field/field.h"
#include "server/server_frame.h"
#include "utils/utils.h"

//Namespaces for http server.
using namespace httplib;

int main() {
  std::cout << "--- Hello War ---\n" << std::endl;
  srand (time(NULL));

  std::shared_ptr<Field> field = std::make_shared<Field>(util::LoadJsonFromDisc("config.json"));
  
  // Create httpserver
  ServerFrame http = ServerFrame(field
#ifdef _DEVELOPMENT_
      , server_config["certificate-paths"]["cert"],
      server_config["certificate-paths"]["privkey"],
#endif
   );

  // Start http server in separat thread
  std::thread thread_http([&http]() {
    std::cout << "Starting http server." << std::endl;
    int return_code = http.Start(4444);
    std::cout << "Http-server closed with error code: " << return_code << std::endl;
  });
  std::thread thread_field([&field]() {
      field->DoPhases();
  });

  thread_http.join();
  thread_field.join();

  return 0;
}
