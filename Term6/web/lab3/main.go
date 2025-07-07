package main

import (
	"database/sql"
	"fmt"
	_ "github.com/lib/pq"
	"io"
	"log"
	"net/http"
	"strings"
)

var db *sql.DB

const search_form_html = `
<form method="GET" action="">
<p>ID:<input type="number" step="1" min="1" name="id">
<input type="SUBMIT" value="Search"></p></form>
<a href="/index">Show all</a>
<a href="/insert">Insert</a>
`
const sql_select = `
SELECT orders.id,employes.name,orders.price,items.name,clients.name,orderers.name
FROM orders
LEFT JOIN employes ON employee_id = employes.id
LEFT JOIN items ON item_id = items.id
LEFT JOIN clients ON client_id = clients.id
LEFT JOIN orderers ON orderer_id = orderers.id
ORDER BY orders.id
`
const table_start = `
<table border="1"><tbody>
<tr><th><b>id</b></th><th><b>employee</b></th><th><b>price</b></th><th><b>item</b></th><th><b>client</b></th><th><b>orderer</b></th><th><b>Update</b></th><th><b>Delete</b></th></tr>
`
const table_end = `
</tbody></table>
`
const table_row_fmt = `
<tr><td>%d</td><td>%s</td><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td><a href="update?id=%d">Update</a></td><td><a href="delete?id=%d">Delete</a></td></tr>
`

func make_row(w io.Writer, id int, employee string, price int, item string, client string, orderer string) {
	fmt.Fprintf(w, table_row_fmt, id, employee, price, item, client, orderer, id, id)
}

func handler(w http.ResponseWriter, r *http.Request) {
	cur_select := sql_select
	request_id := r.URL.Query().Get("id")
	if request_id != "" {
		cur_select += fmt.Sprintf("\nWHERE orders.id = '%s'", request_id)
	}

	fmt.Fprintf(w, "<h1>Just a db, bro</h1><div>")
	fmt.Fprintf(w, "%s", cur_select)
	fmt.Fprintf(w, "%s", table_start)

	rows, err := db.Query(cur_select)
	if err != nil {
		http.Error(w, "Internal DB error", http.StatusInternalServerError)
		panic(err)
	}
	defer rows.Close()

	for rows.Next() {
		var id int
		var employee string
		var price int
		var item string
		var client string
		var orderer string
		err := rows.Scan(&id, &employee, &price, &item, &client, &orderer)
		if err != nil {
			fmt.Println(err)
			continue
		}

		make_row(w, id, employee, price, item, client, orderer)

	}

	fmt.Fprintf(w, "%s", table_end)
	fmt.Fprintf(w, search_form_html)
	fmt.Fprintf(w, "</div>")
}

const sql_insert_tmpl = `
INSERT INTO orders (employee_id, price, item_id, client_id, orderer_id) VALUES
($1, $2, $3, $4, $5);
`

func handler_insert(w http.ResponseWriter, r *http.Request) {
	if r.Method == http.MethodGet {
		fmt.Fprintf(w, "<html><form method=\"POST\" action=\"/insert\"><p>")

		fmt.Fprintf(w, "Employee<select name=\"employee_id\">")
		fmt.Fprintf(w, "%s", get_options("employes"))
		fmt.Fprintf(w, "</select>")

		fmt.Fprintf(w, "Price<input type=\"number\" step=\"1\" min=\"1\" name=\"price\"/>")

		fmt.Fprintf(w, "Item<select name=\"item_id\">")
		fmt.Fprintf(w, "%s", get_options("items"))
		fmt.Fprintf(w, "</select>")

		fmt.Fprintf(w, "Client<select name=\"client_id\">")
		fmt.Fprintf(w, "%s", get_options("clients"))
		fmt.Fprintf(w, "</select>")

		fmt.Fprintf(w, "Orderer<select name=\"orderer_id\">")
		fmt.Fprintf(w, "%s", get_options("orderers"))
		fmt.Fprintf(w, "</select>")

		fmt.Fprintf(w, "<input type=\"SUBMIT\" value=\"Add\"></p></form></html>")
	}
	if r.Method == http.MethodPost {

		_, err := db.Exec(sql_insert_tmpl, r.FormValue("employee_id"), r.FormValue("price"), r.FormValue("item_id"), r.FormValue("client_id"), r.FormValue("orderer_id"))
		if err != nil {
			http.Error(w, "Error while inserting", http.StatusBadRequest)
			return
		}
		http.Redirect(w, r, "/index", http.StatusSeeOther)
	}
}

