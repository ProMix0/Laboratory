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
    result = '<a href="/new"><h1>Add new</h1></a>'
    connection = get_db()
    for (name, repo, id) in connection.execute("SELECT name, repo, ROWID FROM portfolio"):
        result += f'<p>Name: <a href="/{id}">{name}</a>, repo: <a href="{repo}">{repo}</a></p>'
    return result

@app.get("/<int:id>")
def user_get(id):
    result = '<a href="/"><h1>Home</h1></a>'
    connection = get_db()
    row = connection.execute("SELECT name, repo FROM portfolio WHERE ROWID = ?", (id, )).fetchone()
    if row is None:
        return "Unexisting ID!"
    (name, repo) = row
    result += f'<p>Name: {name}, repo: <a href="{repo}">{repo}</a></p>'
    result += f"""
    <div class="container mt-5">
        <form method="POST">
            <input type="text" name="name" id="name" class="form-control" value="{name}"><br>
            <textarea name="repo" id="repo" class="form-control">{repo}</textarea><br>
            <input type="submit" class="btn btn-success" value="Update">
        </form>
        <a href="/{id}/delete" data-method="delete">Delete</a>
    </div>
    """
    return result

@app.post("/<int:id>")
def user_update(id):
    name = request.form['name']
    repo = request.form['repo']
    if '.' not in repo:
        return "Unable to add invalid repo"
    connection = get_db()
    connection.execute("UPDATE portfolio SET name = ?, repo = ? WHERE ROWID = ?", (name, repo, id))
    connection.commit()
    return redirect(url_for('user_get', id=id))

@app.get("/<int:id>/delete")
def user_delete(id):
    connection = get_db()
    connection.execute("DELETE FROM portfolio WHERE ROWID = ?", (id, ))
    connection.commit()
    return redirect(url_for('list_entries'))

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
    if '.' not in repo:
        return "Unable to add invalid repo"
    connection = get_db()
    cursor = connection.execute('INSERT INTO portfolio VALUES (?, ?)', (name, repo))
    connection.commit()
    return redirect(url_for('user_get', id=cursor.lastrowid))
