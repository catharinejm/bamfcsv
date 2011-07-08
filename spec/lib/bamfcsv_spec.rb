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
      BAMFCSV.parse(%Q|1,"""2"""\n|).should == [["1", '"2"']]
    end

    it "parses unquoted empty cells as nil" do
      BAMFCSV.parse("1,,2").should == [["1",nil,"2"]]
    end

    it 'parses quoted empty cells as ""' do
      BAMFCSV.parse("1,\"\",2").should == [["1","","2"]]
    end

    it "parses a single cell not followed by a newline correctly" do
      BAMFCSV.parse("1").should == [["1"]]
      BAMFCSV.parse("1\n2").should == [["1"],["2"]]
      BAMFCSV.parse("1\r\n2").should == [["1"],["2"]]
    end

    describe "default CSV module compatibility" do
      it "adds a nil cell after a trailing comma with no newline" do
        BAMFCSV.parse("1,2,").should == [["1","2",nil]]
      end

      it "adds a nil cell after a trailing comma with an ending newline" do
        BAMFCSV.parse("1,2,\n").should == [["1","2",nil]]
      end

      describe "when a quoted cell ends a line" do
        it "does not raise an exception" do
          expect { BAMFCSV.parse(%Q|1,2,"3,4"\n5,6,7|) }.should_not raise_error
          expect { BAMFCSV.parse(%Q|1,2,"3,4"\r\n5,6,7|) }.should_not raise_error
        end

        it "correctly parses a quoted cell at the end of a line" do
          BAMFCSV.parse(%Q|1,2,"3,4"\n5,6,7|).should == [["1","2","3,4"],["5","6","7"]]
          BAMFCSV.parse(%Q|1,2,"3,4"\r\n5,6,7|).should == [["1","2","3,4"],["5","6","7"]]
        end
      end

      it "raises BAMFCSV::MalformedCSVError when quotes appear in a cell which was not started with quotes" do
        expect { BAMFCSV.parse(' ""') }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse(" \"\"\n") }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse(" \"\"\r\n") }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse('1, "",3') }.should raise_error(BAMFCSV::MalformedCSVError)
      end

      it "raises BAMFCSV::MalformedCSVError when a quoted cell is not closed at its end" do
        expect { BAMFCSV.parse('"') }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse('" ""') }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse("\"\n") }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse("\"\r\n") }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse("\" \"\"\n") }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse("\" \"\"\r\n") }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse('1,"2,3') }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse("1,\"2,3\n") }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse("1,\"2,3\r\n") }.should raise_error(BAMFCSV::MalformedCSVError)
      end

      it "raises BAMFCSV::MalformedCSVError when quoted cell is closed before its end" do
        expect { BAMFCSV.parse('"" ') }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse("\"\" \n") }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse("\"\" \r\n") }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse('1,"" ,2') }.should raise_error(BAMFCSV::MalformedCSVError)
      end

      it "raises BAMFCSV::MalformedCSVError when unescaped quotes appear in a quoted cell" do
        expect { BAMFCSV.parse('"a"b"c"') }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse('"a"b"c",2') }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse(%Q("a"b"c"\n)) }.should raise_error(BAMFCSV::MalformedCSVError)
        expect { BAMFCSV.parse(%Q("a"b"c"\r\n)) }.should raise_error(BAMFCSV::MalformedCSVError)
      end

      ['', "\n", "\r\n", ",", "\n\n", "\r\n\r\n"].each do |csv|
        csv_result = CSV.parse(csv)
        it "BAMFCSV parses #{csv.inspect} the same way as CSV (#{csv_result.inspect})" do
          BAMFCSV.parse(csv).should == csv_result
        end
      end
    end
  end

  describe "generating a Table" do
    describe "identity" do
      let(:table) { BAMFCSV.parse("foo,bar\n1,2", :headers => true) }
      it "Table isa Array" do
        table.kind_of?(Array).should be_true
        table.is_a?(Array).should be_true
      end
      
      it "Array === table" do
        pending { (Array === table).should be_true }
      end

      it "Table::Row isa Hash" do
        row = table.first
        row.kind_of?(Hash).should be_true
        row.is_a?(Hash).should be_true
      end

      it "Hash === row" do
        pending { (Hash === row).should be_true }
      end
    end

    describe "with only a header" do
      let(:header_only) { BAMFCSV.parse("1,2,3", :headers => true) }
      it "has no body rows" do
        header_only.first.should be_nil
      end

      it "does nothing when iterating" do
        expect { header_only.each { |x| raise "Oh dang!" } }.should_not raise_error
      end
    end

    describe "with body rows" do
      it "maps the headers the the values of each row" do
        table = BAMFCSV.parse("a,b,c\r\n1,2,3\r\nx,y,z", :headers => true)
        table.first["a"].should == "1"
        table[1]["c"].should == "z"
      end

      it "knows the headers" do
        table = BAMFCSV.parse("a,b,c\r\n1,2,3\r\nx,y,z", :headers => true)
        row = table.first
        row.headers.should == ['a', 'b', 'c']
      end

      it "knows the fields" do
        table = BAMFCSV.parse("a,b,c\r\n1,2,3\r\nx,y,z", :headers => true)
        row = table.first
        row.fields.should == ['1', '2', '3']
      end
    end

    describe "Table#each" do
      let(:table) { BAMFCSV.parse("a,b\n1,2", :headers => true) }
      it "Table#each returns self" do
        table.each { |r| ;}.should == table
      end

      it "does not require a block" do
        expect { table.each }.should_not raise_error(LocalJumpError)
      end
    end

    describe "Table#inspect" do
      it "is an Array of Hashes" do
        csv = <<CSV
foo,bar
1,2
baz,quux
CSV

        inspected = '[{"foo" => "1", "bar" => "2"}, {"foo" => "baz", "bar" => "quux"}]'

        BAMFCSV.parse(csv, :headers => true).inspect.should == inspected
      end
    end

    describe "Table#empty?" do
      it "returns true for an empty table with headers" do
        table = BAMFCSV.parse("column1,column2\n", :headers => true)
        table.empty?.should be_true
      end

      it "returns false for a non-empty table with headers" do
        table = BAMFCSV.parse("column1,column2\nfoo,bar", :headers => true)
        table.empty?.should be_false
      end

      it "returns true for an empty table without headers" do
        table = BAMFCSV.parse("", :headers => false)
        table.empty?.should be_true
      end

      it "returns false for a non-empty table without headers" do
        table = BAMFCSV.parse("foo,bar", :headers => false)
        table.empty?.should be_false
      end
    end

    describe "Table::Row#inspect" do
      it "is a Hash" do
        csv = <<CSV
foo,bar
1,2
baz,quux
CSV

        inspected = '{"foo" => "1", "bar" => "2"}'

        BAMFCSV.parse(csv, :headers => true).first.inspect.should == inspected
      end
    end
  end
end
