

import oracledb

DSN = """
(DESCRIPTION =
	(ADDRESS = (PROTOCOL = TCP)(HOST = localhost)(PORT = 1521))
	(CONNECT_DATA =
		(SERVER = DEDICATED)
		(SERVICE_NAME = orclpdb)
	)
)
"""

USER = 'RTKDM708'
PASSWORD = 'rtkdm708'
DNS_NAME = 'p'

class DbConnector():
	def __init__(self):
		'''
		'''
		#self.env = 'p'
		# thickモード有効化
		#oracledb.init_oracle_client()
	
	def create_connection_pool(self,):
		''' コネクションプールの作成
		'''
		# dnsnameの取得
		#dsn_name = 'p'

		# コネクションプールの作成
		self.pool = \
			oracledb.create_pool(
				user=USER,
				password=PASSWORD,
				dsn=DSN,
				min=0,
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