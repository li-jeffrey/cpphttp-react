#pragma once

#include "spdlog/fmt/ostr.h"
#include "json/json.h"
#include <shared_mutex>
#include <unordered_map>

namespace Products {

typedef struct Product Product;
struct Product {
  int Id;
  std::string Name;
  std::string Description;

  friend std::ostream &operator<<(std::ostream &, const Product &);
  bool operator==(const Product &p) const;
  bool operator!=(const Product &p) const;
};

void serialize(const Product &, Json::Value &);
void serialize(const std::vector<Product> &, Json::Value &);

class ProductManager {
public:
  virtual std::vector<Product> getAllProducts() const = 0;
  virtual std::optional<Product> getById(const int &id) const = 0;

  virtual int createProduct(const std::string &name,
                            const std::string &description) = 0;
  virtual bool updateProduct(const Product &) = 0;
  virtual void deleteProduct(const int &id) = 0;
};

class ProductManagerImpl : public ProductManager {
private:
  mutable std::shared_mutex mutex;
  std::unordered_map<int, Product> cache;
  int nextId = 0;

public:
  virtual std::vector<Product> getAllProducts() const override;
  virtual std::optional<Product> getById(const int &id) const override;

  virtual int createProduct(const std::string &name,
                            const std::string &description) override;
  virtual bool updateProduct(const Product &pdt) override;
  virtual void deleteProduct(const int &id) override;
};

} // namespace Products
