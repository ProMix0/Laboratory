package main

import (
	"database/sql"
	"fmt"
	_ "github.com/lib/pq"
	"log"
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
	Orderer  string
}
type OrderRaw struct {
	Id         int
	EmployeeId int
	Price      int
	ItemId     int
	ClientId   int
	OrdererId  int
}

const sql_select = `
SELECT orders.id,employes.name,orders.price,items.name,clients.name,orderers.name
FROM orders
LEFT JOIN employes ON employee_id = employes.id
LEFT JOIN items ON item_id = items.id
LEFT JOIN clients ON client_id = clients.id
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
		err := rows.Scan(&order.Id, &order.Employee, &order.Price, &order.Item, &order.Client, &order.Orderer)
		if err != nil {
			fmt.Println(err)
			continue
		}

		result = append(result, order)

	}

	return cur_select, result, nil
}

func GetOrderRaw(id int) (OrderRaw, error) {
	var result OrderRaw
	cur_select := `
SELECT id,employee_id,price,item_id,client_id,orderer_id
FROM orders
WHERE id = %d
`
	err := db.QueryRow(fmt.Sprintf(cur_select, id)).Scan(&result.Id, &result.EmployeeId, &result.Price, &result.ItemId, &result.ClientId, &result.OrdererId)
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

func Delete(id int) error {
	_, err := db.Exec("DELETE FROM orders WHERE id = $1", id)
	return err
}

const sql_insert_tmpl = `
INSERT INTO orders (employee_id, price, item_id, client_id, orderer_id) VALUES
($1, $2, $3, $4, $5);
`

func InsertWithIds(employee int, price int, item int, client int, orderer int) error {
	_, err := db.Exec(sql_insert_tmpl, employee, price, item, client, orderer)
	return err
}

const sql_update_tmpl = `
UPDATE orders
SET employee_id=$1, price=$2, item_id=$3, client_id=$4, orderer_id=$5
WHERE id = $6
`

func UpdateWithIds(id int, employee int, price int, item int, client int, orderer int) error {
	_, err := db.Exec(sql_update_tmpl, employee, price, item, client, orderer, id)
	return err
}
