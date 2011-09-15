require 'spec_helper'

describe FuzzBall::SmithWaterman do

	before(:each) do
		@smith = FuzzBall::SmithWaterman.new([1,2,3], [2,3,4])
	end

	it "should align the two arrays and return alignment" do
		@smith.alignment.should == [2, 1, 1, 0]
	end

	it "should return the score" do
		@smith.score.should == 2.0
	end
end
