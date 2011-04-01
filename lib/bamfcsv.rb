require 'bamfcsv/bamfcsv'

module BAMFCSV

  def self.read(thing_to_read)
    if  String === thing_to_read
      raise ArgumentError.new("#{thing_to_read} is not a flat file.") unless File.file? thing_to_read
      read_path(thing_to_read)
    end
  end

end
