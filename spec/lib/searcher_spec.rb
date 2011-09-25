require 'spec_helper'

describe FuzzBall::Searcher do

  before(:all) do
    @fuzz = FuzzBall::Searcher.new(["aaa", "bbb"])
    @aaa_array = "aaa".unpack("U*")
    @bbb_array = "bbb".unpack("U*")
  end

  describe "#initialize" do
    it "should initialize with a list of files" do
      @fuzz.files.should == ["aaa", "bbb"]
    end

    it "should convert the list of files into their array representations" do
      @fuzz.files_array.should == [@aaa_array, @bbb_array]
    end

    it "should weed out characters that are designated to be ignored" do
      fuzz = FuzzBall::Searcher.new(["path/to/file.txt"], :ignore => %w(. /))
      fuzz.files_array.should == ["pathtofiletxt".unpack("U*")]
    end

		it "should properly instantiate a duple index" do
			@duple_mock = mock('duple_index')
			FuzzBall::DupleIndex.should_receive(:new).once.and_return(@duple_mock)
			@duple_mock.should_receive(:add).once

			FuzzBall::Searcher.new(["aaa"])
		end
  end

	describe "#decimate_strings!" do
		before(:each) do 
			@duple_index = mock('duple_index')
			@fuzz.stub!(:duple_index).and_return(@duple_index)
		end

		 it "should use the duple index to find weed out non-matching strings" do
			 @duple_index.should_receive(:match).once.with(
				 @aaa_array, :by_score => true
       ).and_return({2 => [0]})

       @fuzz.send(:decimate_strings!, @aaa_array).should == [@aaa_array]
		end
	end

  describe "#search" do
    before(:each) do
      @smith_waterman_mock = mock('smith_waterman')
    end

    it "should be fine when presented with malformed input (e.g., blank string)" do
      lambda {
        @fuzz.search("")
      }.should_not raise_exception
    end

    it "should only search the candidate strings that have the highest duples" do
      @fuzz.should_receive(:decimate_strings!).once.with( @aaa_array ).and_return([@aaa_array])
      FuzzBall::SmithWaterman.should_receive(:new).once.with(@aaa_array, @aaa_array).and_return( @smith_waterman_mock )
      @smith_waterman_mock.should_receive(:alignment).once.and_return([1, 2, 3])
      @smith_waterman_mock.should_receive(:score).once.and_return(2.0)

      @fuzz.search("aaa")
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
