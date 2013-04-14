#!/usr/bin/python
# -*- coding: utf-8 -*-
#сервер социальной игры
#copyright (c) 2013 Евгений Лежнин
import webapp2

from google.appengine.ext import db

#главная страница
class MainPage(webapp2.RequestHandler):

    def post(self):
        user = User()
        #регистрация, либо вход / login
        user.username = self.request.get('username')
        user.password = self.request.get('password')
        #определяем, что это / sign up or sign in
        type = self.request.get('type')
        #создавали ли уже / is already created?
        users = db.GqlQuery("SELECT * FROM User WHERE username = :1", user.username)
        if users.count()==0 :
            user.put()
            self.response.write('200') #свободно / free
        else :
            self.response.write('400 %s' % user.username) #не свободно / already created

    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        users = db.GqlQuery("SELECT * FROM User")
        for user in users: #список пользователей / user list
            self.response.write('user: %s\n' % user.username)

class User(db.Model):
    username = db.StringProperty()
    password = db.StringProperty()

app = webapp2.WSGIApplication([('/', MainPage)], debug=True)
