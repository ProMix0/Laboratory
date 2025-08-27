package main

import (
	"fmt"
	"io"
	"net/http"
)

const search_form_html = `
<form method="GET" action="">
<p>ID:<input type="number" step="1" min="1" name="id">
<input type="SUBMIT" value="Search"></p></form>
<a href="/index">Show all</a>
`
const table_end = `
</tbody></table>
`

func make_table_header(w io.Writer, role *UserRole) {
	fmt.Fprintf(w, "%s", `
<table border="1"><tbody>
<tr><th><b>id</b></th><th><b>employee</b></th><th><b>price</b></th><th><b>item</b></th><th><b>client</b></th><th><b>orderer</b></th>`)
	if role != nil {
		fmt.Fprintf(w, "%s", `<th><b>Update</b></th>`)
		if *role >= ADMIN {
			fmt.Fprintf(w, "%s", `<th><b>Delete</b></th>`)
		}
	}
	fmt.Fprintf(w, "%s", `</tr>`)
}

func make_row(w io.Writer, id int, employee string, price int, item string, client string, orderer string, role *UserRole) {
	fmt.Fprintf(w, `<tr><td>%d</td><td>%s</td><td>%d</td><td>%s</td><td>%s</td><td>%s</td>`, id, employee, price, item, client, orderer)

	if role != nil {
		fmt.Fprintf(w, `<td><a href="update?id=%d">Update</a></td>`, id)
		if *role >= ADMIN {
			fmt.Fprintf(w, `<td><a href="delete?id=%d">Delete</a></td>`, id)
		}
	}
	fmt.Fprintf(w, `</tr>`)
}

func PrintIndex(w http.ResponseWriter, query string, orders []Order, session *SessionEntry) {
	fmt.Fprintf(w, "<h1>Just a db, bro</h1><div>")
	fmt.Fprintf(w, "<pre>%s</pre>", query)

	var role *UserRole = nil
	if session != nil {
		role = &session.User.Role
	}
	make_table_header(w, role)

	for _, order := range orders {
		make_row(w, order.Id, order.Employee, order.Price, order.Item, order.Client, order.Orderer, role)
	}

	fmt.Fprintf(w, "%s", table_end)
	fmt.Fprintf(w, search_form_html)

	fmt.Fprintf(w, `<br/>`)
	if session != nil {
		fmt.Fprintf(w, `<a href="/insert">Insert</a>`)
		fmt.Fprintf(w, `<div>Logged as %s</div>`, session.User.Username)
		fmt.Fprintf(w, `<a href="/logout">Log out</a>`)
	} else {
		fmt.Fprintf(w, `<a href="/login">Login</a>`)
	}
	fmt.Fprintf(w, "</div>")
}

func PrintInsert(w http.ResponseWriter, employes []IdName, items []IdName, clients []IdName, orderers []IdName) {
	fmt.Fprintf(w, "<html><form method=\"POST\" action=\"/insert\"><p>")

	fmt.Fprintf(w, "Employee<select name=\"employee_id\">")
	print_options(w, employes)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, "Price<input type=\"number\" step=\"1\" min=\"1\" name=\"price\"/>")

	fmt.Fprintf(w, "Item<select name=\"item_id\">")
	print_options(w, items)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, "Client<select name=\"client_id\">")
	print_options(w, clients)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, "Orderer<select name=\"orderer_id\">")
	print_options(w, orderers)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, "<input type=\"SUBMIT\" value=\"Add\"></p></form></html>")
}
func PrintUpdate(w http.ResponseWriter, id int, employes []IdName, price int, items []IdName, clients []IdName, orderers []IdName) {
	fmt.Fprintf(w, "<html><form method=\"POST\" action=\"/update\"><p>")

	fmt.Fprintf(w, "Employee<select name=\"employee_id\">")
	print_options(w, employes)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, "Price<input type=\"number\" step=\"1\" min=\"1\" name=\"price\" value=\"%d\"/>", price)

	fmt.Fprintf(w, "Item<select name=\"item_id\">")
	print_options(w, items)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, "Client<select name=\"client_id\">")
	print_options(w, clients)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, "Orderer<select name=\"orderer_id\">")
	print_options(w, orderers)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, "<input type=\"hidden\" name=\"id\" value=\"%d\"/>", id)
	fmt.Fprintf(w, "<input type=\"SUBMIT\" value=\"Update\"></p></form></html>")
}

func PrintLogin(w http.ResponseWriter, error *string) {
	fmt.Fprintf(w, "<html><form method=\"POST\" action=\"/login\"><p>")

	fmt.Fprintf(w, "Login<input name=\"username\"/>")
	fmt.Fprintf(w, "Password<input name=\"password\" type=\"password\"/>")

	fmt.Fprintf(w, "<input type=\"SUBMIT\" value=\"Login\"></p></form>")

	if error != nil {
		fmt.Fprintf(w, "Error: %s", *error)
	}

	fmt.Fprintf(w, "</html>")
}
func print_options(w io.Writer, table []IdName) {
	for _, tmp := range table {
		fmt.Fprintf(w, "<option value=\"%d\">%s</option>", tmp.Id, tmp.Name)
	}
}
