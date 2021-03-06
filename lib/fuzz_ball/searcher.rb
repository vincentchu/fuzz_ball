module FuzzBall
  class Searcher

    DECIMATE_LIMIT = 10

    attr_reader :files, :files_array, :options, :duple_index

    def initialize(files, opts = {})
      @options     = opts
      @files       = files
      @files_array = files.collect {|f| str2arr(f)}

      index_duples!
    end

    def add( str )
      str_arr = str2arr( str )

      files       << str
      files_array << str_arr

      duple_index.add( files_array.count - 1, str_arr )

      true
    end

    def search(needle, opts = {})

      needle_ary = str2arr(needle)
      results  = []

      return results if (needle.length < 2)

      decimate_strings!( needle_ary ).each do |candidate|
        smith = SmithWaterman.new(needle_ary, candidate)

        results << {
          :alignment => smith.alignment,
          :score     => (smith.score - 0.01*candidate.length), # penalize against longer strings
          :string    => candidate.pack("U*")
        }
      end

      if (opts[:order] == :descending)
        results.sort! {|a,b| b[:score] <=> a[:score]}
      else
        results.sort! {|a,b| a[:score] <=> b[:score]}
      end

      results = results.first(opts[:limit]) if opts[:limit].is_a?(Fixnum)

      results
    end

    def inspect
      %Q[<FuzzBall::Searcher n_files=#{files_array.count}>]
    end

    private

    def index_duples!
      @duple_index = DupleIndex.new
      files_array.each_with_index do |str, index|
        duple_index.add(index, str)
      end
    end

    def decimate_strings!(needle)
      matches_by_score = duple_index.match(needle)

      indices = []
      matches_by_score.keys.sort { |a,b| b <=> a}.each do |key|
        if (indices.length < DECIMATE_LIMIT)
          indices += matches_by_score[key]
        else
          break
        end
      end

      files_array.values_at(*indices)
    end

    def str2arr( str )
      if options[:ignore]

        regexp = options[:ignore].collect { |s|
          Regexp.escape(s)
        }.join("|")

        regexp = Regexp.new("(#{regexp})")
        str.gsub(regexp, "").unpack("U*")
      else
        str.unpack("U*")
      end
    end

    def arr2str( arr )
      arr.pack("U*")
    end
  end
end
