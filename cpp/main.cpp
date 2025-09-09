/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-09-08
 * 
 * @copyright Copyright (c) 2025
 * 
 * pybind11 to /usr/include/ (only header)
 * https://github.com/pybind/pybind11/tree/master
 * https://pybind11.readthedocs.io/en/stable/faq.html
 * sudo apt install pybind11-dev
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>

namespace py = pybind11;

class OracleDbConnector {
private:
	py::scoped_interpreter guard{}; // Python インタープリターを初期化

public:
	std::vector<std::map<std::string, py::object>> fetchData(
		const std::string& username,
		const std::string& password, 
		const std::string& dsn,
		const std::string& query
	) {
		
		try {
			// Pythonモジュールをインポート
			py::module oracle_module = py::module::import("oracle_db");
			
			// Python関数を呼び出し
			py::object result = oracle_module.attr("get_oracle_data")(
					username, password, dsn, query
			);
			
			// Pythonの結果をC++の型に変換
			return result.cast<std::vector<std::map<std::string, py::object>>>();
			
		} catch (py::error_already_set &e) {
			std::cerr << "Python実行エラー: " << e.what() << std::endl;
			return {};
		}
	}
	
	// より型安全な版（文字列として取得）
	std::vector<std::map<std::string, std::string>> fetchDataAsString(
		const std::string& username,
		const std::string& password, 
		const std::string& dsn,
		const std::string& query
	) {
		
		try {
			py::module oracle_module = py::module::import("oracle_db");
			py::object result = oracle_module.attr("get_oracle_data")(
				"", "", "", query
			);
			
			std::vector<std::map<std::string, std::string>> cpp_result;
			
			for (auto item : result) {
				std::map<std::string, std::string> row;
				py::dict py_row = item.cast<py::dict>();
				
				for (auto pair : py_row) {
					std::string key = pair.first.cast<std::string>();
					std::string value = py::str(pair.second).cast<std::string>();
					row[key] = value;
				}
				cpp_result.push_back(row);
			}
			
			return cpp_result;
			
		} catch (py::error_already_set &e) {
			std::cerr << "Python実行エラー: " << e.what() << std::endl;
			return {};
		}
	}
};

int main() {
	OracleDbConnector db_manager;
	
	// データベース接続情報
	std::string username = "your_username";
	std::string password = "your_password";
	std::string dsn = "localhost:1521/XE"; // または適切なDSN
	std::string query = "select * from t_store_weekly_sales where store_cd = '' and jan_cd = ''";
	
	// データを取得
	auto data = db_manager.fetchDataAsString(username, password, dsn, query);
	
	// 結果を表示
	std::cout << "取得したデータ:" << std::endl;
	for (const auto& row : data) {
		for (const auto& pair : row) {
			std::cout << pair.first << ": " << pair.second << " ";
		}
		std::cout << std::endl;
	}
	
	return 0;
}