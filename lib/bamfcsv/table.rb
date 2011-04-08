module BAMFCSV
  class Table
    include Enumerable
    def initialize(matrix)
      @headers = matrix.shift
      @matrix = matrix
      @header_map = {}
      @headers.each_with_index do |h, i|
        @header_map[h] = i
      end
      @row_cache = []
    end

    def each
      if block_given?
        @matrix.size.times do |idx|
          yield self[idx]
        end
      end
      self
    end

    def [](idx)
      idx += @matrix.size if idx < 0
      return if idx < 0 || idx >= @matrix.size
      @row_cache[idx] ||= Row.new(@header_map, @matrix[idx])
    end

    def inspect
      "#<BAMFCSV::Table>"
    end

    private
    def row_hash(row)
      Hash[@headers.zip(row)]
    end

    class Row
      def initialize(header_map, values)
        @header_map = header_map
        @values = values
      end

      def [](key)
        @values[@header_map[key]]
      end
    end
  end
end
