#include "net.h"
#include "net_testutils.h"
#include "products.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class MockProductManager : public Products::ProductManager {
public:
  MOCK_METHOD(std::vector<Products::Product>, getAllProducts, (),
              (const, override));
  MOCK_METHOD(std::optional<Products::Product>, getById, (const int &id),
              (const, override));
  MOCK_METHOD(int, createProduct,
              (const std::string &name, const std::string &description),
              (override));
  MOCK_METHOD(bool, updateProduct, (const Products::Product &pdt), (override));
  MOCK_METHOD(void, deleteProduct, (const int &id), (override));
};

TEST(Net, FsHandlerFileNotFound) {
  using namespace testing;
  StrictMock<MockRequest> req;
  EXPECT_CALL(req, getUrl()).WillRepeatedly(Return("/abc.txt"));
  MockResponse res;

  Net::FsHandler<MockResponse, MockRequest> handler;
  handler.handleRequest(&res, &req);
  ASSERT_EQ(res.getStatus(), "404 Not Found");
  ASSERT_EQ(res.getBody(), "Not Found");
}

TEST(Net, FsHandlerReadFile) {
  using namespace testing;
  CustomTmpFile tmpf(".html");
  ASSERT_TRUE(tmpf.isOpen());

  tmpf.write("Hello world!");
  std::filesystem::path tmpPath(tmpf.filename);

  StrictMock<MockRequest> req;
  EXPECT_CALL(req, getUrl())
      .WillRepeatedly(Return("/" + tmpPath.filename().string()));

  MockResponse res;
  Net::FsHandler<MockResponse, MockRequest> handler;
  handler.handleRequest(&res, &req);
  ASSERT_EQ(res.getStatus(), "200 OK");
  ASSERT_EQ(res.getHeaderValue("Content-Type"), "text/html");
  ASSERT_EQ(res.getBody(), "Hello world!");
}

TEST(Net, ProductCtrlGetAll) {
  using namespace testing;
  std::vector<Products::Product> pdts{Products::Product{1, "Volvo", "SUV"}};
  StrictMock<MockProductManager> pm;
  EXPECT_CALL(pm, getAllProducts()).WillRepeatedly(Return(pdts));

  StrictMock<MockRequest> req;
  EXPECT_CALL(req, getUrl()).WillRepeatedly(Return("/api/v1/products/all"));
  EXPECT_CALL(req, getParameter(0)).WillRepeatedly(Return("all"));
  EXPECT_CALL(req, getMethod()).WillRepeatedly(Return("get"));

  MockResponse res;
  Net::ProductController<MockResponse, MockRequest> pdtctrl(&pm);
  pdtctrl.handleRequest(&res, &req);

  ASSERT_EQ(res.getStatus(), "200 OK");
  ASSERT_EQ(res.getHeaderValue("Content-Type"), "application/json");
  ASSERT_FALSE(res.getBody().empty());
}

TEST(Net, ProductCtrlUpdatePdt) {
  using namespace testing;
  StrictMock<MockProductManager> pm;
  EXPECT_CALL(pm, updateProduct(Products::Product{1, "Volvo", "Truck"}))
      .Times(1);

  StrictMock<MockRequest> req;
  EXPECT_CALL(req, getUrl()).WillRepeatedly(Return("/api/v1/products/update"));
  EXPECT_CALL(req, getParameter(0)).WillRepeatedly(Return("update"));
  EXPECT_CALL(req, getMethod()).WillRepeatedly(Return("post"));

  MockResponse res;
  res.postData = std::vector<std::string>{
      "{ \"Id\": 1, \"Name\": ", "\"Volvo\", \"Description\": \"Truck\" }"};
  Net::ProductController<MockResponse, MockRequest> pdtctrl(&pm);
  pdtctrl.handleRequest(&res, &req);
  ASSERT_EQ(res.getStatus(), "204 No Content");
}

TEST(Net, ProductCtrlUpdatePdt_InvalidJson) {
  using namespace testing;
  StrictMock<MockProductManager> pm;
  StrictMock<MockRequest> req;
  EXPECT_CALL(req, getUrl()).WillRepeatedly(Return("/api/v1/products/update"));
  EXPECT_CALL(req, getParameter(0)).WillRepeatedly(Return("update"));
  EXPECT_CALL(req, getMethod()).WillRepeatedly(Return("post"));

  MockResponse res;
  res.postData = std::vector<std::string>{"{ \"Id\": 1, "};
  Net::ProductController<MockResponse, MockRequest> pdtctrl(&pm);
  pdtctrl.handleRequest(&res, &req);
  ASSERT_EQ(res.getStatus(), "400 Bad Request");
  ASSERT_FALSE(res.getBody().empty());
}

TEST(Net, ProductCtrlDeletePdt) {
  using namespace testing;
  StrictMock<MockProductManager> pm;
  EXPECT_CALL(pm, deleteProduct(2)).Times(1);

  StrictMock<MockRequest> req;
  EXPECT_CALL(req, getUrl())
      .WillRepeatedly(Return("/api/v1/products/delete?id=2"));
  EXPECT_CALL(req, getParameter(0)).WillRepeatedly(Return("delete"));
  EXPECT_CALL(req, getMethod()).WillRepeatedly(Return("delete"));
  EXPECT_CALL(req, getQuery(std::string_view("id")))
      .WillRepeatedly(Return("2"));

  MockResponse res;
  Net::ProductController<MockResponse, MockRequest> pdtctrl(&pm);
  pdtctrl.handleRequest(&res, &req);
  ASSERT_EQ(res.getStatus(), "204 No Content");
}

TEST(Net, ProductCtrlUnknownRequest) {
  using namespace testing;
  StrictMock<MockProductManager> pm;

  StrictMock<MockRequest> req;
  EXPECT_CALL(req, getUrl()).WillRepeatedly(Return("/api/v1/products/abc"));
  EXPECT_CALL(req, getParameter(0)).WillRepeatedly(Return("abc"));
  EXPECT_CALL(req, getMethod()).WillRepeatedly(Return("get"));

  MockResponse res;
  Net::ProductController<MockResponse, MockRequest> pdtctrl(&pm);
  pdtctrl.handleRequest(&res, &req);

  ASSERT_EQ(res.getStatus(), "404 Not Found");
}
