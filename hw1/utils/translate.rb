PROGRAM = File.expand_path("../../src/build/AcDc", __FILE__)
TEST_SUITE_DIR = "../../test"
TEST_NUMBER = 8

def expand_test_dir(str)
  File.expand_path(File.join(TEST_SUITE_DIR, str), __FILE__)
end

(1..TEST_NUMBER).each do |i|
  input = expand_test_dir("sample" + i.to_s + ".dc")
  output = expand_test_dir("standard_dc_ans" + i.to_s)
  `dc #{input} > #{output}`
end
