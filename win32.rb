# gem_path_str = %x(gem env gempath)
# puts gem_path_str
# gem_paths = gem_path_str.strip.split(":")
# puts gem_paths
# 
# gem_paths.each { |gp| $LOAD_PATH.unshift(gp) }
# puts $LOAD_PATH

$LOAD_PATH.unshift("/usr/lib64/ruby/2.6.0/")
$LOAD_PATH.unshift("/usr/lib64/ruby/2.6.0/x86_64-linux/")
$LOAD_PATH.unshift("/usr/lib64/ruby/site_ruby/2.6.0/")

require 'rubygems'
require 'ffi'

module LibC
    extend FFI::Library
    ffi_lib FFI::Library::LIBC

    attach_function :malloc, [:size_t], :pointer
    attach_function :calloc, [:size_t], :pointer
    attach_function :valloc, [:size_t], :pointer
    attach_function :realloc, [:pointer, :size_t], :pointer
    attach_function :free, [:pointer], :void

    attach_function :memcpy, [:pointer, :pointer, :size_t], :pointer
    attach_function :bcopy, [:pointer, :pointer, :size_t], :void
end

module Win32API
    extend FFI::Library
    ffi_lib FFI::CURRENT_PROCESS

    def new(dllName, fnName, argList, retType)
        symName = dllName.downcase + "_" + fnName
        symSym = symName.to_sym
        
        param_transform = lambda do |arg|
            case arg.to_s.downcase
            when 'p'
                :pointer
            when 'l'
                :long
            when 'n', 'i'
                :int
            when 'v'
                :void
            else
                nil
            end
        end
        
        
        if argList.is_a? String # TODO remove spaces from string
            argList = argList.chars
        end
        
        paramList = argList.map{|arg| param_transform.call(arg)}
        paramList.delete :void
        
        retType = param_transform.call(retType)
        
        Win32API.attach_function symSym, paramList, retType
        
        lambda do |*args|
            args.each_index do |i|
                if paramList[i].equal? :pointer and args[i].equal? 0
                    args[i] = nil
                elsif paramList[i].equal? :int and args[i] >= 2**31
                    args[i] = args[i] - 2**32
                end
            end
            
            Win32API.method(symSym).call(*args)
        end
#         Win32API.method(symSym)
    end

    module_function :new
end
