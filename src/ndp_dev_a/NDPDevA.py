from m5.params import *
from m5.proxy import *
from m5.objects.NDP import NDP

class NDPDevA(NDP):
	type = 'NDPDevA'
	cxx_header = "ndp_dev_a/ndp_dev_a.hh"
	cxx_class = 'gem5::NDPDevA'