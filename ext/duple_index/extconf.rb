require 'mkmf'

%w(duple_index).each do |extension_name|
	dir_config(extension_name)
	create_makefile(extension_name)
end

