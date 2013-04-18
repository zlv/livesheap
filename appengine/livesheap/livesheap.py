#!/usr/bin/python
# -*- coding: utf-8 -*-
#сервер социальной игры
#copyright (c) 2013 Евгений Лежнин
import webapp2

from google.appengine.ext import db
import hashlib

#главная страница
class MainPage(webapp2.RequestHandler):
    userList = []
    userDict = {}

    def post(self):
        user = User()
        #регистрация, либо вход / login
        user.username = self.request.get('username')
        user.password = self.request.get('password')
        user.ip = self.request.remote_addr
        #определяем, что это / sign up or sign in
        type = self.request.get('type')
        #создавали ли уже / is already created?
        users = db.GqlQuery("SELECT * FROM User WHERE username = :1", user.username)
        hash = hashlib.md5()
        hash.update(user.password)
        if type=='login' :
            if users.count()==1 and (users[0].username!=user.username or users[0].password!=user.password) :
                if users[0].password==hash.hexdigest() :
                    self.response.write('200 %s' % user.username) #существует / exists
                    self.userList.append(user.info())
                    self.userDict[user.username] = len(self.userList)
                else :
                    if users[0].username==user.username and users[0].password!=user.password :
                        self.response.write('400 2 %s' % user.username) #уже на сервере / already on server
                    else :
                        self.response.write('400 1 %s' % user.username) #неверный пароль / wrong password
            else :
                self.response.write('400 0 %s' % user.username) #не существует / not exists
        elif type=='greetings' :
            length = 0
            usersString = ""
            for curuser in self.userList :
                if curuser.username!=user.username or curuser.ip!=user.ip :
                    usersString += ' %s $ 0' % curuser.username
                    length += 1
            self.response.write('200 %(a)d %(b)d %(c)s' % {'a' : self.userDict[user.username],'b' : length,'c' : usersString}) #существует / exists
        elif type=='bye' :
            id = int(self.request.get('id'))
            user = self.userList[id-1]
            self.userDict[user.username] = 0
            self.userList.remove(user)
        else :
            if users.count()==0 and user.username!="" :
                user.password = hash.hexdigest()
                user.put()
                self.response.write('200') #свободно / free
            else :
                self.response.write('400 %s' % user.username) #не свободно / already created

    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        users = db.GqlQuery("SELECT * FROM User")
        for user in users: #список пользователей / user list
            self.response.write('user: %s\n' % user.username)

class Userinfo: #минимум информации
    def __init__(self, uname, ip):
        self.username = uname
        self.ip = ip
    username = ""
    ip = ""

class User(db.Model):
    username = db.StringProperty()
    password = db.StringProperty()
    ip = db.StringProperty()
    def info(self) :
        userinfo = Userinfo(self.username, self.ip)
        return userinfo

app = webapp2.WSGIApplication([('/', MainPage)], debug=True)