const sql_update_tmpl = `
UPDATE orders
SET employee_id=$1, price=$2, item_id=$3, client_id=$4, orderer_id=$5
WHERE id = $6
`

func handler_update(w http.ResponseWriter, r *http.Request) {
	if r.Method == http.MethodGet {
		request_id := r.URL.Query().Get("id")
		if request_id == "" {
			http.Error(w, "Id not present", http.StatusBadRequest)
			return
		}
		var id int
		var employee_id int
		var price int
		var item_id int
		var client_id int
		var orderer_id int
		db.QueryRow(fmt.Sprintf("SELECT id,employee_id,price,item_id,client_id,orderer_id FROM orders WHERE id = %s", request_id)).Scan(&id, &employee_id, &price, &item_id, &client_id, &orderer_id)

		fmt.Fprintf(w, "<html><form method=\"POST\" action=\"/update\"><p>")

		fmt.Fprintf(w, "Employee<select name=\"employee_id\">")
		fmt.Fprintf(w, "%s", get_options_selected("employes", employee_id))
		fmt.Fprintf(w, "</select>")

		fmt.Fprintf(w, "Price<input type=\"number\" step=\"1\" min=\"1\" name=\"price\" value=\"%d\"/>", price)

		fmt.Fprintf(w, "Item<select name=\"item_id\">")
		fmt.Fprintf(w, "%s", get_options_selected("items", item_id))
		fmt.Fprintf(w, "</select>")

		fmt.Fprintf(w, "Client<select name=\"client_id\">")
		fmt.Fprintf(w, "%s", get_options_selected("clients", client_id))
		fmt.Fprintf(w, "</select>")

		fmt.Fprintf(w, "Orderer<select name=\"orderer_id\">")
		fmt.Fprintf(w, "%s", get_options_selected("orderers", orderer_id))
		fmt.Fprintf(w, "</select>")

		fmt.Fprintf(w, "<input type=\"hidden\" name=\"id\" value=\"%d\"/>", id)
		fmt.Fprintf(w, "<input type=\"SUBMIT\" value=\"Update\"></p></form></html>")
	}
	if r.Method == http.MethodPost {
		_, err := db.Exec(sql_update_tmpl, r.FormValue("employee_id"), r.FormValue("price"), r.FormValue("item_id"), r.FormValue("client_id"), r.FormValue("orderer_id"), r.FormValue("id"))
		if err != nil {
			http.Error(w, "Unknown error while updating", http.StatusBadRequest)
			return
		}
		http.Redirect(w, r, "/index", http.StatusSeeOther)
	}
}

func get_options(table string) string {
	var builder strings.Builder
	rows, err := db.Query(fmt.Sprintf("SELECT id,name FROM %s", table))
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	for rows.Next() {
		var id int
		var name string
		err := rows.Scan(&id, &name)
		if err != nil {
			fmt.Println(err)
			continue
		}

		builder.WriteString(fmt.Sprintf("<option value=\"%d\">%s</option>", id, name))
	}
	return builder.String()
}

func get_options_selected(table string, target_id int) string {
	var builder strings.Builder
	rows, err := db.Query(fmt.Sprintf("SELECT id,name FROM %s", table))
	if err != nil {
		panic(err)
	}
	defer rows.Close()

	for rows.Next() {
		var id int
		var name string
		err := rows.Scan(&id, &name)
		if err != nil {
			fmt.Println(err)
			continue
		}

		selected := ""
		if id == target_id {
			selected = "selected"
		}

		builder.WriteString(fmt.Sprintf("<option value=\"%d\" %s>%s</option>", id, selected, name))
	}
	return builder.String()
}

func handler_delete(w http.ResponseWriter, r *http.Request) {
	request_id := r.URL.Query().Get("id")
	if request_id == "" {
		http.Error(w, "Id not present", http.StatusBadRequest)
		return
	}
	_, err := db.Exec("DELETE FROM orders WHERE id = $1", request_id)
	if err != nil {
		http.Error(w, "Invalid id", http.StatusBadRequest)
		return
	}
	http.Redirect(w, r, "/index", http.StatusSeeOther)
}

func main() {
	var err error
	db, err = sql.Open("postgres", "user=postgres dbname=orders sslmode=disable")
	if err != nil {
		log.Fatal(err)
		return
	}
	defer db.Close()
	http.HandleFunc("/index", handler)
	http.HandleFunc("/insert", handler_insert)
	http.HandleFunc("/update", handler_update)
	http.HandleFunc("/delete", handler_delete)
	fmt.Println("Start listening at http://localhost:8080/index")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
