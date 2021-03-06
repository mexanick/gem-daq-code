#!/bin/env python

import sys, re
import time, datetime, os

sys.path.append('${GEM_PYTHON_PATH}')

import uhal
from registers_uhal import *
from glib_system_info_uhal import *
from rate_calculator import errorRate
from glib_user_functions_uhal import *

#from glib_clock_src import *

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-s", "--slot", type="int", dest="slot",
		  help="slot in uTCA crate", metavar="slot", default=10)
parser.add_option("-g", "--gtx", type="int", dest="gtx",
		  help="GTX on the GLIB", metavar="gtx", default=0)
parser.add_option("-c", "--clksrc", type="string", dest="clksrc",
		  help="select the input for the XPoint1 outputs", metavar="clksrc", default="local")
parser.add_option("-r", "--reset", action="store_true", dest="resetCounters",
		  help="reset link error counters", metavar="resetCounters")
parser.add_option("-x", "--external", type="int", dest="trgSrc",
		  help="change trigger source", metavar="trgSrc")
parser.add_option("-b", "--sbitout", type="int", dest="sbitSrc",
		  help="use s-bit from VFAT <num>", metavar="sbitSrc")
parser.add_option("-d", "--debug", action="store_true", dest="debug",
		  help="print extra debugging information", metavar="debug")
parser.add_option("-t", "--ttc", type="int", dest="gemttc", default=2,
		  help="choose the TTC encoding (gem/csc=0, amc13=1,default=2 meaning no modification)", metavar="gemttc")
parser.add_option("-e", "--errors", type="int", dest="errorRate", default=1,
		  help="calculate link error rates for N seconds", metavar="errorRate")
parser.add_option("-u", "--user", action="store_true", dest="userOnly",
		  help="print user information only", metavar="userOnly")
parser.add_option("--testbeam", action="store_true", dest="testbeam",
		  help="fixed IP address for testbeam", metavar="testbeam")
parser.add_option("--daq_enable", type="int", dest="daq_enable",
		  help="enable daq output", metavar="daq_enable", default=-1)
parser.add_option("--rd", type="int", dest="reset_daq",
		  help="reset daq", metavar="reset_daq", default=-1)
parser.add_option("--ftx0", action="store_true", dest="flipPolarityTX0",
		  help="flip polarity TX 0", metavar="flipPolarityTX0")

(options, args) = parser.parse_args()

uhal.setLogLevelTo( uhal.LogLevel.FATAL )

uTCAslot = 170
if options.slot:
	uTCAslot = 160+options.slot
print options.slot, uTCAslot
ipaddr = '192.168.0.%d'%(uTCAslot)
#ipaddr = '192.168.250.53'
if options.testbeam:
        ipaddr        = '137.138.115.185'
address_table = "file://${GEM_ADDRESS_TABLE_PATH}/glib_address_table.xml"
uri = "chtcp-2.0://localhost:10203?target=%s:50001"%(ipaddr)
#uri = "ipbustcp-2.0://eagle45:60002"
glib  = uhal.getDevice( "glib" , uri, address_table )

########################################
# IP address
########################################
print
print "--=======================================--"
print "  Opening GLIB with IP", ipaddr
print "--=======================================--"
print

if not options.userOnly:
  #pass
	getSystemInfo(glib)
print
print "--=======================================--"
print "-> DAQ INFORMATION"
print "--=======================================--"
print

if (options.daq_enable>=0):
        writeRegister(glib, "GLIB.DAQ.CONTROL.DAQ_ENABLE", 0x1)
        writeRegister(glib, "GLIB.DAQ.CONTROL.TTS_OVERRIDE", 0x8)
        writeRegister(glib, "GLIB.DAQ.CONTROL.INPUT_KILL_MASK", 0x1)
        writeRegister(glib, "GLIB.DAQ.EXT_CONTROL.INPUT_TIMEOUT", 0x30D40)
        print "Reset daq_enable: %i"%(options.daq_enable)

print "-> DAQ control reg    :0x%08x"%(readRegister(glib,"GLIB.DAQ.CONTROL"))
print "-> DAQ status reg     :0x%08x"%(readRegister(glib,"GLIB.DAQ.STATUS"))
print "-> DAQ L1A ID         :0x%08x"%(readRegister(glib,"GLIB.DAQ.EXT_STATUS.L1AID"))
print "-> DAQ sent events cnt:0x%08x"%(readRegister(glib,"GLIB.DAQ.EXT_STATUS.EVT_SENT"))
print
print "-> DAQ DAV_TIMEOUT  :0x%08x"%(readRegister(glib,"GLIB.DAQ.CONTROL.DAV_TIMEOUT"))
print "-> DAQ INPUT_TIMEOUT:0x%08x"%(readRegister(glib,"GLIB.DAQ.EXT_CONTROL.INPUT_TIMEOUT"))
print "-> DAQ RUN_TYPE     :0x%08x"%(readRegister(glib,"GLIB.DAQ.EXT_CONTROL.RUN_TYPE"))
print "-> DAQ RUN_PARAMS   :0x%08x"%(readRegister(glib,"GLIB.DAQ.EXT_CONTROL.RUN_PARAMS"))
print
print "-> DAQ GTX NOT_IN_TABLE error counter:0x%08x"%(readRegister(glib,"GLIB.DAQ.EXT_STATUS.NOTINTABLE_ERR"))
print "-> DAQ GTX dispersion error counter  :0x%08x"%(readRegister(glib,"GLIB.DAQ.EXT_STATUS.DISPER_ERR"))
print
print "-> GLIB MAX_DAV_TIMER :0x%08x"%(readRegister(glib,"GLIB.DAQ.EXT_STATUS.MAX_DAV_TIMER"))
print "-> GLIB LAST_DAV_TIMER:0x%08x"%(readRegister(glib,"GLIB.DAQ.EXT_STATUS.LAST_DAV_TIMER"))
if options.gemttc in [0,1]:
        writeRegister(glib,"GLIB.TTC.CONTROL.GEMFORMAT",options.gemttc)
