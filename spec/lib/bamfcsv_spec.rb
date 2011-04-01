require 'spec_helper'

describe BAMFCSV do
  it "has a parse method" do
    BAMFCSV.should respond_to(:read)
  end

  describe "#read" do
    it "is a matrix given a filename" do
      BAMFCSV.read("spec/fixtures/test.csv").should be_instance_of Array
    end

    it "is an empty array passed an empty file" do
      BAMFCSV.read("spec/fixtures/empty.csv").should == []
    end

    it "is a 1xn matrix with a one column csv file" do
      BAMFCSV.read("spec/fixtures/one-column.csv").should == [["BAMF"],["CSV"]]
    end

    it "interprets empty cells correctly" do
      BAMFCSV.read("spec/fixtures/bamf-comma-comma.csv").should == [["BAMF","","CSV"]]
    end

    it "escapes cells that are quoted" do
      BAMFCSV.read("spec/fixtures/escapes.csv").should == [["this is a semicolon:", " ;"],["this is a comma:", " ,"]]
    end

    it "escapes cells that are quoted" do
      BAMFCSV.read("spec/fixtures/double-quotes.csv").should == [["this is a semicolon:", " ;"], ["this is a comma:", " ,"], ["this is quote:", " \""]]
    end

    it "doesn't create a row when the file terminates with [CR]LF" do
      BAMFCSV.read("spec/fixtures/terminated-with-cr.csv").should == [["a"],["b"]]
    end

    it "raises Errno::ENOENT when the file does not exist" do
      expect do
        BAMFCSV.read("spec/fixtures/this-file-does-not-not-exist.csv")
      end.should raise_error Errno::ENOENT
    end

    it "raises Errno::EISDIR when the file is not a flat file" do
      expect do
        BAMFCSV.read("spec/fixtures/")
      end.should raise_error Errno::EISDIR
    end
  end
end
