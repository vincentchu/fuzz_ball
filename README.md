# fuzz_ball

FuzzBall is a gem that finds fuzzy matches of a string (the 'needle') within an array of strings (the 'haystack'). It does so via a two-step process: first, it finds candidate strings from the haystack that have high similarity to the needle, then uses a Smith-Waterman algorithm to fuzzily match from these candidates. Strings are returned along with a matching score. Both steps of the search are written in C for greater performance.

# Usage

    require 'fuzz_ball'
    ruby-1.9.2-p290 :001 > words = %Q[Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.].split(/\s+/)
    
    ruby-1.9.2-p290 :002 > fuzz = FuzzBall::Searcher.new( words )
    => <FuzzBall::Searcher n_files=19>
    
    ruby-1.9.2-p290 :003 > fuzz.search("lor")
    => [{:alignment=>[2, 4, 1, 3, 0, 2], :score=>0.6666666666666666, :string=>"dolore"}, {:alignment=>[2, 4, 1, 3, 0, 2], :score=>0.8, :string=>"dolor"}]
    
    ruby-1.9.2-p290 :004 > fuzz.add("a_new_string")
    => true
    
    ruby-1.9.2-p290 :005 > fuzz.search("new_string")
    => [{:alignment=>[9, 11, 8, 10, 7, 9, 6, 8, 5, 7, 4, 6, 3, 5, 2, 4, 1, 3, 0, 2], :score=>1.5, :string=>"a_new_string"}]
	
# Algorithms

FuzzBall uses a two-step process. When a search is executed, the number of haystack strings that must be searched is reduced by first eliminating haystack strings that are not similar to the needle string. Similarity is determined by a high number of shared duples between the needle string and a haystack string (in the appropriate order). This metric is computed by pre-indexing the duples in the haystack strings.

These candidate strings are then fuzzily matched using the Smith-Waterman algorithm for performaing local sequence alignment.

## Duple Indexing

When a new instance of `FuzzBall::Searcher` is instantiated, each haystack string is decomposed into duples. The position of each duple is indexed internally by the C implementation and stored in a lightweight hash data structure, allowing fast (_O(1)_) lookups of the haystack strings that contain a given duple. The C implementation uses [uthash](http://uthash.sourceforge.net). See `DupleIndex.c` in `ext/duple_index`.

## Smith-Waterman

The [Smith-Waterman](http://en.wikipedia.org/wiki/Smith%E2%80%93Waterman_algorithm) algorithm is a dynamic programming algorithm that performs local sequence alighment by comparing segments of all possible lengths. It is commonly used to align protein or nucleotide sequences and is heavily used within the computational biology communinity. However it works just as well on normal strings.

# Dependencies

The C extension makes use of [uthash](http://uthash.sourceforge.net/), a C hash implementation written by Troy D. Hanson under a modified BSD license.

# Author

FuzzBall was written by Vincent Chu (vincentchu [at] gmail.com).
