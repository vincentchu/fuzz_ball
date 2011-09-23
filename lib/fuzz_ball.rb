require File.join(File.dirname(__FILE__), '../ext/smith_waterman/smith_waterman')
require File.join(File.dirname(__FILE__), '../ext/duple_index/duple_index')
require File.join(File.dirname(__FILE__), 'fuzz_ball/duple_index')

module FuzzBall
  autoload :Searcher, 'fuzz_ball/searcher'
end
