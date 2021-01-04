#include "products.h"
#include <gtest/gtest.h>

TEST(Products, ProductEquality) {
  Products::Product pdt{1, "Volvo", "SUV"};
  Products::Product pdt2{2, "Toyota", "Camry"};
  ASSERT_EQ(pdt, pdt);
  ASSERT_NE(pdt, pdt2);
}

TEST(Products, ProductOStream) {
  std::ostringstream os;
  Products::Product pdt{1, "Volvo", "SUV"};
  os << pdt;
  ASSERT_EQ(os.str(), "Product [Id=1, Name='Volvo', Description='SUV']");
}

TEST(Products, SerializeOneProduct) {
  Products::Product pdt{1, "Volvo", "SUV"};
  Json::Value root;
  Products::serialize(pdt, root);
  ASSERT_EQ(root["Id"].asInt(), 1);
  ASSERT_EQ(root["Name"].asString(), "Volvo");
  ASSERT_EQ(root["Description"].asString(), "SUV");
}

TEST(Products, SerializeManyProducts) {
  Products::Product pdt{1, "Volvo", "SUV"};
  Products::Product pdt2{2, "Toyota", "Camry"};
  std::vector<Products::Product> pdts{pdt, pdt2};
  Json::Value root;
  Products::serialize(pdts, root);
  ASSERT_TRUE(root.isArray());
  ASSERT_EQ(root.size(), (unsigned int)2);

  auto value1 = root[0];
  ASSERT_EQ(value1["Id"].asInt(), 1);
  ASSERT_EQ(value1["Name"].asString(), "Volvo");
  ASSERT_EQ(value1["Description"].asString(), "SUV");

  auto value2 = root[1];
  ASSERT_EQ(value2["Id"].asInt(), 2);
  ASSERT_EQ(value2["Name"].asString(), "Toyota");
  ASSERT_EQ(value2["Description"].asString(), "Camry");
}

TEST(Products, CreateAndGetProduct) {
  Products::ProductManagerImpl pm;
  int id1 = pm.createProduct("Volvo", "SUV");
  int id2 = pm.createProduct("Toyota", "Camry");

  ASSERT_NE(id1, id2);
  ASSERT_EQ(pm.getAllProducts().size(), (size_t)2);

  auto pdt1 = pm.getById(id1);
  ASSERT_TRUE((bool)pdt1);
  ASSERT_EQ(pdt1->Id, id1);
  ASSERT_EQ(pdt1->Name, "Volvo");
  ASSERT_EQ(pdt1->Description, "SUV");

  auto pdt2 = pm.getById(id2);
  ASSERT_TRUE((bool)pdt2);
  ASSERT_EQ(pdt2->Id, id2);
  ASSERT_EQ(pdt2->Name, "Toyota");
  ASSERT_EQ(pdt2->Description, "Camry");

  auto nonexistent_pdt = pm.getById(999);
  ASSERT_FALSE((bool)nonexistent_pdt);
}

TEST(Products, UpdateProduct) {
  Products::ProductManagerImpl pm;
  int id1 = pm.createProduct("Volvo", "SUV");

  ASSERT_TRUE(pm.updateProduct(Products::Product{id1, "Volvo", "Truck"}));
  auto pdt1 = pm.getById(id1);
  ASSERT_TRUE((bool)pdt1);
  ASSERT_EQ(pdt1->Id, id1);
  ASSERT_EQ(pdt1->Name, "Volvo");
  ASSERT_EQ(pdt1->Description, "Truck");

  ASSERT_FALSE(pm.updateProduct(Products::Product{999, "abc", "def"}));
}

TEST(Products, DeleteProduct) {
  Products::ProductManagerImpl pm;
  int id1 = pm.createProduct("Volvo", "SUV");
  pm.createProduct("Toyota", "Camry");

  pm.deleteProduct(id1);
  ASSERT_FALSE((bool)pm.getById(id1));
  ASSERT_EQ(pm.getAllProducts().size(), (size_t)1);
}
