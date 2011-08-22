require File.join(File.dirname(__FILE__), '../ext/fuzz_ball/array_methods')

class FuzzBall

  include ArrayMethods

  attr_reader :files, :files_array

  def initialize(files)
    @files = files
    @files_array = files.collect {|f| str2arr(f)}

    @curr_alignment = []
    @curr_score     = 0.0
  end

  def search(needle, opts = {})

    needle_ary = str2arr(needle)
    results  = []

    return results if (needle.length < 2)

    decimate_strings!( needle_ary ).each do |candidate|
      smith_waterman(needle_ary, candidate)

      results << {:alignment => @curr_alignment, :score => @curr_score, :string => candidate.pack("U*")}
    end

    if (opts[:order] == :descending)
      results.sort! {|a,b| b[:score] <=> a[:score]}
    else
      results.sort! {|a,b| a[:score] <=> b[:score]}
    end

    results = results.first(opts[:limit]) if opts[:limit].is_a?(Fixnum)

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
