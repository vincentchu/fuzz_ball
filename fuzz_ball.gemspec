Gem::Specification.new do |s|
  s.name        = 'fuzz_ball'
  s.version     = '0.9.1'
  s.summary     = "Fast fuzzy string matching from an array of strings using an implementation of Smith-Waterman."
  s.description = "FuzzBall is a gem that finds fuzzy matches of a string (the 'needle') within an array of strings (the 'haystack'). It does so via a two-step process: first, it finds candidate strings from the haystack that have high similarity to the needle, then uses a Smith-Waterman algorithm to fuzzily match from these candidates. Strings are returned along with a matching score. Both steps of the search are written in C for greater performance."
  s.authors     = ["Vincent Chu"]
  s.email       = "vincentchu [at] gmail.com"
  s.files       = Dir.glob("lib/**/*.rb") + Dir.glob("ext/**/*.{c,h,rb}")
  s.homepage    = "http://github.com/vincentchu/fuzz_ball"
	s.extensions  = ["ext/fuzz_ball/smith_waterman/extconf.rb", "ext/fuzz_ball/duple_index/extconf.rb"]
end

