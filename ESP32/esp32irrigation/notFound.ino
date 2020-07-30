void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
