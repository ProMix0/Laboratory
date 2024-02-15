from flask import *
import sqlite3

app = Flask(__name__)

def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect("db.sql")
    return db

@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()

@app.route("/")
def list_entries():
    result = ""
    cursor = get_db().cursor()
    for (name, repo, id) in cursor.execute("SELECT name, repo, ROWID FROM portfolio"):
        result += f'<p>Name: <a href="/{id}">{name}</a>, repo: <a href="{repo}">{repo}</a></p>'
    return result

@app.get("/<int:id>")
def user_get(id):
    result = ""
    cursor = get_db().cursor()
    row = cursor.execute("SELECT name, repo FROM portfolio WHERE ROWID = ?", (id, )).fetchone()
    if row is None:
        return "Unexisting ID!"
    (name, repo) = row
    result += f'<p>Name: {name}, repo: <a href="{repo}">{repo}</a></p>'
    return result

@app.get("/new")
def new_user_get():
    return """
    <div class="container mt-5">
        <h1>New employee</h1>
        <form method="POST">
            <input type="text" name="name" id="name" class="form-control"><br>
            <textarea name="repo" id="repo" class="form-control"></textarea><br>
            <input type="submit" class="btn btn-success" value="Submit"><br>
        </form>
    </div>
    """

@app.post("/new")
def new_user_post():
    name = request.form['name']
    repo = request.form['repo']
    cursor = get_db().cursor()
    cursor.execute('INSERT INTO portfolio VALUES (?, ?)', (name, repo))
    get_db().commit()
    return redirect(url_for('user', id=cursor.lastrowid))
