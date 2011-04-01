require 'bamfcsv/bamfcsv'

module BAMFCSV

  def self.read(thing_to_read)
    if  String === thing_to_read
      raise Errno::ENOENT.new("#{thing_to_read} does not exist") unless File.exist? thing_to_read
      raise Errno::EISDIR.new("#{thing_to_read} is a directory") if File.directory? thing_to_read
      read_path(thing_to_read)
    end
  end

end
