#include "products.h"
#include "spdlog/spdlog.h"

namespace Products {

std::ostream &operator<<(std::ostream &os, const Product &p) {
  return os << "Product [Id=" << p.Id << ", Name='" << p.Name
            << "', Description='" << p.Description << "']";
}

Json::Value &operator<<(Json::Value &value, const Product &product) {
  value["Id"] = product.Id;
  value["Name"] = product.Name;
  value["Description"] = product.Description;
  return value;
}

Json::Value &operator<<(Json::Value &value,
                        const std::vector<Product> &products) {
  for (const auto &p : products) {
    Json::Value obj;
    obj << p;
    value.append(obj);
  }

  return value;
}

Json::Value &operator>>(Json::Value &json, Product &p) {
  p.Id = json["Id"].asInt();
  p.Name = json["Name"].asString();
  p.Description = json["Description"].asString();
  return json;
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

int ProductManagerImpl::createProduct(const Product &pdt) {
  std::unique_lock lock(this->mutex);
  int nextId = ++this->nextId;
  auto cpy = pdt;
  cpy.Id = nextId;
  this->cache[nextId] = cpy;
  SPDLOG_INFO("Created product: {}", cpy);
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
