from optparse import OptionParser
import sys
import string
import rts2
import rts2.target
import rts2.radec
import rts2.rtsapi
import rts2.json
import re
import time
import calendar
import json

print 'ismail'
rts2.createProxy(url='localhost:8889', username='ismail', password='ismail', verbose='')
qs = rts2.Queues(rts2.rtsapi.getProxy())
qs.load()
for q in qs.queues:
                                print "Queue {0}: {1}".format(qs.queues[q].name, qs.queues[q])

print 'ismail2'
