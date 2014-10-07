PROGRAM = File.expand_path("../../src/build/AcDc", __FILE__)
TEST_SUITE_DIR = "../../test"
TEST_NUMBER = 6

def expand_test_dir(str)
  File.expand_path(File.join(TEST_SUITE_DIR, str), __FILE__)
end

error = 0
(1..TEST_NUMBER).each do |i|
  input = expand_test_dir("sample" + i.to_s + ".ac")
  output = expand_test_dir("myAnswer" + i.to_s + ".dc")

  `#{PROGRAM} #{input} #{output}`

  answer = expand_test_dir("sample" + i.to_s + ".dc")

  diff = `diff #{output} #{answer}`

  if diff != ""
    error += 1
    puts "test number #{i} is wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    puts diff
  end
end
puts "****************************************************"
puts "#{error} errors occur, total #{TEST_NUMBER} test case"
puts "****************************************************"
