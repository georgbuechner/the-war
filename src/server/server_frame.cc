#include <httplib.h>
#include <memory>
#include <string>
#include "server_frame.h"
#include "../utils/utils.h"
#include "field/field.h"

#define CORS_ALLOWED_ORIGIN "http://gc-android"
#define UI_PATH "web/"
#define COOKIE_NAME "SESSID"

using namespace httplib;

std::string GetCookie(const Request &req);

ServerFrame::ServerFrame(std::shared_ptr<Field> field, std::string path_to_cert, std::string path_to_key) 
#ifdef _DEVELOPMENT_
  server_(path_to_cert.c_str(), path_to_key.c_str()) 
#endif
{ 
  _field = field;
  std::cout << "Created server." << std::endl;
}

int ServerFrame::Start(int port) {
  // Add specific handlers for user-managment.
  server_.Get("/api/heartbeat", [&](const Request& req, Response& resp) { resp.status = 200; });
  server_.Get("/api/bf/map", [&](const Request& req, Response& resp) { 
      resp.set_content(_field->GetFieldJson().dump(), "application/json"); });
  server_.Post("/api/bf/pause", [&](const Request& req, Response& resp) { 
      _field->set_paused(true);
      resp.status = 200;
    });
  server_.Post("/api/bf/unpause", [&](const Request& req, Response& resp) { 
      _field->set_paused(false);
      resp.status = 200;
    });


  //Add simple handler for css, javaskript and images 
  server_.Get("/", [](const Request& req, Response& resp) {
    resp.set_content(util::GetPage(UI_PATH "index.html"), "text/html"); 
  });
  server_.Get("/js/general.js", [](const Request& req, Response& resp) {
    resp.set_content(util::GetPage(UI_PATH "js/general.js"), "application/javaskript"); 
  });
  server_.Get("/images/neon-world-map.jpg", [](const Request& req, Response& resp) {
    resp.set_content(util::GetPage(UI_PATH "images/neon-world-map.jpg"), "image/jpg"); 
  });


  std::cout << "Successfully started http-server on port: " << port << std::endl;
  server_.set_keep_alive_max_count(1);
  return server_.listen("0.0.0.0", port);
}

//Handler

bool ServerFrame::IsRunning() {
  return server_.is_running();
}

void ServerFrame::Stop() {
  server_.stop();
}


ServerFrame::~ServerFrame() {
  Stop();
}

