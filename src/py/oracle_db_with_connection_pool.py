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
	
	def test(self):
		'''
		'''
		print('test')

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

	def run_queries_in_parallel(self, sqls, table_names):
		'''
		'''
		def fetch_data(sql, table_name):
			try:
				with self.pool.acquire() as connection:
					with connection.cursor() as cursor:
						cursor.execute(sql)
						result = cursor.fetchall()
						return result, table_name, None
			except Exception as e:
				return [], None, e
		
		try:
			self.__create_connection_pool()

			with ThreadPoolExecutor(max_workers=2) as executor:
				futures = [executor.submit(fetch_data, sql, table_name) for sql, table_name in zip(sqls, table_names)]

				results = {}
				for future in as_completed(futures):
					result, table_name, error_code = future.result()
					results[table_name] = result
			
			return results
		
		except Exception as e:
			return e
	
		finally:
			self.__close_connection_pool()

	def test_sql(self):
		'''
		'''
		with self.pool.acquire() as connection:
			with connection.cursor() as cursor:
				cursor.execute(
					"""
						select /*+ INDEX(T_STORE_WEEKLY_SALES IDX_LOCAL) */ *
						from t_store_weekly_sales
						where 1 = 1
						  and store_cd = '676584'
						  and jan_cd = '010668'
					"""
				)
				result = cursor.fetchall()
				print(result)
	
