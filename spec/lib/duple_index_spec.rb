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

  describe "#match" do
    before(:each) do
      [
        [1, 2, 3, 4],
        [1, 2, 3],
        [1, 2, 4],
        [4, 5],
        [10, 11, 10, 11]
      ].each_with_index do |string, i|
        @index.add(i, string)
      end
    end

    it "should not attempt to match with one or fewer duples in the candidate" do
      @index.match([1]).should == {}
    end

    it "should return strings by score" do
      @index.match([1, 2, 3]).should == {3 => [1, 0], 1 => [2]}
    end

    it "shouldn't be fooled by the appearance of multiple duples in a string" do
      @index.match([10, 11]).should == {2 => [4]}
    end
  end
end
