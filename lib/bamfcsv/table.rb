module BAMFCSV
  class Table
    include Enumerable
    def initialize(matrix)
      @headers = matrix.shift
      @matrix = matrix
      @table_cache = []
    end

    def each
      @matrix.each_with_index do |row, idx|
        yield self[idx]
      end
    end

    def [](idx)
      idx += @matrix.size if idx < 0
      return if idx >= @matrix.size || idx < 0
      @table_cache[idx] ||= row_hash(@matrix[idx])
    end

    def inspect
      "#<BAMFCSV::Table>"
    end

    private
    def row_hash(row)
      Hash[@headers.zip(row)]
    end
  end
end
