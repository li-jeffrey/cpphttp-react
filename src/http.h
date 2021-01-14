#pragma once

#include "spdlog/spdlog.h"
#include "json/json.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "products.h"

namespace Http {

template <typename HttpResponse, typename HttpRequest> class RequestHandler {
public:
  void handleRequest(HttpResponse *res, HttpRequest *req) {
    SPDLOG_DEBUG("{} {}", req->getMethod(), req->getUrl());
    doRequest(res, req);
  }

private:
  virtual void doRequest(HttpResponse *res, HttpRequest *req) = 0;
};

template <typename HttpResponse, typename HttpRequest>
class JsonController : public RequestHandler<HttpResponse, HttpRequest> {
private:
  std::unique_ptr<Json::StreamWriter> writer;
  std::unique_ptr<Json::CharReader> reader;

protected:
  JsonController() {
    Json::StreamWriterBuilder sb;
    this->writer = std::unique_ptr<Json::StreamWriter>(sb.newStreamWriter());

    Json::CharReaderBuilder cb;
    this->reader = std::unique_ptr<Json::CharReader>(cb.newCharReader());
  }

  void parseJson(HttpResponse *res,
                 std::function<void(HttpResponse *res, Json::Value &json)> cb) {
    auto buffer = new std::vector<char>();
    res->onData([=](std::string_view chunk, bool isEnd) {
      buffer->insert(buffer->end(), chunk.begin(), chunk.end());
      if (isEnd) {
        if (buffer->empty()) {
          res->writeStatus("400 Bad Request")->end("Missing request body");
          return;
        }

        Json::Value json;
        std::string errs;
        if (this->reader->parse(&buffer->front(), &buffer->back() + 1,
                                &json, &errs)) {
          cb(res, json);
        } else {
          res->writeStatus("400 Bad Request")->end(errs);
        }
        
        delete buffer;
      }
    });
    res->onAborted([=]() {
        delete buffer;
    });
  }

  void end(HttpResponse *res) {
    res->writeStatus("204 No Content")
        ->writeHeader("Access-Control-Allow-Origin", "*")
        ->end();
  }

  void tryEnd(HttpResponse *res, Json::Value &json) {
    if (json.empty()) {
      this->end(res);
      return;
    }
    std::ostringstream str;
    this->writer->write(json, &str);

    res->writeHeader("Access-Control-Allow-Origin", "*")
        ->writeHeader("Content-Type", "application/json")
        ->end(str.str());
  }
};

template <typename HttpResponse, typename HttpRequest>
class ProductController : public JsonController<HttpResponse, HttpRequest> {
private:
  Products::ProductManager *pm;

  virtual void doRequest(HttpResponse *res, HttpRequest *req) override {
    Json::Value json;
    auto requestType = req->getMethod();
    auto param = req->getParameter(0);
    if (param == "all" && requestType == "get") {
      json << this->pm->getAllProducts();
      this->tryEnd(res, json);
    } else if (param == "create" && requestType == "post") {
      this->parseJson(res, [this](auto *res, auto &json) {
        Products::Product pdt;
        json >> pdt;
        this->pm->createProduct(pdt);
        this->end(res);
      });
    } else if (param == "update" && requestType == "post") {
      this->parseJson(res, [this](auto *res, auto &json) {
        Products::Product pdt;
        json >> pdt;
        this->pm->updateProduct(pdt);
        this->end(res);
      });
    } else if (param == "delete" && requestType == "post") {
      auto id = req->getQuery("id");
      int parsed;
      try {
        parsed = std::stoi(std::string(id));
      } catch (const std::exception &) {
        res->writeStatus("400 Bad Request")->end("Invalid Id");
        return;
      }

      this->pm->deleteProduct(parsed);
      this->end(res);
    } else {
      res->writeStatus("404 Not Found")->end("Not Found");
    }
  }

public:
  ProductController(Products::ProductManager *pm) { this->pm = pm; }
};

static inline bool hasExt(const std::string &file, const std::string &ext) {
  if (ext.size() > file.size()) {
    return false;
  }

  return std::equal(ext.rbegin(), ext.rend(), file.rbegin());
}

template <typename HttpResponse>
static void addContentType(const std::string &path, HttpResponse *res) {
  if (hasExt(path, ".html")) {
    res->writeHeader("Content-Type", "text/html");
  } else if (hasExt(path, ".css")) {
    res->writeHeader("Content-Type", "text/css");
  } else if (hasExt(path, ".js")) {
    res->writeHeader("Content-Type", "text/javascript");
  } else if (hasExt(path, ".ico")) {
    res->writeHeader("Content-Type", "image/x-icon");
  }
}

static inline unsigned int getBufSiz(const char *filename) {
  struct stat buf;
  if (stat(filename, &buf) == 0) {
    return buf.st_blksize;
  }

  return (unsigned int)BUFSIZ;
}

template <typename HttpResponse, typename HttpRequest>
class FsHandler : public RequestHandler<HttpResponse, HttpRequest> {
private:
  std::vector<char> buffer;

  virtual void doRequest(HttpResponse *res, HttpRequest *req) override {
    auto url = req->getUrl();
    std::string path = url == "/" ? "/index.html" : std::string(url);

    std::string filename = path.substr(1);
    std::ifstream in(filename);
    if (!in) {
      res->writeStatus("404 Not Found")->end("Not Found");
      return;
    }

    res->writeStatus("200 OK");
    addContentType(path, res);
    auto bufsiz = getBufSiz(filename.c_str());
    this->buffer.reserve(bufsiz);
    while (!in.eof()) {
      in.read(this->buffer.data(), bufsiz);
      auto read = in.gcount();
      std::string_view sv(this->buffer.data(), read);
      res->write(sv);
    }
    res->end();
  }
};

} // namespace Http
