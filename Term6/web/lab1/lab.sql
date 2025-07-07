DROP DATABASE orders;
CREATE DATABASE orders;


\set ON_ERROR_STOP on

\c orders

CREATE TABLE employes (
	id SERIAL PRIMARY KEY,
	name VARCHAR(64) NOT NULL
);

CREATE TABLE items (
	id SERIAL PRIMARY KEY,
	name VARCHAR(64) NOT NULL
);

CREATE TABLE clients (
	id SERIAL PRIMARY KEY,
	name VARCHAR(64) NOT NULL
);

CREATE TABLE orderers (
	id SERIAL PRIMARY KEY,
	name VARCHAR(64) NOT NULL
);

CREATE TABLE orders (
	id SERIAL PRIMARY KEY,
	employee_id INT REFERENCES employes(id) ON DELETE RESTRICT,
	price INT CHECK (price >= 0),
	item_id INT REFERENCES items(id) ON DELETE RESTRICT,
	client_id INT REFERENCES clients(id) ON DELETE RESTRICT,
	orderer_id INT REFERENCES orderers(id) ON DELETE RESTRICT
);

INSERT INTO employes (name) VALUES
('Employe 1'),
('Employe 2'),
('Best employe');

INSERT INTO items (name) VALUES
('Good stuff'),
('Nice stuff'),
('Bad stuff');

INSERT INTO clients (name) VALUES
('One-day firm'),
('Better future inc.');

INSERT INTO orderers (name) VALUES
('James Wilson'),
('Gregory House'),
('Liza Cuddy');

INSERT INTO orders (employee_id, price, item_id, client_id, orderer_id) VALUES
((SELECT id FROM employes WHERE name = 'Best employe'), 1200, (SELECT id FROM items WHERE name = 'Good stuff'), (SELECT id FROM clients WHERE name = 'Better future inc.'), (SELECT id FROM orderers WHERE name = 'Liza Cuddy'));

SELECT * FROM items;

UPDATE orderers SET name = 'House M.D.' WHERE name = 'Gregory House';

DELETE FROM items WHERE name = 'Bad stuff';

SELECT orders.id,employes.name,orders.price,items.name,clients.name,orderers.name
FROM orders
LEFT JOIN employes ON employee_id = employes.id
LEFT JOIN items ON item_id = items.id
LEFT JOIN clients ON client_id = clients.id
LEFT JOIN orderers ON orderer_id = orderers.id
ORDER BY orders.id;

