require 'bamfcsv/bamfcsv'

module BAMFCSV

  def self.read(thing_to_read)
    __parse_string(File.read(thing_to_read))
  end

  def self.parse(csv_str)
    __parse_string(csv_str)
  end

end
