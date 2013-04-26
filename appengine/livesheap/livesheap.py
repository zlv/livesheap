#!/usr/bin/python
# -*- coding: utf-8 -*-
#сервер социальной игры
#copyright (c) 2013 Евгений Лежнин
import webapp2

from google.appengine.ext import db
import hashlib
import time

#главная страница
class MainPage(webapp2.RequestHandler):
    userList = []
    userDict = {}
    minTimeForKick = 300.0

    def checkListForDeadPeople(self) :
        i = 0
        for curuser in self.userList :
            if curuser and time.time()-curuser.time>self.minTimeForKick :
                self.userList[i] = 0
                self.userDict[curuser.username] = 0
            i = i+1

    def post(self):
        user = User()
        #регистрация, либо вход / login
        user.username = self.request.get('username')
        user.password = self.request.get('password')
        if (self.request.get('id')!='') :
            id = int(self.request.get('id'))
        if (self.request.get('secondid')!='') :
            sid = int(self.request.get('secondid'))
        bAlreadyOnServer = (user.username in self.userDict) and self.userDict[user.username]
        user.ip = self.request.remote_addr
        #определяем, что это / sign up or sign in
        type = self.request.get('type')
        #создавали ли уже / is already created?
        users = db.GqlQuery("SELECT * FROM User WHERE username = :1", user.username)
        hash = hashlib.md5()
        hash.update(user.password)
        if type=='login' :
            if bAlreadyOnServer :
                self.checkListForDeadPeople()
                bAlreadyOnServer = (user.username in self.userDict) and self.userDict[user.username]
            if users.count()==1 and not bAlreadyOnServer :
                if users[0].password==hash.hexdigest() :
                    self.response.write('200 %s' % user.username) #существует / exists
                    userinfo = user.info()
                    userinfo.time = time.time()
                    self.userList.append(userinfo)
                    self.userDict[user.username] = len(self.userList)
                    for curuser in self.userList :
                        if curuser :
                            curuser.changedList.append(Userchange(1,self.userDict[curuser.username]))
                else :
                    self.response.write('400 1 %s' % user.username) #неверный пароль / wrong password
            elif not bAlreadyOnServer :
                self.response.write('400 0 %s' % user.username) #не существует / not exists
            else :
                self.response.write('400 2 %s' % user.username) #уже там / already here
        elif type=='greetings' :
            print 'check'
            print self.userList
            print self.userList[len(self.userList)-1].time
            self.checkListForDeadPeople()
            print time.time()
            print 'checked'
            print self.userList
            length = 0
            usersString = ""
            for curuser in self.userList :
                if curuser and (curuser.username!=user.username or curuser.ip!=user.ip) and curuser.bFree :
                    if length!=0 :
                        usersString += ' '
                    usersString += '%(a)s $ %(b)s %(c)s 0' % {'a' : curuser.username, 'b' : curuser.ip, 'c' : self.userDict[curuser.username]}
                    length += 1
            self.response.write('200 %(a)d %(b)d %(c)s' % {'a' : self.userDict[user.username],'b' : length,'c' : usersString}) #существует / exists
        elif type=='bye' :
            if len(self.userList)<id or self.userList[id-1]==0 :
                return
            user = self.userList[id-1]
            self.userDict[user.username] = 0
            self.userList[id-1] = 0
            for curuser in self.userList :
                if curuser :
                    curuser.changedList.append(Userchange(0,id))
        elif type=='register' :
            if users.count()==0 and user.username!="" :
                user.password = hash.hexdigest()
                user.put()
                self.response.write('200') #свободно / free
            else :
                self.response.write('400 %s' % user.username) #не свободно / already created
        elif type=='howAreYou' :
            print self.userList
            print id
            if len(self.userList)<id or self.userList[id-1]==0 :
                self.response.write('400')
                return
            self.userList[id-1].time = time.time()
            if self.userList[id-1].idWannaPlay :
                self.response.write("100 %d" % self.userList[id-1].idWantToPlay)
                self.userList[id-1].idWantToPlay = 0
            if self.userList[id-1].changedList!=[] :
                usersString = ""
                length = 0
                for change in self.userList[id-1].changedList :
                    if length!=0 :
                        usersString += ' '
                    if change.added==0 :
                        usersString += '0 %(a)d' % {'c' : change.id}
                    else :
                        curuser = self.userList[change.id-1]
                        usersString += '%(a)s $ %(b)s %(c)s 0' % {'a' : curuser.username, 'b' : curuser.ip, 'c' : change.id-1}
                    length += 1
                self.response.write('200 %(a)d %(b)s' % {'a' : length, 'b' : usersString})
                self.userList[id-1].changedList = []
        elif type=='wannaChatWith' :
            self.userList[sid-1].idWantToPlay = id

    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        users = db.GqlQuery("SELECT * FROM User")
        for user in users: #список пользователей / user list
            self.response.write('user: %s\n' % user.username)
        self.response.write('time: %s\n' % time.time())
        self.response.write('on server: %s\n' % self.userList)

class Userchange:
    def __init__(self, added, id):
        self.added = added
        self.id = id
    added = 0
    id = 0

class Userinfo: #минимум информации
    def __init__(self, uname, ip):
        self.username = uname
        self.ip = ip
    username = ""
    ip = ""
    time = time.time()
    bFree = 1
    idWannaPlay = 0
    changedList = []

class User(db.Model):
    username = db.StringProperty()
    password = db.StringProperty()
    ip = db.StringProperty()
    def info(self) :
        userinfo = Userinfo(self.username, self.ip)
        return userinfo

app = webapp2.WSGIApplication([('/', MainPage)], debug=True)
