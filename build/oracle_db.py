'''
'''

import oracledb

def connect_to_oracle(user, password, dsn_name):
	"""Oracle DBに接続"""
	try:
		#connection = oracledb.connect(user, password, dsn_name) => error
		connection = oracledb.connect(user=user, password=password, dsn=dsn_name)
		return connection
	except oracledb.Error as error:
		print(f"Oracle接続エラー: {error}")
		return None

def fetch_data(connection, query):
	"""データを取得"""
	try:
		cursor = connection.cursor()
		cursor.execute(query)
		
		# カラム名を取得
		columns = [desc[0] for desc in cursor.description]
		
		# データを取得
		rows = cursor.fetchall()
		
		# 辞書形式で返す
		result = []
		for row in rows:
			result.append(dict(zip(columns, row)))
		
		cursor.close()
		return result
	
	except oracledb.Error as error:
		print(f"クエリ実行エラー: {error}")
		return []

def close_connection(connection):
	"""接続を閉じる"""
	if connection:
		connection.close()

# 便利な関数
def get_oracle_data(username, password, dsn, query):
	"""一連の処理をまとめた関数"""
	conn = connect_to_oracle(username, password, dsn)
	if conn:
		data = fetch_data(conn, query)
		close_connection(conn)
		return data
	return []