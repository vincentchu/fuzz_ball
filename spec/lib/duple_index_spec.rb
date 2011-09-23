require 'spec_helper'

describe FuzzBall::DupleIndex do
  before(:each) do
    @index = FuzzBall::DupleIndex.new
  end

  describe "#add" do
    it "should be able to add a string" do
      lambda {
        @index.add(0, [1, 2, 3]);
      }.should_not raise_exception
    end
  end

  describe "#query" do
    before(:each) do
      @index.add(0, [1, 2, 3]);
      @index.add(1, [0, 1, 2]);
    end

    it "should tell you where a given duple is found" do
      @index.query(1, 2).should == [[1, 1], [0, 0]]
    end
  end

  describe "#match" do
    before(:each) do
      @index.add(0, [1, 2, 3, 4])
      @index.add(1, [1, 2, 3])
      @index.add(2, [1, 2, 4])
      @index.add(3, [4, 5])
    end

    it "should not attempt to match with one or fewer duples in the candidate" do
      @index.match([1]).should == {}
    end

    it "should return strings that match candidate by score" do
      @index.match([1, 2, 3]).should == { 0 => 2, 1 => 2, 2 => 1 }
    end

    it "should return strings by score" do
      @index.match([1, 2, 3], :by_score => true).should == {2 => [1, 0], 1 => [2]}
    end
  end
end
