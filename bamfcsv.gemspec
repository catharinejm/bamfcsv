# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "bamfcsv/version"

Gem::Specification.new do |s|
  s.name        = "bamfcsv"
  s.version     = BAMFCSV::VERSION
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Jon Distad", "Alex Redington"]
  s.email       = ["jon@thinkrelevance.com", "lovemachine@thinkrelevance.com"]
  s.homepage    = "https://github.com/jondistad/bamfcsv"
  s.summary     = %q{BAMF!!! Your csv is parsed.}
  s.description = %q{BAMFCSV parses csv like a BAMF. BAMF!!}

  s.rubyforge_project = "bamfcsv"

  s.files         = `git ls-files`.split("\n")
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.require_paths = ["lib", "ext"]
  s.extensions = %w{ext/bamfcsv/extconf.rb}
end
