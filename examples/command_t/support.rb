$:.unshift File.join(File.dirname(__FILE__), "../../lib")

require 'rubygems'
require 'fuzz_ball'
require 'termios'

# This method enables the ability to capture a *single* character from
# STDIN. Surprisingly, this behavior doesn't seem to be possible in the
# standard ruby and requires installing the ruby-termios gem.
#
# Shamelessly adapted/stolen from Highline::SystemExtensions source
# http://highline.rubyforge.org/doc/classes/HighLine/SystemExtensions.html
def get_character( input = STDIN )
  old_settings = Termios.getattr(input)

  new_settings                     =  old_settings.dup
  new_settings.c_lflag             &= ~(Termios::ECHO | Termios::ICANON)
  new_settings.c_cc[Termios::VMIN] =  1

  byte = begin
    Termios.setattr(input, Termios::TCSANOW, new_settings)
    input.getbyte
  ensure
    Termios.setattr(input, Termios::TCSANOW, old_settings)
  end

  [byte].pack("U*")
end


def clear_screen!
  print "\e[2J\e[f"
end

def print_at_position(i, j, line)
  print "\e[#{i};#{j}H#{line}"
end

def print_header(needle)
  print_at_position(1, 1, "Search: #{needle}")
  print_at_position(2, 1, "-" * 100)
  print_at_position(20, 1, "Powered by FuzzBall - Written by vincentchu (c) 2011")
end

def print_results(needle, results, elapsed, count)
  print_at_position(1, 68, sprintf("Files: %6d Elapsed: %7.2f ms", count, elapsed))

  results.each_with_index do |result, i|
    print_at_position(i+3, 1, sprintf("%6.2f - %s", result[:score], result[:string]))
  end
end

