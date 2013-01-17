import tempfile
import subprocess
import collections

def read_fuses(target, source, env):
	"""Executes avrdude to read fuse bits from MCU.
	base_command should contain avrdude with required arguments (`avrdude -p <part> -c <programmer>`)
	"""

	fuse_list = ('lfuse','hfuse','efuse')
	fuses = collections.OrderedDict()
	for f in fuse_list:
		fuses[f] = {'file': tempfile.NamedTemporaryFile(), 'value': '-'}

	arg_list = ["-U %s:r:%s:h" % (k, v['file'].name) for k, v  in fuses.items()]
	fuse_read_command = "%s -qq %s" % (env['AVRDUDE_BASE'], ' '.join(arg_list))


	return_code = subprocess.call(fuse_read_command, shell=True)
	if return_code == 0:
		for fuse in fuses.keys():
			print "%s: %s" % (fuse, fuses[fuse]['file'].read().strip())

		return False
	else:
		print "Failed to read fuses"
		return True

def write_fuses(target, source, env):
	fuse_write_command = "%(command)s -qq -u -U lfuse:w:%(lfuse)s:m -U hfuse:w:%(hfuse)s:m -U efuse:w:%(efuse)s:m" % \
						{'command': env["AVRDUDE_BASE"], 'lfuse': env["LFUSE"], 
						'hfuse':env["HFUSE"],'efuse':env["EFUSE"]}

	return_code = subprocess.call(fuse_write_command, shell=True)
	if return_code == 0:
		print "Fuses written successfully"
		return False
	else:
		print "Failed to write fuses"
		return True

def upload_hex(target, source, env):

	upload_command = "%s -q -U flash:w:%s" % (env['AVRDUDE_BASE'], source[0].path)
	return subprocess.call(upload_command, shell=True)


if __name__=='__main__':
	read_fuses('avrdude -c usbasp -p atmega328p')