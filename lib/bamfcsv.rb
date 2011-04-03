require 'bamfcsv/bamfcsv'

module BAMFCSV

  def self.read(thing_to_read)
    __parse_string(File.read(thing_to_read))
  end

  def self.parse(csv_str)
    __parse_string(csv_str)
  end

  def self.rb_read(file)
    parsed = []
    cur_row = []
    cur_cell = ""
    quote_count = 0
    File.read(file).each_char do |cur|
      if cur == '"'
        if quote_count.zero? && !cur_cell.empty?
          raise MalformedCSVError, "Illegal quoting on line #{parsed.size+1}, cell #{cur_row.size+1}"
        else
          quote_count += 1
        end
      end

      if quote_count.even?
        if cur == ","
          if quote_count > 0 && cur_cell[-1] != '"'
            raise MalformedCSVError, "Unclosed quoted field on line #{parsed.size+1}, cell #{cur_row.size+1}."
          end

          cur_cell.gsub!('""', '"')
          cur_cell.chomp!
          cur_row << cur_cell
          cur_cell = ""
          quote_count = 0
        elsif cur == "\n"
          if quote_count > 0 && !(cur_cell[-1] == '"' || cur_cell[-1] == "\r" && cur_cell[-2] == '"')
            raise MalformedCSVError, "Unclosed quoted field on line #{parsed.size+1}, cell #{cur_row.size+1}: EOL"
          end

          cur_cell.gsub!('""', '"')
          cur_cell.chomp!
          cur_row << cur_cell
          cur_cell = ""
          parsed << cur_row
          cur_row = []
          quote_count = 0
        else
          cur_cell << cur
        end
      end
    end

    if quote_count.odd?
      raise MalformedCSVError, "Illegal quoting on line #{parsed.size+1}, cell #{cur_row.size+1}: File ends without closing '\"'"
    elsif quote_count > 0 && cur != '"'
      raise MalformedCSVError, "Unclosed quoted field on line #{parsed.size+1}, cell #{cur_row.size+1}: EOF"
    end

    if cur_row.empty?
      parsed.pop
    else
      cur_cell.gsub! '""', '"'
      cur_cell.chomp!
      cur_row << cur_cell
      parsed << cur_row
    end

    parsed
  end

end
