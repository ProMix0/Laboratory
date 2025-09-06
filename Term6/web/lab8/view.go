package main

import (
	"fmt"
	"io"
	"net/http"
	"strings"
)

const search_form_html = `
<form method="GET" action="">
<p>ID:<input type="number" step="1" min="1" name="id">
<input type="SUBMIT" value="Search"></p></form>
<a href="/index">Show all</a>
<a href="/histogram">View histogram</a>
`
const table_end = `
</tbody></table>
`

const ckeditor_fmt=`
		<link rel="stylesheet" href="./static/ckeditor5/ckeditor5.css"/>
		<div class="main-container">
			<div id="editor">
			%s
			</div>
		</div>
		<script type="importmap">
			{
				"imports": {
					"ckeditor5": "./static/ckeditor5/ckeditor5.js",
					"ckeditor5/": "./static/ckeditor5/"
				}
			}
		</script>
		<script type="module">
			import {
				ClassicEditor,
				Essentials,
				Paragraph,
				Bold,
				Italic,
				Font
			} from 'ckeditor5';

            ClassicEditor
                .create( document.querySelector( '#editor' ), {
                    licenseKey: 'GPL',
                    plugins: [ Essentials, Paragraph, Bold, Italic, Font ],
                    toolbar: [
						'undo', 'redo', '|', 'bold', 'italic', '|',
						'fontSize', 'fontFamily', 'fontColor', 'fontBackgroundColor'
					],
					licenseKey: 'GPL'
				} )
				.then( editor => {
					window.editor = editor;
				} )
				.catch( error => {
					console.error( error );
				} );
		</script>
	`

func make_table_header(w io.Writer, role *UserRole) {
	fmt.Fprintf(w, "%s", `
<table border="1"><tbody>
<tr><th><b>id</b></th><th><b>employee</b></th><th><b>price</b></th><th><b>item</b></th><th><b>client</b></th><th><b>client department</b></th><th><b>orderer</b></th>`)
	if role != nil {
		fmt.Fprintf(w, "%s", `<th><b>Update</b></th>`)
		if *role >= ADMIN {
			fmt.Fprintf(w, "%s", `<th><b>Delete</b></th>`)
		}
	}
	fmt.Fprintf(w, "%s", `</tr>`)
}

func make_row(w io.Writer, id int, employee string, price int, item string, client string, client_department string, orderer string, role *UserRole) {
	fmt.Fprintf(w, `<tr><td>%d</td><td>%s</td><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td>`, id, employee, price, item, client, client_department, orderer)

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
	fmt.Fprintf(w, ckeditor_fmt, query)

	var role *UserRole = nil
	if session != nil {
		role = &session.User.Role
	}
	make_table_header(w, role)

	for _, order := range orders {
		make_row(w, order.Id, order.Employee, order.Price, order.Item, order.Client, order.ClientDepartment, order.Orderer, role)
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

	fmt.Fprintf(w, `Client<select id="client_id_select" name="client_id">`)
	print_options(w, clients)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, `Client department<select id="client_department_id_select" name="client_department_id"></select>`)
	fmt.Fprintf(w, `
	<script src="static/jquery.js"></script>
	<script>
	function update_client_domains() {
		$('#client_department_id_select').load('get_client_departments', 'id=' + $('#client_id_select').val());
	}
	$(document).ready(function() {
		update_client_domains();
	});
	$('#client_id_select').on('change', function() {
		update_client_domains();
	});
	</script>
	`);

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

	fmt.Fprintf(w, `Client<select id="client_id_select" name="client_id">`)
	print_options(w, clients)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, `Client department<select id="client_department_id_select" name="client_department_id"></select>`)
	fmt.Fprintf(w, `
	<script src="static/jquery.js"></script>
	<script>
	function update_client_domains() {
		$('#client_department_id_select').load('get_client_departments', 'id=' + $('#client_id_select').val());
	}
	$(document).ready(function() {
		update_client_domains();
	});
	$('#client_id_select').on('change', function() {
		update_client_domains();
	});
	</script>
	`);

	fmt.Fprintf(w, "Orderer<select name=\"orderer_id\">")
	print_options(w, orderers)
	fmt.Fprintf(w, "</select>")

	fmt.Fprintf(w, "<input type=\"hidden\" name=\"id\" value=\"%d\"/>", id)
	fmt.Fprintf(w, "<input type=\"SUBMIT\" value=\"Update\"></p></form></html>")
}

func PrintClientDepartments(w http.ResponseWriter, client_departments []IdName) {
	print_options(w, client_departments)
}

func PrintLogin(w http.ResponseWriter, error *string) {
	fmt.Fprintf(w, "<html><form id=\"login-form\" method=\"POST\" action=\"/login\">")

	fmt.Fprintf(w, "<p>Login<input name=\"username\" minlength=\"2\" required/></p>")
	fmt.Fprintf(w, "<p>Password<input name=\"password\" minlength=\"2\" required type=\"password\"/></p>")

	fmt.Fprintf(w, `<div><input class="jCaptcha" type="number" placeholder="Type in result please">`)
	fmt.Fprintf(w, `
	   <script src="static/jCaptcha.js"></script>
           <script src="static/jquery.js"></script>
           <script src="static/jquery.validate.js"></script>
	   <script>
    let myCaptcha = new jCaptcha({
	requiredValue: '=',
        canvasStyle: {
            // required properties for captcha stylings:
            width: 100,
            height: 15,
            textBaseline: 'top',
            font: '15px Arial',
            textAlign: 'left',
            fillStyle: '#ddd'
        }
    });
    $("#login-form").validate({
        submitHandler: function(form) {
            if(myCaptcha.validate()){
                form.submit();
            }
        }
    });
</script></div>
    `)
	fmt.Fprintf(w, "<p><input type=\"SUBMIT\" value=\"Login\"></p></form>")

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

const histogram_script = `
<script src="static/chart.js"></script>

<script>
  const ctx = document.getElementById('histogram');

  new Chart(ctx, {
    type: 'bar',
    data: {
      labels: [%s],
      datasets: [{
        data: [%s],
        borderWidth: 1
      }]
    },
    options: {
      scales: {
        y: {
          beginAtZero: true
        }
      }
    }
  });
</script>
`

func PrintHistogram(w http.ResponseWriter, query string, data []ClientAndSum) {
	fmt.Fprintf(w, "<h1>Orders sum by clients</h1><div>")
	fmt.Fprintf(w, "<pre>%s</pre>", query)

	var labels strings.Builder
	var values strings.Builder
	for _, element := range data {
		fmt.Fprintf(&labels, "'%s',", element.Client)
		fmt.Fprintf(&values, "%d,", element.Sum)
	}
	fmt.Fprintf(w, `<div><canvas id="histogram"/></div>`)
	fmt.Fprintf(w, histogram_script, labels.String(), values.String())

	fmt.Fprintf(w, "</div>")
}
