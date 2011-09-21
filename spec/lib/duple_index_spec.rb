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
      @index.add(2, [3, 4, 3, 4, 3, 4])
    end

    it "should tell you where a given duple is found" do
      @index.query(1, 2).should == [[1, 1], [0, 0]]
    end
  end
end
