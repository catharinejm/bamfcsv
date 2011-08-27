require 'bamfcsv/bamfcsv'
require 'bamfcsv/table'

module BAMFCSV

  def self.read(thing_to_read, opts={})
    parse(File.read(thing_to_read), opts)
  end

  def self.parse(csv_str, opts={})
    return [] if csv_str.empty?
    # We need to do this because the C extension currently overwrites
    # the input, and all of String#clone, String#dup, and String.new
    # copy the pointer, not the contents. So we make a copy, parse
    # that, and throw away the copy.
    copy = "" + csv_str
    separator = opts.fetch(:separator, ',')
    raise InvalidSeparator, "Separator must be exactly one character long" if separator.size != 1
    raise InvalidSeparator, "Separator cannot be '\"'" if separator == '"'
    matrix = __parse_string(copy, separator)
    if opts[:headers]
      Table.new(matrix)
    else
      matrix
    end
  end

  class InvalidSeparator < StandardError; end
end
