require File.join(File.dirname(__FILE__), '../ext/fuzz_ball/array_methods')
require 'fuzz_ball/string_align'

class FuzzBall

  include ArrayMethods
  include StringAlign

  attr_reader :files, :files_array

  def initialize(files)
    @files = files
    @files_array = files.collect {|f| str2arr(f)}

    @tmp_align = []
  end

  def search(term)

  end

  private

  def decimate_strings!(needle)
    max_duples = -1
    grouped_results = {}

    files_array.each do |haystack|
      n_duples   = count_duples(needle, haystack)
      max_duples = n_duples if (n_duples > max_duples)

      grouped_results[n_duples] ||= []
      grouped_results[n_duples] << haystack
    end

    grouped_results[max_duples]
  end

  def str2arr( str )
    str.unpack("U*")
  end

  def arr2str( arr )
    arr.pack("U*")
  end
end
