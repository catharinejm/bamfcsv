require "rake/extensiontask"

def gemspec
  @clean_gemspec ||= eval(File.read(File.expand_path('../../bamfcsv.gemspec', __FILE__)))
end

Rake::ExtensionTask.new("bamfcsv", gemspec) do |ext|
  ext.lib_dir = File.join 'lib', 'bamfcsv'
  CLEAN.include "#{ext.lib_dir}/*.#{RbConfig::CONFIG['DLEXT']}"
end
Rake::Task[:spec].prerequisites << :compile
