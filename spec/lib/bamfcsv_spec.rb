require 'spec_helper'

describe BAMFCSV do
  it "has a read method" do
    BAMFCSV.should respond_to(:read)
  end

  it "has a parse method" do
    BAMFCSV.should respond_to(:parse)
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
      BAMFCSV.read("spec/fixtures/bamf-comma-comma.csv").should == [["BAMF",nil,"CSV"]]
    end

    it "escapes cells that are quoted" do
      BAMFCSV.read("spec/fixtures/escapes.csv").should == [["this is a semicolon:", " ;"],["this is a comma:", " ,"]]
    end

    it "escapes cells that are quoted" do
      BAMFCSV.read("spec/fixtures/double-quotes.csv").should == [["this is a semicolon:", " ;"], ["this is a comma:", " ,"], ["this is a quote:", " \""]]
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

  describe "#parse" do
    it "correctly parses the last cell even if there is no newline" do
      BAMFCSV.parse("1,2").should == [["1","2"]]
    end

    it 'correctly escaptes ""' do
      BAMFCSV.parse("1,\"\"2\"\"\n").should == [["1", '"2"']]
    end

    it "parses unquoted empty cells as nil" do
      BAMFCSV.parse("1,,2").should == [["1",nil,"2"]]
    end

    it 'parses quoted empty cells as ""' do
      BAMFCSV.parse("1,\"\",2").should == [["1","","2"]]
    end

    describe "default CSV module compatibility" do
      it "adds a nil cell after a trailing comma with no newline" do
        BAMFCSV.parse("1,2,").should == [["1","2",nil]]
      end

      it "adds a nil cell after a trailing comma with an ending newline" do
        BAMFCSV.parse("1,2,\n").should == [["1","2",nil]]
      end

      it "raises BAMFCSV::MalformedCSVError when quotes appear in a cell which was not started with quotes" do
        expect { BAMFCSV.parse(' ""') }.should raise_error(BAMFCSV::MalformedCSVError)
      end

      it "raises BAMFCSV::MalformedCSVError when a quoted cell is not closed at its end"
      it "raises BAMFCSV::MalformedCSVError when quoted cell is closed before its end"
    end
  end
end