print "-> TTC Control :0x%08x"%(readRegister(glib,"GLIB.TTC.CONTROL"))
print "-> TTC Spy     :0x%08x"%(readRegister(glib,"GLIB.TTC.SPY"))

NGTX = 2
print "--=======================================--"
print "-> DAQ GTX INFO"
print "--=======================================--"
for olink in range(NGTX):
        print "-------------------------================--"
        print "----------> DAQ GTX%d INFO <---------------"%(olink)
        print "-------------------------================--"
        print "---------> DAQ GTX%d status                      :0x%08x"%(olink,readRegister(glib,"GLIB.DAQ.GTX%d.STATUS"%(olink)))
        print "---------> DAQ GTX%d corrupted VFAT block counter:0x%08x"%(olink,readRegister(glib,"GLIB.DAQ.GTX%d.COUNTERS.CORRUPT_VFAT_BLK_CNT"%(olink)))
        print "---------> DAQ GTX%d evn                         :0x%08x"%(olink,readRegister(glib,"GLIB.DAQ.GTX%d.COUNTERS.EVN"%(olink)))
        print
        print "---------> DAQ GTX%d MAX_DAV_TIMER :0x%08x"%(olink,readRegister(glib,"GLIB.DAQ.GTX%d.COUNTERS.MAX_DAV_TIMER" %(olink)))
        print "---------> DAQ GTX%d LAST_DAV_TIMER:0x%08x"%(olink,readRegister(glib,"GLIB.DAQ.GTX%d.COUNTERS.LAST_DAV_TIMER"%(olink)))
        print "---------> DAQ GTX%d DAV_TIMEOUT   :0x%08x"%(olink,readRegister(glib,"GLIB.DAQ.GTX%d.CONTROL.DAV_TIMEOUT"%(olink)))
        print "-------------------------================--"
        print "-------------> DEBUG INFO <----------------"
        print "-------------------------================--"
        dbgWords = readBlock(glib,"GLIB.DAQ.GTX%d.LASTBLOCK"%(olink),7)
        for word in dbgWords:
                print "-> DAQ GTX%d debug:0x%08x"%(olink,word)
                
print
print "--=======================================--"
print "-> SSSSSSSSSSSSSSSSSSSSSSBITSSSSSSSSSSSSSS"
print "--=======================================--"
print "-> SBIT_RATE:0x%08x"%(readRegister(glib,"GLIB.DAQ.SBIT_RATE"))
print
for olink in range(NGTX):
        print "-> DAQ GTX%d clusters 01:0x%08x"%(olink,readRegister(glib,"GLIB.DAQ.GTX%d_CLUSTER_01"%(olink)))
        print "-> DAQ GTX%d clusters 23:0x%08x"%(olink,readRegister(glib,"GLIB.DAQ.GTX%d_CLUSTER_23"%(olink)))
        print


print
print "--=======================================--"
print "-> BOARD USER INFORMATION"
print "--=======================================--"
print

if (options.resetCounters):
        glibCounters(glib,options.gtx,True)
        writeRegister(glib,"GLIB.DAQ.CONTROL.DAQ_LINK_RESET",0x1)
        writeRegister(glib,"GLIB.DAQ.CONTROL.DAQ_LINK_RESET",0x0)
print

if (options.flipPolarityTX0):
        tx0polarity = readRegister(glib,"GLIB.IPBus_System_Control.TX_polarity")
        if tx0polarity == 0x0:
          print "TX0 polarity %s" %(tx0polarity)
          writeRegister(glib,"GLIB.IPBus_System_Control.TX_polarity",0x1)
        else:
          print "TX0 polarity %s" %(tx0polarity)
          writeRegister(glib,"GLIB.IPBus_System_Control.TX_polarity",0x0)
sys.stdout.flush()
for olink in range(NGTX):
        print "--=====GTX%d==============================--"%(olink)
        errorCounts = []
        SAMPLE_TIME = 1.
        for trial in range(options.errorRate):
                errorCounts.append(calculateLinkErrors(True,glib,olink,SAMPLE_TIME))
        sys.stdout.flush()
        
        rates = errorRate(errorCounts,SAMPLE_TIME)
        print "-> TRK: 0x%08x  (%6.2f%1sHz)"%(rates["TRK"][0],rates["TRK"][1],rates["TRK"][2])
        print "-> TRG: 0x%08x  (%6.2f%1sHz)"%(rates["TRG"][0],rates["TRG"][1],rates["TRG"][2])
        print 
        
        sys.stdout.flush()
        print "-> Counters    %8s     %8s     %8s     %8s"%("L1A","Cal","Resync","BC0")
        counters = glibCounters(glib,olink)
        print "   %8s  0x%08x   0x%08x   0x%08x   0x%08x"%(
                "",
                counters["T1"]["L1A"],
                counters["T1"]["CalPulse"],
                counters["T1"]["Resync"],
                counters["T1"]["BC0"])
print "--=======================================--"
print "-> VCAL %x"%(readRegister(glib,"GLIB.OptoHybrid_0.OptoHybrid.GEB.VFATS.VFAT6.VCal"))
sys.stdout.flush()
