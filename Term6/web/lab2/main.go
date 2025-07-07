package main

import (
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"time"
)

var file *os.File

const post_log = "post.log"
const form_html = `
<form method="POST" action="">
<p><input type="checkbox" name="read">Read</p>
<p><input type="checkbox" name="write">Write</p>
<p><input type="checkbox" name="edit">Edit</p>
<p><input type="checkbox" name="delete">Delete</p>
<p><input type="SUBMIT"></p></form>
<a href="/log">View logs</a>
`

func handler(w http.ResponseWriter, r *http.Request) {
	cookie, err := r.Cookie("cookie")
	cookie_text := ""
	if err == nil {
		cookie_text = cookie.Value
	}

	if r.Method == http.MethodPost {
		form_values := fmt.Sprintf("Read:%s, Write:%s, Edit:%s, Delete:%s",
			r.FormValue("read"),
			r.FormValue("write"),
			r.FormValue("edit"),
			r.FormValue("delete"))

		cookie := &http.Cookie{
			Name:  "cookie",
			Value: form_values,
		}
		fmt.Fprintf(file, "%s:\t%s\n", time.Now().Format(time.UnixDate), form_values)
		http.SetCookie(w, cookie)
		http.Redirect(w, r, "/index", http.StatusSeeOther)
	}

	fmt.Fprintf(w, "<h1>Just a form, bro</h1><p>%s</p><div>%s</div>", cookie_text, form_html)
}

func handler_log(w http.ResponseWriter, r *http.Request) {
	file, err := os.Open(post_log)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()
	_, _ = io.Copy(w, file)
}

func main() {
	var err error
	file, err = os.OpenFile(post_log, os.O_APPEND|os.O_CREATE|os.O_RDWR, 0644)
	if err != nil {
		log.Fatal(err)
		return
	}
	defer file.Close()
	http.HandleFunc("/index", handler)
	http.HandleFunc("/log", handler_log)
	fmt.Println("Start listening at http://localhost:8080/index")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
