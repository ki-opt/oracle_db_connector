/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-09-08
 * 
 * @copyright Copyright (c) 2025
 * 
 * pybind11 to /usr/include/ (header only)
 * sudo apt install pybind11-dev
 * https://github.com/pybind/pybind11/tree/master
 * https://pybind11.readthedocs.io/en/stable/faq.html
 * 
 * inipp (configparserと同等の処理が可能)
 * headerをダウンロードし配置すればOK
 * https://github.com/mcmtroffaes/inipp/blob/master/inipp/inipp.h
 * https://qiita.com/apiss/items/b67a758a6a03ff57c3b5
 * 
 * cmake 
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>

#include "inipp.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

namespace py = pybind11;

class OracleDbConnector {
private:
	py::scoped_interpreter guard{}; // Python インタープリターを初期化

public:
	std::vector<std::map<std::string, std::string>> fetchDataAsString(
		const std::string& user,
		const std::string& password, 
		const std::string& dsn_name,
		const std::string& query
	) {
		
		try {
			py::module oracle_module = py::module::import("oracle_db");
			py::object result = oracle_module.attr("get_oracle_data")(
				user, password, dsn_name, query
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
	
	inipp::Ini<char> ini;
	std::ifstream is("../config.ini");
	ini.parse(is);
	std::string user = ini.sections["CONNECTION"]["USER"];
	std::string password = ini.sections["CONNECTION"]["PASSWORD"];
	std::string dsn_name = ini.sections["ENVIRONMENT"]["DSN_NAME"];

	// データベース接続情報
	std::string query = "select * from t_store_weekly_sales where store_cd = '676584' and jan_cd = '010668'";
	
	// データを取得
	auto data = db_manager.fetchDataAsString(user, password, dsn_name, query);
	
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