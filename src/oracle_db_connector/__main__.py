
from src.db_connector import DbConnector

def main():
	db_connector = DbConnector()
	
	db_connector.create_connection_pool()
	db_connector.test_sql()
	db_connector.close_connection_pool()

if __name__ == '__main__':
	main()
	