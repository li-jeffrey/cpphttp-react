#pragma once

#include "spdlog/spdlog.h"
#include "json/json.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "products.h"

namespace Net {

template <typename HttpResponse, typename HttpRequest> class RequestHandler {
public:
  void handleRequest(HttpResponse *res, HttpRequest *req) {
    SPDLOG_DEBUG("Handle request: {}", req->getUrl());
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
  std::vector<char> buffer;

  static void internalTryEnd(HttpResponse *res, const char *str,
                             size_t remaining) {
    if (!res->tryEnd(str, remaining).first) {
      res->onWritable([&](int offset) {
        internalTryEnd(res, str + offset, remaining - offset);
        return true;
      });
    }
  }

protected:
  JsonController() {
    Json::StreamWriterBuilder sb;
    this->writer = std::unique_ptr<Json::StreamWriter>(sb.newStreamWriter());

    Json::CharReaderBuilder cb;
    this->reader = std::unique_ptr<Json::CharReader>(cb.newCharReader());
  }

  void parseJson(HttpResponse *res,
                 std::function<void(HttpResponse *res, Json::Value &json)> cb) {
    this->buffer.clear();
    res->onData([&](std::string_view chunk, bool isEnd) {
      this->buffer.insert(this->buffer.end(), chunk.begin(), chunk.end());
      if (isEnd) {
        if (this->buffer.empty()) {
          res->writeStatus("400 Bad Request")->end("Missing request body");
          return;
        }

        Json::Value json;
        std::string errs;
        if (this->reader->parse(&this->buffer.front(), &this->buffer.back() + 1,
                                &json, &errs)) {
          cb(res, json);
        } else {
          res->writeStatus("400 Bad Request")->end(errs);
        }
      }
    });
  }

  void tryEnd(HttpResponse *res, Json::Value &json) {
    if (json.empty()) {
      res->writeStatus("204 No Content")->end();
      return;
    }
    std::ostringstream str;
    this->writer->write(json, &str);
    auto strdata = str.str();

    res->writeStatus("200 OK")
        ->writeHeader("Access-Control-Allow-Origin", "*")
        ->writeHeader("Content-Type", "application/json");

    internalTryEnd(res, strdata.data(), strdata.size());
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
      Products::serialize(this->pm->getAllProducts(), json);
      this->tryEnd(res, json);
    } else if (param == "update" && requestType == "post") {
      this->parseJson(res, [this](auto *res, auto &json) {
        Products::Product pdt{json["Id"].asInt(), json["Name"].asString(),
                              json["Description"].asString()};
        this->pm->updateProduct(pdt);
        res->writeStatus("204 No Content")->end();
      });
    } else if (param == "delete" && requestType == "delete") {
      auto id = req->getQuery("id");
      int parsed = std::stoi(std::string(id));
      this->pm->deleteProduct(parsed);
      res->writeStatus("204 No Content")->end();
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

} // namespace Net
