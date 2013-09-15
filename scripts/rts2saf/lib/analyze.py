#!/usr/bin/python
# (C) 2013, Markus Wildi, markus.wildi@bluewin.ch
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software Foundation,
#   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#   Or visit http://www.gnu.org/licenses/gpl.html.
#

__author__ = 'markus.wildi@bluewin.ch'

import os
import numpy as np

from ds9 import *

try:
    import lib.data as dt
except:
    import data as dt
try:
    import lib.ds9region as ds9r
except:
    import ds9region as ds9r
try:
    import lib.fitfwhm as ft
except:
    import fitfwhm as ft


class Analyze(object):
    """Analyze a set of FITS"""
    def __init__(self, debug=False, dataSex=None, displayDs9=False, displayFit=False, logger=None):
        self.debug=debug
        self.dataSex=dataSex
        self.displayDs9=displayDs9
        self.displayFit=displayFit
        self.logger=logger

    def analyze(self):
        # very ugly
        nobjs=list()
        pos=list()
        fwhm=list()
        errx=list()
        stdFwhm=list()

        for cnt in self.dataSex.keys():
            # all sextracted objects
            try:
                nObjs= len(self.dataSex[cnt].catalogue)
            except:
                continue

            if self.debug: self.logger.debug('analyze: {0:5d}, sextracted objects: {1:5d}, filtered sextracted objects: {2:5d}'.format(self.dataSex[cnt].focPos, nObjs, self.dataSex[cnt].nstars))
            # star like objects
            nobjs.append(self.dataSex[cnt].nstars)
            pos.append(self.dataSex[cnt].focPos)
            fwhm.append(self.dataSex[cnt].fwhm)
            errx.append(20.)
            stdFwhm.append(self.dataSex[cnt].stdFwhm)
        # Weighted mean based on number of extracted objects (stars)
        weightedMeanObjects= np.average(a=pos, axis=0, weights=nobjs) 
        if self.debug: self.logger.debug('analyze: {0:5d}: weighted mean derived from sextracted objects'.format(int(weightedMeanObjects)))

        # Weighted mean based on median FWHM
        posC= pos[:]
        fwhmC= fwhm[:]
        while True:
            try:
                ind=fwhmC.index(0.)
            except:
                break
            del posC[ind]
            del fwhmC[ind]

        weightedMeanFwhm= np.average(a=posC, axis=0, weights=map( lambda x: 1./x, fwhmC)) 
        self.logger.debug('analyze: {0:5d}: weighted mean derived from FWHM'.format(int(weightedMeanFwhm)))

        # Fit median FWHM data
        dataFwhm=dt.DataFwhm(pos=np.asarray(pos),fwhm=np.asarray(fwhm),errx=np.asarray(errx),stdFwhm=np.asarray(stdFwhm))

        fit=ft.FitFwhm(showPlot=True, filterName='U', objects=10, temperature=20., date='2013-09-08T09:30:09', comment='Test', pltFile='./test.png', dataFwhm=dataFwhm)
        minFwhmPos, fwhm=fit.fitData()

        # make other decissions on if the fit converged
        if minFwhmPos:
            if min(dataFwhm.pos) < minFwhmPos  < max(dataFwhm.pos):
                self.logger.info('analyze: {0:5d}: fitted minimum position inside, FWHM: {1:4.1f}'.format(int(minFwhmPos), fwhm))
            else:
                self.logger.info('analyze: {0:5d}: fitted minimum position outside, FWHM: {1:4.1f}'.format(int(minFwhmPos), fwhm))
                
        else:
            self.logger.warn('analyze: fit failed'.format())
            
        # ToDo ugly here
        DISPLAY=False
        if self.displayFit or self.displayDs9:
            try:
                os.environ['DISPLAY']
                DISPLAY=True
            except:
                self.logger.warn('analyze: no X-Window DISPLAY, do not plot with mathplotlib and/or ds9')

        if DISPLAY:
            if self.displayFit:
                fit.plotData()
            # plot them through ds9
            if self.displayDs9:
                dds9=ds9()

                for cnt, dSx in self.dataSex.iteritems():
                    if dSx.fitsFn:
                        dr=ds9r.Ds9Region( dataSex=dSx, display=dds9, logger=self.logger)
                        if not dr.displayWithRegion():
                            break # something went wrong
                        time.sleep(1.)
                    else:
                        self.logger.warn('analyze: OOOOOOOOPS, no file name for fits image number: {0:3d}'.format(cnt))
                    
        return [weightedMeanObjects, weightedMeanFwhm, minFwhmPos, fwhm]


