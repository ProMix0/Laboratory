import os
import sys
from django.conf import settings
from django.core.management import execute_from_command_line
from django.urls import path
from django.http import *
import sqlite3

def list_entries(request):
    result = '<a href="/new"><h1>Add new</h1></a>'
    with sqlite3.connect("db.sql") as connection:
        for (name, repo, id) in connection.execute("SELECT name, repo, ROWID FROM portfolio"):
            result += f'<p>Name: <a href="/{id}">{name}</a>, repo: <a href="{repo}">{repo}</a></p>'
        return HttpResponse(result)

def user(request, id):
    if request.method == "GET":
        with sqlite3.connect("db.sql") as connection:
            result = '<a href="/"><h1>Home</h1></a>'
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
            return HttpResponse(result)
    if request.method == "POST":
        name = request.POST.get('name')
        repo = request.POST.get('repo')
        if '.' not in repo:
            return HttpResponse("Unable to add invalid repo")
        with sqlite3.connect("db.sql") as connection:
            connection.execute("UPDATE portfolio SET name = ?, repo = ? WHERE ROWID = ?", (name, repo, id))
            connection.commit()
            return HttpResponseRedirect(f"/{id}")

def user_delete(request, id):
    with sqlite3.connect("db.sql") as connection:
        connection.execute("DELETE FROM portfolio WHERE ROWID = ?", (id, ))
        connection.commit()
        return HttpResponseRedirect(f"/")

def user_new(request):
    print(request.method)
    if request.method == "GET":
        with sqlite3.connect("db.sql") as connection:
            return HttpResponse("""
            <div class="container mt-5">
                <h1>New employee</h1>
                <form method="POST">
                    <input type="text" name="name" id="name" class="form-control"><br>
                    <textarea name="repo" id="repo" class="form-control"></textarea><br>
                    <input type="submit" class="btn btn-success" value="Submit"><br>
                </form>
            </div>
            """)
    if request.method == "POST":
        with sqlite3.connect("db.sql") as connection:
            name = request.POST.get('name')
            repo = request.POST.get('repo')
            if '.' not in repo:
                return HttpResponse("Unable to add invalid repo")
            cursor = connection.execute('INSERT INTO portfolio VALUES (?, ?)', (name, repo))
            connection.commit()
            return HttpResponseRedirect(f"/{cursor.lastrowid}")

urlpatterns = [
    path("", list_entries),
    path("new", user_new),
    path("<int:id>", user),
    path("<int:id>/delete", user_delete),
]

if __name__ == "__main__":
    fname = os.path.splitext(os.path.basename(__file__))[0]
    settings.configure(DEBUG=True, MIDDLEWARE_CLASSES=[], ROOT_URLCONF=fname)
    execute_from_command_line([sys.argv[0], 'runserver'])

