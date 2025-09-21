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

namespace {
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
				// プロジェクトのルートを指定
				std::string project_root_path = "..";
				py::object sys = py::module::import("sys");
				sys.attr("path").attr("insert")(0, project_root_path);

				py::module oracle_db_with_connection_pool_module = py::module::import("src.py.oracle_db_with_connection_pool");
				py::object py_oracle_db_connector_object = 
					oracle_db_with_connection_pool_module.attr("PyOracleDbConnector");

				// PyOracleDbConnectorクラスの定義
				py::object py_oracle_db_connector = 
					py_oracle_db_connector_object(user, password, dsn_name);
				
				// sql生成
				py::list sqls, table_names;
				sqls.append(query); table_names.append("0");
				sqls.append(query); table_names.append("1");

				// 
				py::object results = py_oracle_db_connector
											.attr("run_queries_in_parallel")(
												sqls, table_names
											);
				
				// py::objectをdictにキャスト
				py::dict results_dict = results.cast<py::dict>();
				
				for (auto const &item : results_dict) {
					// key取得
					const std::string &key = item.first.cast<std::string>();

					for (const auto &ele_tuple : item.second) {
						const auto &row = ele_tuple.cast<py::tuple>();
						const std::string &week_id = row[0].cast<std::string>();
						const std::string &store_cd = row[1].cast<std::string>();
						const std::string &jan_cd = row[2].cast<std::string>();
						const double sales_qty = row[3].cast<double>();
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
}

int main() {	
	inipp::Ini<char> ini;
	std::ifstream is("../config.ini");
	ini.parse(is);
	std::string user = ini.sections["CONNECTION"]["USER"];
	std::string password = ini.sections["CONNECTION"]["PASSWORD"];
	std::string dsn_name = ini.sections["ENVIRONMENT"]["DSN_NAME"];

	// sql
	std::string query = "select * from t_store_weekly_sales where store_cd = '676584' and jan_cd = '010668'";
	
	// データを取得
	OracleDbConnector oracle_db_connector;
	oracle_db_connector.fetchDataAsString(user, password, dsn_name, query);
	
	return 0;
}