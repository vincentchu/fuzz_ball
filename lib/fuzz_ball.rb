require File.join(File.dirname(__FILE__), '../ext/fuzz_ball/count_duples')

class FuzzBall


  include CountDuples

  attr_reader :files, :files_array

  def initialize(files)
    @files = files
    @files_array = files.collect {|f| f.unpack("U*")}
  end

  def search(term)

  end

  private

  def str2arr( str )
    str.unpack("U*")
  end

  def arr2str( arr )
    arr.pack("U*")
  end

  def winnow!(term)

  end

end
