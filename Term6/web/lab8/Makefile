DB_USER ?= postgres
HOST ?= localhost

init:
	psql -U $(DB_USER) -f client_departments.sql

