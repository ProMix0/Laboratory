package main

import (
	"database/sql"
	"errors"
	"fmt"
	_ "github.com/lib/pq"
	"log"
	"strings"
)

var db *sql.DB

func InitModel() error {
	var err error
	db, err = sql.Open("postgres", "user=postgres dbname=orders sslmode=disable")
	if err != nil {
		log.Fatal(err)
		return err
	}
	return nil
}

func DeinitModel() {
	db.Close()
}

type Order struct {
	Id       int
	Employee string
	Price    int
	Item     string
	Client   string
	ClientDepartment string
	Orderer  string
}
type OrderRaw struct {
	Id         int
	EmployeeId int
	Price      int
	ItemId     int
	ClientId   int
	ClientDepartmentId int
	OrdererId  int
}

const sql_select = `
SELECT orders.id,employes.name,orders.price,items.name,clients.name,client_departments.name,orderers.name
FROM orders
LEFT JOIN employes ON employee_id = employes.id
LEFT JOIN items ON item_id = items.id
LEFT JOIN clients ON client_id = clients.id
LEFT JOIN client_departments ON client_department_id = client_departments.id
LEFT JOIN orderers ON orderer_id = orderers.id
`

func GetOrders(id int) (string, []Order, error) {
	result := make([]Order, 0, 8)
	cur_select := sql_select
	if id >= 0 {
		cur_select += fmt.Sprintf("WHERE orders.id = %d", id)
	} else {
		cur_select += fmt.Sprintf("ORDER BY orders.id")
	}

	rows, err := db.Query(cur_select)
	if err != nil {
		return "", nil, err
	}
	defer rows.Close()

	for rows.Next() {
		var order Order
		err := rows.Scan(&order.Id, &order.Employee, &order.Price, &order.Item, &order.Client, &order.ClientDepartment, &order.Orderer)
		if err != nil {
			fmt.Println(err)
			continue
		}

		result = append(result, order)

	}

	return cur_select, result, nil
}

const aggregated_data_query = `
SELECT clients.name, SUM(orders.price) AS sum
FROM clients
LEFT JOIN orders ON client_id = clients.id
GROUP BY clients.name
`

type ClientAndSum struct {
	Client string
	Sum    int
}

func GetAggregatedData() (string, []ClientAndSum, error) {
	result := make([]ClientAndSum, 0, 8)

	rows, err := db.Query(aggregated_data_query)
	if err != nil {
		return "", nil, err
	}
	defer rows.Close()

	for rows.Next() {
		var result_element ClientAndSum
		err := rows.Scan(&result_element.Client, &result_element.Sum)
		if err != nil {
			fmt.Println(err)
			continue
		}

		result = append(result, result_element)

	}

	return aggregated_data_query, result, nil
}

func GetOrderRaw(id int) (OrderRaw, error) {
	var result OrderRaw
	cur_select := `
SELECT id,employee_id,price,item_id,client_id,client_department_id,orderer_id
FROM orders
WHERE id = %d
`
	err := db.QueryRow(fmt.Sprintf(cur_select, id)).Scan(&result.Id, &result.EmployeeId, &result.Price, &result.ItemId, &result.ClientId, &result.ClientDepartmentId, &result.OrdererId)
	if err != nil {
		return result, err
	}

	return result, nil
}

type IdName struct {
	Id   int
	Name string
}

func GetIdName(table string) []IdName {
	result := make([]IdName, 0, 8)
	rows, err := db.Query(fmt.Sprintf("SELECT id,name FROM %s", table))
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	for rows.Next() {
		var tmp IdName
		err := rows.Scan(&tmp.Id, &tmp.Name)
		if err != nil {
			fmt.Println(err)
			continue
		}

		result = append(result, tmp)
	}
	return result
}

func GetClientDepartments(client_id int) []IdName {
	result := make([]IdName, 0, 8)
	rows, err := db.Query(fmt.Sprintf(`
	SELECT id,name FROM client_departments
	INNER JOIN client_departments_relations ON department_id = id
	WHERE client_id = %d
	`, client_id))
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	for rows.Next() {
		var tmp IdName
		err := rows.Scan(&tmp.Id, &tmp.Name)
		if err != nil {
			fmt.Println(err)
			continue
		}

		result = append(result, tmp)
	}
	return result
}

func Delete(id int) error {
	_, err := db.Exec("DELETE FROM orders WHERE id = $1", id)
	return err
}

const sql_insert_tmpl = `
INSERT INTO orders (employee_id, price, item_id, client_id, client_department_id, orderer_id) VALUES
($1, $2, $3, $4, $5, $6);
`

func InsertWithIds(employee int, price int, item int, client int, client_department int, orderer int) error {
	_, err := db.Exec(sql_insert_tmpl, employee, price, item, client, client_department, orderer)
	return err
}

const sql_update_tmpl = `
UPDATE orders
SET employee_id=$1, price=$2, item_id=$3, client_id=$4, client_department_id=$5, orderer_id=$6
WHERE id = $7
`

func UpdateWithIds(id int, employee int, price int, item int, client int, client_department int, orderer int) error {
	_, err := db.Exec(sql_update_tmpl, employee, price, item, client, client_department, orderer, id)
	return err
}

type UserRole int

const (
	USER  UserRole = 0
	ADMIN UserRole = 1
)

type UserRecord struct {
	Username string
	Password string
	Role     UserRole
}

const sql_select_user = `
SELECT username,password,role
FROM users
WHERE username = '%s'
`

func GetUserRecord(username string) (UserRecord, error) {
	var result UserRecord
	var role_str string
	err := db.QueryRow(fmt.Sprintf(sql_select_user, username)).Scan(&result.Username, &result.Password, &role_str)
	if err != nil {
		return result, err
	}
	if strings.Compare(role_str, "user") == 0 {
		result.Role = USER
	} else if strings.Compare(role_str, "admin") == 0 {
		result.Role = ADMIN
	} else {
		return result, errors.New("Invalid user role in DB")
	}
	return result, nil
}
