#pragma once

#include "gmock/gmock.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <unordered_map>

class MockRequest {
public:
  MOCK_METHOD(std::string_view, getUrl, ());
  MOCK_METHOD(std::string_view, getMethod, ());
  MOCK_METHOD(std::string_view, getParameter, (unsigned int));
  MOCK_METHOD(std::string_view, getQuery, ());
  MOCK_METHOD(std::string_view, getQuery, (std::string_view));
};

class MockResponse {
private:
  std::string status;
  std::unordered_map<std::string, std::string> header;
  std::ostringstream body;
  bool ended = false;

public:
  std::vector<std::string> postData; // chunks

  MockResponse *writeHeader(std::string_view key, std::string_view value) {
    assert(!ended);
    header[std::string(key)] = std::string(value);
    return this;
  }

  MockResponse *writeStatus(std::string_view status) {
    assert(!ended);
    this->status = std::string(status);
    return this;
  }

  MockResponse *write(std::string_view data) {
    assert(!ended);
    this->body << data;
    return this;
  }

  void onWritable(std::function<void(int offset)> cb) { (void)cb; }

  void onData(std::function<void(std::string_view chunk, bool isEnd)> cb) {
    if (postData.empty()) {
      return;
    }

    for (size_t i = 0; i < postData.size() - 1; i++) {
      cb(postData[i], false);
    }

    cb(postData.back(), true);
    return;
  }

  void onAborted(std::function<void()> cb) {}

  std::pair<bool, bool> tryEnd(std::string_view data, size_t size) {
    assert(!ended);
    this->body << data;
    this->ended = true;
    return {true, true};
  }

  void end(std::string_view data = {}) {
    assert(!ended);
    this->body << data;
    this->ended = true;
  }

  std::string getStatus() const { return this->status; }

  std::string getHeaderValue(const std::string &key) const {
    const auto &entry = this->header.find(key);
    return entry != this->header.end() ? entry->second : "";
  }

  std::string getBody() const { return this->body.str(); }
};

class CustomTmpFile {
public:
  char *filename;
  int fd = -1;

  CustomTmpFile(const std::string &ext) {
    std::string tpl = "XXXXXX" + ext;
    this->filename = new char[6 + ext.length() + 1];
    std::strcpy(this->filename, tpl.c_str());
    this->fd = mkstemps(this->filename, ext.length());
  }

  bool isOpen() { return fd != -1; }

  void write(const std::string &data) {
    std::ofstream ofs(this->filename);
    ofs << data;
    ofs.close();
  }

  ~CustomTmpFile() {
    if (fd != -1) {
      close(fd);
      unlink(this->filename);
    }

    delete[] this->filename;
  }
};
