require 'spec_helper'

describe FuzzBall do

  before(:all) do
    @fuzz = FuzzBall.new(["aaa", "bbb"])
    @aaa_array = "aaa".unpack("U*")
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
    it "should only search the candidate strings that have the highest duples" do
      @fuzz.should_receive(:decimate_strings!).once.with( @aaa_array ).and_return(@aaa_array)
      @fuzz.search("aaa")
    end
  end

  describe "#decimate_strings!" do
    it "should remove strings that don't have a high duple count" do
      @fuzz.send(:decimate_strings!, @aaa_array).should == [@aaa_array]
    end
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

    describe "#smith_waterman" do
      it "should align the two strings" do 
        @fuzz.send(:smith_waterman, [1,2,3], [2,3,4])
        @fuzz.instance_eval { @curr_alignment }.should == [2, 1, 1, 0]
        @fuzz.instance_eval { @curr_score }.should == 2.0
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
