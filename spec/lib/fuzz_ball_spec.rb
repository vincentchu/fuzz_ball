require 'spec_helper'

describe FuzzBall do

  describe "#initialize" do
    it "should initialize with a list of files" do
      @fuzz = FuzzBall.new(["aaa", "bbb"])
      @fuzz.files.should == ["aaa", "bbb"] 
    end
  end

  describe "#search" do
    it "should allow you to search for a file within the list"
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
