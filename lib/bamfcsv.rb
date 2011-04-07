require 'bamfcsv/bamfcsv'
require 'bamfcsv/table'

module BAMFCSV

  def self.read(thing_to_read, opts={})
    parse(File.read(thing_to_read), opts)
  end

  def self.parse(csv_str, opts={})
    return [] if csv_str.empty?
    csv_str.chomp!
    matrix = __parse_string(csv_str)
    if opts[:headers]
      Table.new(matrix)
    else
      matrix
    end
  end

end
