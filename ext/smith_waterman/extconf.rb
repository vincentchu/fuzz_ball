require 'mkmf'

%w(smith_waterman).each do |extension_name|
	dir_config(extension_name)
	create_makefile(extension_name)
end

