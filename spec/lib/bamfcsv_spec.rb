require 'spec_helper'

describe BAMFCSV do
  it "has a parse method" do
    BAMFCSV.should respond_to(:parse)
  end

  describe "#parse" do
    it "is a matrix given a filename" do
      BAMFCSV.parse("spec/fixtures/test.csv").should be_instance_of Array
    end
  end
end
