#include <iostream>
#include <string>
#include <rocksdb/db.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class RocksDBJson {
public:
  RocksDB(const std::string& db_path) : db_path_(db_path) {}

  bool open_db() {
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, db_path_, &db_);

    if (status.ok()) {
      return true;
    } else {
      std::cerr << "Error opening database: " << status.ToString() << std::endl;
      return false;
    }
  }

bool store_json_data(const std::string& key, const json& data) {
  // Convert the JSON data to a string
  std::string value = data.dump();

  // Store the data in RocksDB
  rocksdb::Status put_status = db_->Put(rocksdb::WriteOptions(), key, value);

  if (put_status.ok()) {
    std::cout << "Data stored successfully" << std::endl;
    return true;
  } else {
    std::cerr << "Error storing data: " << put_status.ToString() << std::endl;
    return false;
  }
}

bool update_json_data(const std::string& key, const json& data) {
  // Check if the key exists in the database
  std::string value;
  rocksdb::Status get_status = db_->Get(rocksdb::ReadOptions(), key, &value);

  if (get_status.ok()) {
    // Convert the existing JSON data to a JSON object
    json existing_data = json::parse(value);

    // Merge the existing data with the new data
    existing_data.merge_patch(data);

    // Convert the merged data to a string
    std::string merged_value = existing_data.dump();

    // Update the data in RocksDB
    rocksdb::Status put_status = db_->Put(rocksdb::WriteOptions(), key, merged_value);

    if (put_status.ok()) {
      std::cout << "Data updated successfully" << std::endl;
      return true;
    } else {
      std::cerr << "Error updating data: " << put_status.ToString() << std::endl;
      return false;
    }
  } else {
    std::cerr << "Error getting data: " << get_status.ToString() << std::endl;
    return false;
  }
}

bool delete_json_data(const std::string& key) {
  // Delete the data from RocksDB
  rocksdb::Status delete_status = db_->Delete(rocksdb::WriteOptions(), key);

  if (delete_status.ok()) {
    std::cout << "Data deleted successfully" << std::endl;
    return true;
  } else {
    std::cerr << "Error deleting data: " << delete_status.ToString() << std::endl;
    return false;
  }
}

bool get_json_data(const std::string& key, json& data) {
  // Check if the key exists in the database
  std::string value;
  rocksdb::Status get_status = db_->Get(rocksdb::ReadOptions(), key, &value);

  if (get_status.ok()) {
    // Convert the JSON data to a JSON object
    data = json::parse(value);
    return true;
  } else {
    std::cerr << "Error getting data: " << get_status.ToString() << std::endl;
    return false;
  }
}  
private:
  std::string db_path_;
  rocksdb::DB* db_;
};

int main() {
  // Example usage:
  RocksDBJson db("db");
  if (db.open_db()) {
    // Database opened successfully

    // Store data
    json data = {{"name", "John"}, {"age", 30}};
    db.store_json_data("key1", data);

    // Update data
    json update_data = {{"age", 31}};
    db.update_json_data("key1", update_data);

    // Get data
    json get_data;
    db.get_json_data("key1", get_data);
    std::cout << "Name: " << get_data["name"] << ", Age: " << get_data["age"] << std::endl;

    // Delete data
    db.delete_json_data("key1");
  } else {
    // Error opening database
  }
  return 0;
}