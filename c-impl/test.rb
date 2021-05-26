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

def test() 
    # test1()
    test2()
end

test