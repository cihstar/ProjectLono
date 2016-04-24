# Python Server created to accept incoming data from rain gauges
# Runs on Google App Engine.
# Create account at appengine.google.com, downlaod the launcher to deploy app.

# Database contains a device and a reading

# Also can tweet via the Tweepy library, the current api keys in the code are for twitter.com/projectlono

import webapp2
from google.appengine.ext import ndb
import datetime
import json
import tweepy

class Device(ndb.Model):
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
    else:
        return None

class Reading(ndb.Model):
    #Stores data readings from devices
    device = ndb.KeyProperty(required=True,kind=Device, indexed=True)
    reading = ndb.FloatProperty(required=True) #rainfall total in mm over the interval
    interval = ndb.IntegerProperty(required=True) #in seconds
    timerx = ndb.DateTimeProperty(required=True)
    time = ndb.DateTimeProperty(required=True, indexed=True)

class TestPageHandler(webapp2.RequestHandler):
	def get(self):
		self.response.headers.add_header("Access-Control-Allow-Origin", "*")
		self.response.write("Project Lono Rain Gauge Server!")

class RegisterPageHander(webapp2.RequestHandler):
    def post(self):
        self.response.headers.add_header("Access-Control-Allow-Origin","*")
        id = self.request.get("id")        
        if id == "":
            self.response.write("No ID")
        else:
            device = getDeviceFromId(id)
            if (device == None):
                d = Device()
                d.id = int(id)
                d.dateRegistered = datetime.datetime.now()
                d.lastSeen = datetime.datetime.now()
                d.put()
            else:
                device.lastSeen = datetime.datetime.now()
                device.put()
            self.response.write(datetime.datetime.now().strftime("%d/%m/%Y %H:%M:%S")) 

class NewReadingPageHandler(webapp2.RequestHandler):
    def post(self):
        #Save the reading to database
        self.response.headers.add_header("Access-Control-Allow-Origin","*")
        id = self.request.get("id")
        reading = self.request.get("reading")
        interval = self.request.get("interval")
        time = self.request.get("time")
        r = Reading()
        r.device = getDeviceFromId(id).key
        r.reading = float(reading)
        r.interval = int(interval)
        r.time =  datetime.datetime.strptime(time, "%d/%m/%Y %H:%M:%S")
        r.timerx = datetime.datetime.now()
        r.put()
        
        #Send tweet
        auth = tweepy.OAuthHandler("sbaQ8u5zHGZxYKk2bw3YzBanV", "91wZog7E7RsVsYBE87JfZCKyqMLeWlAv3XoJWbqLoMLwDuwTsr")                    
        auth.set_access_token("704310511677087745-Y31U8td767SJSrQ6ZbgfTJAaUugremV", "6rP6RtDGTqbfykOjDlr8A5xADeZK69kkm71COT6IMeSdV")
        api = tweepy.API(auth)
       # api.update_status('(Test) Current Rain Fall rate: ' + str(float(reading) * ((60*60)/int(interval)))+"mm/h")
        
        self.response.write("Done")

class GetDataForDeviceInTimeRange(webapp2.RequestHandler):
    def post(self):
        self.response.headers.add_header("Access-Control-Allow-Origin","*")
        id = self.request.get("id")
        start = datetime.datetime.strptime(self.request.get("start"), "%d/%m/%Y %H:%M:%S")
        end = datetime.datetime.strptime(self.request.get("end"), "%d/%m/%Y %H:%M:%S")
        
        response = []
        q = ndb.gql("SELECT * FROM Reading WHERE device =:_device AND time >=:_start AND time <=:_end", _device = getDeviceFromId(int(id)).key, _start = start, _end = end).fetch()
        for r in q:        
            response.append({'time': r.time.strftime("%Y-%m-%d %H:%M:%S"), 'interval': r.interval, 'value':r.reading})       
        
        self.response.write(json.dumps(response))

class GetDeviceListHandler(webapp2.RequestHandler):
    def get(self):
        response = []
        q = ndb.gql("SELECT * FROM Device")
        for d in q:
            response.append({'id': d.id, 'name': d.name, 'position': d.position.lat })
        self.response.write(json.dumps(response))
  
class EditDeviceDetailsHandler(webapp2.RequestHandler):
    def post(self):
        name = self.request.get("name")
        lat = self.request.get("lat")  
        longo = self.request.get("long"); 
        id = self.request.get("id")
        d = getDeviceFromId(id)
        d.name = name
        d.position = ndb.GeoPt(float(lat), float(longo))
        d.put()
        self.response.write("Done")
             
app = webapp2.WSGIApplication([('/', TestPageHandler),
                               ('/reg', RegisterPageHander),
                               ('/send', NewReadingPageHandler),
                               ('/getDataForDeviceInRange', GetDataForDeviceInTimeRange),
                               ('/getDeviceList', GetDeviceListHandler),
                               ('/editDeviceDetails', EditDeviceDetailsHandler)
], debug=True)