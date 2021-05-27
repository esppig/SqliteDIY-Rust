#/usr/bin/env mruby

$target = "./db"

def echo(cmd)
    f = IO.popen("echo '#{cmd}' | #{$target}")
    return f.readlines
end

def test1()
    res = echo(".test")
    # puts res

    # res.each do |msg|
    #     puts msg
    # end

    if res[0] == "db> Unrecoginzed command '.test'. \n" and
        res[1] == "db> Error reading input\n"
        puts "OK!"
    end

    res = echo(".exit")

    if res[0] == "db> "
        puts "OK!!"
    end
end

def test2()
    res = echo("insert foo bar")
    if res[0] == "db> This is where we would do an insert.\n" and
        res[1] == "Executed.\n"
        puts "OK!"
    end

    res = echo("delete foo")
    if res[0] == "db> Unrecognized keyword at start of 'delete foo'.\n"
        puts "OK!"
    end

    res = echo("select")
    if res[0] == "db> This is where we would do a select.\n" and
        res[1] == "Executed.\n"
        puts "OK!"
    end

    res = echo(".tables")
    if res[0] == "db> Unrecognized command '.tables'\n" and
        puts "OK!"
    end

    res = echo(".exit")
    if res[0] == "db> "
        puts "OK!!"
    end
end

def test3()
    res = echo("insert 1 c/c++ xiaoming@qq.com")
    if res[0] == "db> Executed.\n"
        puts "OK!"
    end

    res = echo("insert 2 rust xiaohui@qq.com")
    if res[0] == "db> Executed.\n"
        puts "OK!"
    end

    res = echo("insert 3 haskell xiaobai@qq.com")
    if res[0] == "db> Executed.\n"
        puts "OK!"
    end

    res = echo("select")
    if res[0] == "db> Executed.\n"
        puts "OK!"
    end

    res = echo(".exit")
    puts res
    if res[0] == "db> "
        puts "OK!!"
    end

end

def testx()
    IO.popen($target, "r+") do |pipe|

        pipe.write "select\n"
        3.times {puts pipe.gets }

        pipe.write "insert 1 cc xiaoming@cell.com\n"
        3.times {puts pipe.gets}

        pipe.write ".exit\n"

        # pipe.puts "select\n"
        # pipe.puts "__END__"
        # pipe.gets

        # pipe.puts "insert 1 cc xiaoming@cell.com\n"
        # pipe.puts "__END__"
        # pipe.gets

        # pipe.puts "select\n"
        # pipe.puts "__END__"
        # pipe.gets
    end
end

def test() 
    # test1()
    # test2()
    # test3()
    testx
end

test