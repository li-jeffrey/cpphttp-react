#include "App.h"
#include "net.h"
#include "products.h"
#include "spdlog/spdlog.h"
#include "json/json.h"
#include <algorithm>
#include <thread>

#define PORT 6123

void createTestProducts(Products::ProductManager *pm) {
  pm->createProduct("Volvo", "SUV");
  pm->createProduct("Toyota", "Camry");
  pm->createProduct("Honda", "Accord");
}

int main() {
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("%b %d %H:%M:%S.%f [%t] [%l] %s - %v");

  Products::ProductManager *pm = new Products::ProductManagerImpl();
  createTestProducts(pm);

  std::vector<std::thread *> threads(std::thread::hardware_concurrency());
  std::transform(threads.begin(), threads.end(), threads.begin(), [=](auto *t) {
    return new std::thread([=]() {
      Net::FsHandler<uWS::HttpResponse<false>, uWS::HttpRequest> fsHandler;
      Net::ProductController<uWS::HttpResponse<false>, uWS::HttpRequest>
          pdtctrl(pm);
      uWS::App()
          .any("/api/v1/products/:action",
               [&](auto *res, auto *req) { pdtctrl.handleRequest(res, req); })
          .get("/*",
               [&](auto *res, auto *req) { fsHandler.handleRequest(res, req); })
          .listen(PORT,
                  [](auto *listen_socket) {
                    if (listen_socket) {
                      SPDLOG_INFO("Listening on port {}", PORT);
                    }
                  })
          .run();
    });
  });

  std::for_each(threads.begin(), threads.end(), [](auto *t) { t->join(); });
}
