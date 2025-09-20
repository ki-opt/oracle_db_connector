# https://write-remember.com/archives/6770/

import oracledb

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

	def create_connection_pool(self,):
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

	def close_connection_pool(self):
		''' コネクションプールのクローズ
		'''
		self.pool.close()

	

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
	
