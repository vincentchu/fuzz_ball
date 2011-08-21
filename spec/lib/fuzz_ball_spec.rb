require 'spec_helper'

describe FuzzBall do

  describe "#initialize" do
    before(:all) do
      @fuzz = FuzzBall.new(["aaa", "bbb"])
    end

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
    before(:all) do
      @fuzz = FuzzBall.new( %w(aaa bbb) )
    end

    describe "#count_duples" do
      it "should return the number of continuous duples in a string" do
        puts @fuzz.count_duples([1]).inspect

      end
    end
  end

  describe "String utilities" do
    before(:all) do
      @fuzz = FuzzBall.new( %w(aaa bbb) )
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
