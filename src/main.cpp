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
 * vscode intelligence: cmake tools
 * 
 * cmake -DCMAKE_BUILD_TYPE=Debug ..
 * make run_cpp
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>

#include "src/external/inipp.h"

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
	//std::vector<std::map<std::string, std::string>> fetchDataAsString(
	int fetchDataAsString(
		const std::string& user,
		const std::string& password, 
		const std::string& dsn_name,
		const std::string& query
	) {
		
		try {
			py::module oracle_db_with_connection_pool = py::module::import("oracle_db_with_connection_pool");
			/*py::object result = oracle_db_with_connection_pool.attr("get_oracle_data")(
				user, password, dsn_name, query
			);*/
			
			py::object py_oracle_db_connector_class = 
				oracle_db_with_connection_pool.attr("PyOracleDbConnector");

			py::object py_oracle_db_connector = py_oracle_db_connector_class(
				user, password, dsn_name, 1, 3
			);
			
			py::list sqls;
			sqls.append(query);
			sqls.append(query);

			py::object result = py_oracle_db_connector.attr("run_queries_in_parallel")(sqls);
			//std::string result_str = py::str(result);
			//std::cout << result << std::endl;

			for (const auto &item : result) {
				for (const auto &ele : item) {
					const auto &row = ele.cast<py::tuple>();
					std::cout << std::stoi(row[0].cast<std::string>()) << std::endl;
				}
			}

			return 0;
			
		} catch (py::error_already_set &e) {
			std::cerr << "Python実行エラー: " << e.what() << std::endl;
			//return {};

			return 1;
		}
	}
};

int main() {
	OracleDbConnector oracle_db_connector;
	
	inipp::Ini<char> ini;
	std::ifstream is("../config.ini");
	ini.parse(is);
	std::string user = ini.sections["CONNECTION"]["USER"];
	std::string password = ini.sections["CONNECTION"]["PASSWORD"];
	std::string dsn_name = ini.sections["ENVIRONMENT"]["DSN_NAME"];

	// データベース接続情報
	std::string query = "select * from t_store_weekly_sales where store_cd = '676584' and jan_cd = '010668'";
	
	// データを取得
	oracle_db_connector.fetchDataAsString(user, password, dsn_name, query);

	
	return 0;
}