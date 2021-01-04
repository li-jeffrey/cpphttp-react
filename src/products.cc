#include "products.h"
#include "spdlog/spdlog.h"

namespace Products {

void serialize(const Product &product, Json::Value &value) {
  value["Id"] = product.Id;
  value["Name"] = product.Name;
  value["Description"] = product.Description;
}

void serialize(const std::vector<Product> &products, Json::Value &value) {
  for (const auto &p : products) {
    Json::Value obj;
    serialize(p, obj);
    value.append(obj);
  }
}

std::ostream &operator<<(std::ostream &os, const Product &p) {
  return os << "Product [Id=" << p.Id << ", Name='" << p.Name
            << "', Description='" << p.Description << "']";
}

bool Product::operator==(const Product &p) const {
  return this->Id == p.Id && this->Name == p.Name &&
         this->Description == p.Description;
}

bool Product::operator!=(const Product &p) const { return !(*this == p); }

std::vector<Product> ProductManagerImpl::getAllProducts() const {
  std::shared_lock lock(this->mutex);
  std::vector<Product> products(this->cache.size());
  int i = 0;
  for (const auto &p : this->cache) {
    products[i] = p.second;
    i++;
  }
  return products;
}

std::optional<Product> ProductManagerImpl::getById(const int &id) const {
  std::shared_lock lock(this->mutex);
  const auto &entry = this->cache.find(id);
  return entry != this->cache.end() ? std::optional(entry->second)
                                    : std::nullopt;
}

int ProductManagerImpl::createProduct(const std::string &name,
                                      const std::string &description) {
  std::unique_lock lock(this->mutex);
  int nextId = ++this->nextId;
  const auto pdt = Product{nextId, name, description};
  this->cache[nextId] = pdt;
  SPDLOG_INFO("Created product: {}", pdt);
  return nextId;
}

bool ProductManagerImpl::updateProduct(const Product &pdt) {
  std::unique_lock lock(this->mutex);
  const auto &entry = this->cache.find(pdt.Id);
  if (entry != this->cache.end()) {
    auto orig = entry->second;
    this->cache[pdt.Id] = pdt;
    SPDLOG_INFO("Updated product: {}->{}", orig, pdt);
    return true;
  } else {
    SPDLOG_INFO("Update failed - product not found: {}", pdt);
    return false;
  }
}

void ProductManagerImpl::deleteProduct(const int &id) {
  std::unique_lock lock(this->mutex);
  auto nh = this->cache.extract(id);
  if (nh) {
    SPDLOG_INFO("Removed product: {}", nh.mapped());
  }
}

} // namespace Products
