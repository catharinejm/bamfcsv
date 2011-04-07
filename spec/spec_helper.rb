root_path = File.expand_path(File.join(File.dirname(__FILE__), '..'))
$:.unshift(File.join(root_path, "lib"))
$:.unshift(File.join(root_path, "ext"))
require 'bundler'
require 'bamfcsv'
Bundler.require(:development)
require 'csv'

RSpec.configure do |config|
  config.filter_run :focused => true
  config.alias_example_to :fit, :focused => true
  config.run_all_when_everything_filtered = true
end
