package main

import (
	"fmt"
	"log"
	"net/http"
	"slices"
	"strconv"
	"strings"
)

func index_handler(w http.ResponseWriter, r *http.Request) {
	request_id := r.URL.Query().Get("id")
	id, err := strconv.Atoi(request_id)
	if err != nil {
		id = -1
	}
	query, orders, err := GetOrders(id)
	if err != nil {
		http.Error(w, "Internal DB error", http.StatusInternalServerError)
		panic(err)
	}

	var session_ptr *SessionEntry
	session, err := GetSession(r)
	if err == nil {
		session_ptr = &session
	} else {
		session_ptr = nil
	}

	PrintIndex(w, query, orders, session_ptr)
}

func insert_handler(w http.ResponseWriter, r *http.Request) {
	if !IsAuthorized(r, USER) {
		http.Error(w, "Unauthorized", http.StatusUnauthorized)
		return
	}
	if r.Method == http.MethodGet {
		employes := GetIdName("employes")
		items := GetIdName("items")
		clients := GetIdName("clients")
		orderers := GetIdName("orderers")
		PrintInsert(w, employes, items, clients, orderers)
	}
	if r.Method == http.MethodPost {
		var err error
		employee, err := strconv.Atoi(r.FormValue("employee_id"))
		if err != nil {
			http.Error(w, "Invalid employee id", http.StatusBadRequest)
			return
		}
		price, err := strconv.Atoi(r.FormValue("price"))
		if err != nil {
			http.Error(w, "Invalid price", http.StatusBadRequest)
			return
		}
		item, err := strconv.Atoi(r.FormValue("item_id"))
		if err != nil {
			http.Error(w, "Invalid item id", http.StatusBadRequest)
			return
		}
		client, err := strconv.Atoi(r.FormValue("client_id"))
		if err != nil {
			http.Error(w, "Invalid client id", http.StatusBadRequest)
			return
		}
		orderer, err := strconv.Atoi(r.FormValue("orderer_id"))
		if err != nil {
			http.Error(w, "Invalid orderer id", http.StatusBadRequest)
			return
		}

		err = InsertWithIds(employee, price, item, client, orderer)
		if err != nil {
			http.Error(w, "Error while inserting", http.StatusBadRequest)
			return
		}
		http.Redirect(w, r, "/index", http.StatusSeeOther)
	}
}

func update_handler(w http.ResponseWriter, r *http.Request) {
	if !IsAuthorized(r, USER) {
		http.Error(w, "Unauthorized", http.StatusUnauthorized)
		return
	}
	if r.Method == http.MethodGet {
		request_id := r.URL.Query().Get("id")
		id, err := strconv.Atoi(request_id)
		if err != nil {
			http.Error(w, "Id not present", http.StatusBadRequest)
			return
		}
		to_update, err := GetOrderRaw(id)
		employes := getIdNameOrdered("employes", to_update.EmployeeId)
		items := getIdNameOrdered("items", to_update.ItemId)
		clients := getIdNameOrdered("clients", to_update.ClientId)
		orderers := getIdNameOrdered("orderers", to_update.OrdererId)
		PrintUpdate(w, id, employes, to_update.Price, items, clients, orderers)
	}
	if r.Method == http.MethodPost {
		var err error
		id, err := strconv.Atoi(r.FormValue("id"))
		if err != nil {
			http.Error(w, "Invalid order id", http.StatusBadRequest)
			return
		}
		employee, err := strconv.Atoi(r.FormValue("employee_id"))
		if err != nil {
			http.Error(w, "Invalid employee id", http.StatusBadRequest)
			return
		}
		price, err := strconv.Atoi(r.FormValue("price"))
		if err != nil {
			http.Error(w, "Invalid price", http.StatusBadRequest)
			return
		}
		item, err := strconv.Atoi(r.FormValue("item_id"))
		if err != nil {
			http.Error(w, "Invalid item id", http.StatusBadRequest)
			return
		}
		client, err := strconv.Atoi(r.FormValue("client_id"))
		if err != nil {
			http.Error(w, "Invalid client id", http.StatusBadRequest)
			return
		}
		orderer, err := strconv.Atoi(r.FormValue("orderer_id"))
		if err != nil {
			http.Error(w, "Invalid orderer id", http.StatusBadRequest)
			return
		}

		err = UpdateWithIds(id, employee, price, item, client, orderer)
		if err != nil {
			http.Error(w, "Error while updating", http.StatusBadRequest)
			return
		}
		http.Redirect(w, r, "/index", http.StatusSeeOther)
	}
}

func getIdNameOrdered(table string, id int) []IdName {
	result := GetIdName(table)
	for i, idName := range result {
		if idName.Id == id {
			tmp := result[i]
			result = append(result[:i], result[i+1:]...)
			result = slices.Insert(result, 0, tmp)
		}
	}
	return result
}

func delete_handler(w http.ResponseWriter, r *http.Request) {
	if !IsAuthorized(r, ADMIN) {
		http.Error(w, "Unauthorized", http.StatusUnauthorized)
		return
	}
	request_id := r.URL.Query().Get("id")
	id, err := strconv.Atoi(request_id)
	if err != nil {
		http.Error(w, "Invalid id", http.StatusBadRequest)
		return
	}
	err = Delete(id)
	if err != nil {
		http.Error(w, "Wrong id", http.StatusBadRequest)
		return
	}
	http.Redirect(w, r, "/index", http.StatusSeeOther)
}

var login_error_str = "Invalid login"

func login_handler(w http.ResponseWriter, r *http.Request) {
	if r.Method == http.MethodGet {
		PrintLogin(w, nil)
	}
	if r.Method == http.MethodPost {
		username := r.FormValue("username")
		password := r.FormValue("password")
		user, err := GetUserRecord(username)
		if err == nil && strings.Compare(password, user.Password) == 0 {
			err = CreateSession(w, r, user)
		}

		if err != nil {
			PrintLogin(w, &login_error_str)
		} else {
			http.Redirect(w, r, "/index", http.StatusSeeOther)
		}
	}
}

func logout_handler(w http.ResponseWriter, r *http.Request) {
	DeleteCurrentSession(w, r)
	http.Redirect(w, r, "/index", http.StatusSeeOther)
}

func main() {
	var err error
	err = InitModel()
	if err != nil {
		log.Fatal(err)
		return
	}
	defer DeinitModel()
	http.HandleFunc("/index", index_handler)
	http.HandleFunc("/insert", insert_handler)
	http.HandleFunc("/update", update_handler)
	http.HandleFunc("/delete", delete_handler)
	http.HandleFunc("/login", login_handler)
	http.HandleFunc("/logout", logout_handler)
	fmt.Println("Start listening at http://localhost:8084/index")
	log.Fatal(http.ListenAndServe(":8084", nil))
}
