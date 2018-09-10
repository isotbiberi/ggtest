#!/usr/bin/python

# script which demonstartes move functions
#
# (C) 2010 Petr Kubanek <kubanek@fzu.cz>

import rts2.scriptcomm
class Test(rts2.scriptcomm.Rts2Comm):
	def __init__(self):
		rts2.scriptcomm.Rts2Comm.__init__(self)
	
	def run(self):
                #self.altaz(20,20)
		self.setValue('exposure',0.1)
		data=self.exposure(fileexpand='ismail.fits')
                self.log('I', 'first pixel is {0}'.format(len(data)))
                test=self.getValueFloat('average')  
                self.log('I','average is {0}' . format(test))

if __name__ == "__main__":
	a = Test()
	a.run()
