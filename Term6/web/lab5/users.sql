\c orders

DROP TABLE users;
CREATE TYPE user_role AS enum ('user', 'admin');

\set ON_ERROR_STOP on

CREATE TABLE users (
	id SERIAL PRIMARY KEY,
	username VARCHAR(64) NOT NULL,
	password VARCHAR(64) NOT NULL,
	role user_role
);

INSERT INTO users (username, password, role) VALUES
('Student', 'password', 'user'),
('admin', 'admin', 'admin');

