import webapp2

from google.appengine.ext import db

class MainPage(webapp2.RequestHandler):

    def post(self):
        user_name = self.request.get('username')
        password = self.request.get('password')

    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.write('Hello, webapp2 World!')
        greetings = db.GqlQuery("SELECT * FROM User", "Users")


class User(db.Model):
    user_name = db.StringProperty()
    password = db.StringProperty()

app = webapp2.WSGIApplication([('/', MainPage)], debug=True)