if __name__ == '__main__':

    import argparse
    import sys
    import logging
    import os
    import glob

    try:
        import lib.config as cfgd
    except:
        import config as cfgd
    try:
        import lib.sextract as sx
    except:
        import sextract as sx

    parser= argparse.ArgumentParser(prog=sys.argv[0], description='rts2asaf analysis')
    parser.add_argument('--debug', dest='debug', action='store_true', default=False, help=': %(default)s,add more output')
    parser.add_argument('--debugSex', dest='debugSex', action='store_true', default=False, help=': %(default)s,add more output on SExtract')
    parser.add_argument('--level', dest='level', default='INFO', help=': %(default)s, debug level')
    parser.add_argument('--logfile',dest='logfile', default='/tmp/{0}.log'.format(sys.argv[0]), help=': %(default)s, logfile name')
    parser.add_argument('--toconsole', dest='toconsole', action='store_true', default=False, help=': %(default)s, log to console')
    parser.add_argument('--config', dest='config', action='store', default='./rts2saf-my.cfg', help=': %(default)s, configuration file path')
    parser.add_argument('--dryfitsfiles', dest='dryfitsfiles', action='store', default='./samples', help=': %(default)s, directory where a FITS files are stored from a earlier focus run')
#ToDo    parser.add_argument('--ds9region', dest='ds9region', action='store_true', default=False, help=': %(default)s, create ds9 region files')
    parser.add_argument('--displayds9', dest='displayDs9', action='store_true', default=False, help=': %(default)s, display fits images and region files')
    parser.add_argument('--displayfit', dest='displayFit', action='store_true', default=False, help=': %(default)s, display fit')

    args=parser.parse_args()

    logformat= '%(asctime)s:%(name)s:%(levelname)s:%(message)s'
    logging.basicConfig(filename=args.logfile, level=args.level.upper(), format= logformat)
    logger = logging.getLogger()

    if args.level in 'DEBUG' or args.level in 'INFO':
        toconsole=True
    else:
        toconsole=args.toconsole

    if toconsole:
    #http://www.mglerner.com/blog/?p=8
        soh = logging.StreamHandler(sys.stdout)
        soh.setLevel(args.level)
        logger.addHandler(soh)

    rt=cfgd.Configuration(logger=logger)
    rt.readConfiguration(fileName=args.config)

    if args.dryfitsfiles:
        dryFitsFiles=glob.glob('{0}/{1}'.format(args.dryfitsfiles, rt.cfg['FILE_GLOB']))

        if len(dryFitsFiles)==0:
            logger.error('analyze: no FITS files found in:{}'.format(args.dryfitsfiles))
            logger.info('analyze: download a sample from wget http://azug.minpet.unibas.ch/~wildi/rts2saf-test-focus-2013-09-14.tgz')
            logger.info('analyze: and store it in directory: {0}'.format(args.dryfitsfiles))
            sys.exit(1)
    else:
        fitsFiles= args.fitsFiles.split()

    cnt=0
    dataSex=dict()
    for fitsFn in dryFitsFiles:
        
        logger.info('analyze: processing fits file: {0}'.format(fitsFn))
        rsx= sx.Sextract(debug=args.debugSex, rt=rt, logger=logger)
        dataSex[cnt]=rsx.sextract(fitsFn=fitsFn) 
        cnt +=1

    an=Analyze(debug=args.debug, dataSex=dataSex, displayDs9=args.displayDs9, displayFit=args.displayFit, logger=logger)
    weightedMeanObjects, weightedMeanFwhm, minFwhmPos, fwhm= an.analyze()

    logger.info('analyze: result: {0}, {1}, {2}, {3}'.format(weightedMeanObjects, weightedMeanFwhm, minFwhmPos, fwhm))