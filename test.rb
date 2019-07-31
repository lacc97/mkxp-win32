require './win32.rb'

FindWindow = Win32API.new('user32', 'FindWindow', %w(p p), 'l')

GetUserName = Win32API.new('Advapi32', 'GetUserName', %w(p p), 'i')

def user_name
    str = "\0" * 257
    len = [256].pack('i')
    return nil unless GetUserName.call(str, len) != 0
    str.strip
end

puts user_name
