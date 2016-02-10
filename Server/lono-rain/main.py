import webapp2
from google.appengine.ext import ndb
import datetime

class Device(ndb.model):
    #Stores information about the rain gauge device
    id = ndb.IntegerProperty(required=True)    
    name = ndb.StringProperty()
    position = ndb.GeoPtProperty()
    dateRegistered = ndb.DateTimeProperty(required=True)
    lastSeen = ndb.DateTimeProperty()

def getDeviceFromId(id):
    q = ndb.gql("SELECT * FROM Device WHERE id =:_id", _id = int(id)).fetch()
    if len(q) == 1:
        return q[0]
    else
        return None

class Reading(ndb.model):
    #Stores data readings from devices
    device = ndb.KeyProperty(required=True,kind=Device)
    reading = ndb.FloatProperty(required=True) #5min rainfall total in mm
    date = ndb.DateTimeProperty(required=True)

class TestPageHandler(webapp2.RequestHandler):
	def get(self):
		self.response.headers.add_header("Access-Control-Allow-Origin", "*")
		self.response.write("Project Lono Rain Gauge Server!")

class RegisterPageHander(webapp2.RequestHandler):
    def post(self):
        self.response.headers.add_header("Access-Control-Allow-Origin","*")
        id = self.request.get("id")
        device = getDeviceFromId(id)
        if (device == None)
            d = Device()
            d.id = id
            d.dateRegistered = datetime.datetime.now()
            d.put()
        else
            device.lastSeen = datetime.datetime.now()
            device.put()
        self.response.write(datetime.datetime.now()) 

class NewReadingPageHandler(webapp2.RequestHandler):
    def post(self):
        self.response.headers.add_header("Access-Control-Allow-Origin","*")
        id = self.request.get("id")
        reading = self.request.get("data")
        r = Reading()
        r.device = getDeviceFromId(id).key
        r.reading = float(reading)
        r.date = datetime.datetime.now()
        r.put()
        self.response.write("Done")
        
app = webapp2.WSGIApplication([('/', TestPageHandler),
                               ('/reg',RegisterPageHander),
                               ('/send',NewReadingPageHandler)
], debug=True)