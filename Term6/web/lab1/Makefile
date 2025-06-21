DB_USER ?= postgres
HOST ?= localhost

init:
	psql -U $(DB_USER) -f lab.sql	

graph:
	java -jar schemaspy-6.2.4.jar -t pgsql -db orders -host $(HOST) -u $(DB_USER) -o ./schemaspy -dp postgresql-42.7.7.jar -s public -noads

dump:
	pg_dump -U $(DB_USER) orders > pg_dump

