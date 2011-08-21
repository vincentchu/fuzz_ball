class FuzzBall

  attr_reader :files

  def initialize(files)
    @files = files
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
