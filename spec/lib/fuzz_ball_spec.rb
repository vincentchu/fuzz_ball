require 'spec_helper'

describe FuzzBall do

  before(:all) do
    @fuzz = FuzzBall.new(["aaa", "bbb"])
  end

  describe "#initialize" do
    it "should initialize with a list of files" do
      @fuzz.files.should == ["aaa", "bbb"] 
    end

    it "should convert the list of files into their array representations" do
      @fuzz.files_array.should == ["aaa".unpack("U*"), "bbb".unpack("U*")]
    end
  end

  describe "#search" do
    it "should allow you to search for a file within the list"
  end

  describe "Array utilities" do
    describe "#count_duples" do
      it "should return the number of continuous duples between two arrays" do
        [
          [[1,2], [1,3], 0],
          [[1,2,3], [5,6,1,2,3,4], 2],
          [[1,2], [1,2], 1],
          [[1,2,3], [1,2], 1]
        ].each do |test_case|
          @fuzz.send(:count_duples, test_case[0], test_case[1]).should == test_case[2]
        end
      end
    end

    describe "#decimate_strings!" do
      it "should remove strings that don't have a high duple count" do
        aaa_array = "aaa".unpack("U*")
        @fuzz.send(:decimate_strings!, aaa_array).should == [aaa_array]
      end
    end
  end

  describe "String utilities" do
    before(:all) do
      @str  = "lorem ipsum dolor"
      @arr  = @str.unpack("U*")
    end

    describe "#str2arr" do
      it "should convert a string to an array of ints" do
        @fuzz.send(:str2arr, @str).should == @arr
      end
    end

    describe "#arr2str" do
      it "should convert an array of ints to its string" do
        @fuzz.send(:arr2str, @arr).should == @str
      end
    end
  end
end
