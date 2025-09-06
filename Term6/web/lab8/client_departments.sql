\set ON_ERROR_STOP on

\c orders

CREATE TABLE IF NOT EXISTS client_departments (
	id SERIAL PRIMARY KEY,
	name VARCHAR(64) NOT NULL
);

DELETE FROM orders;
ALTER TABLE orders ADD COLUMN IF NOT EXISTS
	client_department_id INT REFERENCES client_departments(id) ON DELETE RESTRICT;

CREATE TABLE IF NOT EXISTS client_departments_relations (
	client_id INT REFERENCES clients(id),
	department_id INT REFERENCES client_departments(id),
	PRIMARY KEY (client_id, department_id)
);

TRUNCATE TABLE client_departments_relations;
DELETE FROM client_departments;
INSERT INTO client_departments (name) VALUES
('Department 1'),
('Department 2'),
('Better department 1'),
('Better department 2');

INSERT INTO client_departments_relations (client_id, department_id) VALUES
((SELECT id FROM clients WHERE name = 'One-day firm'), (SELECT id FROM client_departments WHERE name = 'Department 1')),
((SELECT id FROM clients WHERE name = 'One-day firm'), (SELECT id FROM client_departments WHERE name = 'Department 2')),
((SELECT id FROM clients WHERE name = 'Better future inc.'), (SELECT id FROM client_departments WHERE name = 'Better department 1')),
((SELECT id FROM clients WHERE name = 'Better future inc.'), (SELECT id FROM client_departments WHERE name = 'Better department 2'));

