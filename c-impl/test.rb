#/usr/bin/env ruby

require 'benchmark'

$target = "./db"

def echo(cmd)
    f = IO.popen("echo '#{cmd}' | #{$target}")
    return f.readlines
end

def run_cmds(commands)
    raw_output = nil
    IO.popen($target, "r+") do |pipe|
        commands.each do |cmd|
            pipe.puts cmd
        end

        pipe.close_write

        # Read entire output
        raw_output = pipe.gets(nil)
    end
    if raw_output != nil
        return raw_output.split("\n")
    end
    return nil
end

def expect_match_array(arr1, arr2)
    if arr1.length != arr2.length
        puts "length not equal!!"
        return false
    end
    arr1.each_with_index do |v, i|
        if v != arr2[i]
            return false
        end
    end
    return true
end

def expect_eq(str1, str2)
    if str1 == str2
        return true
    end
    return false
end

def test1()
    cmds = [
        "insert 1 user1 person1@example.com",
        "select",
        ".exit",
    ]
    ress = [
        "db > Executed.",
        "db > (1, user1, person1@example.com)",
        "Executed.",
        "db > ",
    ]
    result = run_cmds(cmds)
    if result == nil
        puts "test failed! result is nil."
        return
    end
    if expect_match_array(result, ress)
        puts "test pass! OK!!"
        return
    end
    puts "test failed!"

    puts "====="
    puts result
    puts "====="
    puts ress
    puts "====="
end

def test2()
    # cmds = (1..1401).map do |i|
    #     "insert #{i} user#{i} person#{i}@example.com"
    # end
    username = "xxxxxaaaaavvvvvsssssqqqqqwwwwwsd"
    email = "test@xx.com"
    cmds = [
        "insert 1 #{username} #{email}",
        "select",
    ]
    cmds << ".exit"

    ress = [
        "db > Executed.",
        "db > (1, #{username}, #{email})",
        "Executed.",
        "db > ",
    ]

    result = run_cmds(cmds)
    if result == nil
        puts "test failed! result is nil."
        return
    end
    if expect_match_array(result, ress)
        puts "test pass! OK!!"
        return
    end
    puts "test failed!"
end

def test()
    puts Benchmark.measure {
        test1()
    }
    puts Benchmark.measure {
        test2()
    }
end

test