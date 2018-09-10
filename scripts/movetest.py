#!/usr/bin/python

# script which demonstartes move functions
#
# (C) 2010 Petr Kubanek <kubanek@fzu.cz>

import rts2.scriptcomm

class Test(rts2.scriptcomm.Rts2Comm):
	def __init__(self):
		rts2.scriptcomm.Rts2Comm.__init__(self)
	
	def run(self):
		self.radec("20:30:40",20)

if __name__ == "__main__":
	a = Test()
	a.run()
