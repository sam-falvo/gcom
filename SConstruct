env = Environment()

# REPLACE THESE WITH SOME CONFIGURE OPTIONS!!
env['INCDIRS'] = "#/include"
env['LIBGCOM'] = "gcom"
env['MAX_PATH_LEN'] = '128'
env['MAX_REGKEY_LEN'] = '128'
env['REGPATH'] = '/gcom/registry'
env['PLATFORM'] = '__LINUX__'

Export('env')
SConscript('utilities/SConscript')
SConscript('libraries/SConscript')
SConscript('examples/SConscript')
