# command_t.rb
#
# This is a simple script demonstrating some of FuzzBall's
# string searching ability wthin the context of a "command-t"-esque
# file searching utility. All you have to do is start typing, and
# a list of matches will be displayed as you type. Currently, the
# file list that this demonstration pulls from is the file list of
# the Linux 2.6 distro (~37,083 files).
require File.join(File.dirname(__FILE__), 'support')

# Load the files, then instantiate a new FuzzBall
# instance from the files
files   = File.open("filelist", "r")  {|f| f.readlines}.collect(&:chomp)
n_files = files.count
fuzz    = FuzzBall::Searcher.new( files )

clear_screen!
needle = ""

# Primary program loop; basically listen for next keyboard input,
# build the needle string from the input, then search the file list
# for matches
loop {

  char = get_character

  # If the character is a backspace, pop off the last character, 
  # otherwise, append to the needle
  if (char == "\u007F")
    needle = needle[0..-2]
  else
    needle += char
  end

  clear_screen!
  print_header( needle )

  # The heart of the search. Time the search in miliseconds
  t       = Time.now
  results = fuzz.search(needle, :limit => 15, :order => :descending)
  elapsed = 1000 * (Time.now - t)

  print_results( needle, results, elapsed, n_files )
}
