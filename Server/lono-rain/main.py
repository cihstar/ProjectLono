import webapp2

class TestPageHandler(webapp2.RequestHandler):
	def get(self):
		self.response.headers.add_header("Access-Control-Allow-Origin", "*")
		self.response.write("Project Lono Rain Gauge Server!")

app = webapp2.WSGIApplication([('/', TestPageHandler)], debug=True)