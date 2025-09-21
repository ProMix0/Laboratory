package main

import (
	"errors"
	"github.com/gorilla/sessions"
	"net/http"
)

const session_name = "session-name"

var store = sessions.NewCookieStore([]byte("secret-key"))

type SessionEntry struct {
	User UserRecord
}

func GetSession(r *http.Request) (SessionEntry, error) {
	var result SessionEntry
	session, _ := store.Get(r, session_name)
	if session.IsNew {
		return result, errors.New("Session doesn't exist")
	}

	var ok bool
	result.User.Username, ok = session.Values["username"].(string)
	if !ok {
		return result, errors.New("Username for session not set")

	}
	var user_role int
	user_role, ok = session.Values["role"].(int)
	if !ok {
		return result, errors.New("User role for session not set")

	}
	result.User.Role = UserRole(user_role)
	return result, nil
}

func IsAuthorized(r *http.Request, role UserRole) bool {
	session, err := GetSession(r)
	if err != nil {
		return false
	}

	return session.User.Role >= role
}

func CreateSession(w http.ResponseWriter, r *http.Request, user UserRecord) error {
	var user_role int
	user_role = int(user.Role)
	session := sessions.NewSession(store, session_name)
	session.Values["username"] = user.Username
	session.Values["role"] = user_role
	err := session.Save(r, w)
	return err
}

func DeleteCurrentSession(w http.ResponseWriter, r *http.Request) error {
	session, _ := store.Get(r, session_name)
	if session.IsNew {
		return errors.New("Session doesn't exist")
	}
	session.Options.MaxAge = -1
	session.Options.Secure = false
	err := session.Save(r, w)
	return err
}
