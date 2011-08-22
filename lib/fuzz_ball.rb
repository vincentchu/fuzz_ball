require File.join(File.dirname(__FILE__), '../ext/fuzz_ball/array_methods')
require 'fuzz_ball/string_align'

class FuzzBall

  include ArrayMethods
  include StringAlign

  attr_reader :files, :files_array

  def initialize(files)
    @files = files
    @files_array = files.collect {|f| str2arr(f)}

    @curr_alignment = []
    @curr_score     = 0.0
  end

  def search(term)

    term_ary = str2arr(term)
    results  = []

    decimate_strings!( term_ary ).each do |candidate|
      smith_waterman(term_ary, candidate)

      results << {:alignment => @curr_alignment, :score => @curr_score, :string => candidate.pack("U*")}
    end

    results.sort_by! {|r| r[:score]}

    results
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
