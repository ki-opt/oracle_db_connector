# https://write-remember.com/archives/6770/

import oracledb
from concurrent.futures import ThreadPoolExecutor, as_completed

class PyOracleDbConnector():
	def __init__(self, user, password, dsn_name):
		'''
		'''
		self.user = user
		self.password = password
		self.dsn_name = dsn_name

		# thickモード有効化
		#oracledb.init_oracle_client()
	
	def run_queries_in_parallel(self, sqls, table_names):
		''' マルチスレッドでデータ取得
		'''
		def fetch_data(sql, table_name):
			try:
				with self.pool.acquire() as connection:
					with connection.cursor() as cursor:
						cursor.execute(sql)
						result = cursor.fetchall()
						return result, table_name, None
					
			except Exception as e:
				return [], table_name, e
		
		try:
			# コネクションプール作成
			self.__create_connection_pool()

			with ThreadPoolExecutor(max_workers=2) as executor:
				futures = [executor.submit(fetch_data, sql, table_name) for sql, table_name in zip(sqls, table_names)]

				results = {}
				for future in as_completed(futures):
					result, table_name, error_code = future.result()
					results[table_name] = result

					# error発生時の処理
					if error_code is None:
						error_code = ""
					results['error_code'] = error_code

			return results
		
		except Exception as e:
			return {'error_code':e}
	
		finally:
			# コネクションプール終了
			self.__close_connection_pool()
	
	def __create_connection_pool(self,):
		''' コネクションプールの作成
		'''
		# コネクションプールの作成
		self.pool = \
			oracledb.create_pool(
				user=self.user,
				password=self.password,
				dsn=self.dsn_name,
				min=1,
				max=3,
			)

	def __close_connection_pool(self):
		''' コネクションプールのクローズ
		'''
		self.pool.close()
